/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <util/crc16.h>
#include "scheduler/scheduler.h"
#include "systimer.h"
#include "usartcommunicate.h"
#include "adcsample.h"
#include "modbus.h"
#include "process.h"

/* Set the station address of MODBUS. */
static uint8_t station = '0';

uint16_t crc;

/* Calculate MODBUS CRC with old CRC. */
inline uint16_t UpdateCRC(uint16_t c, uint8_t *p, uint8_t len) {
	uint8_t i;

	for(i = 0; i < len; i++)
		c = _crc16_update(c, p[i]);

	return c;
}

/* Calculate new MODBUS CRC. */
#define NewCRC(p, len)	UpdateCRC(0xFFFF, p, len)

void SendCRC(void *p) {
	if(!USARTCommSend((uint8_t *)p, sizeof(crc), NULL, WaitForCommandJob))
		SSAdmitJob(p, SendCRC);
}

/* Start Sample. */
void StartSampleJob(void *p) {
	MODBUS_RTU *rtu = p;

	if(crc == *(rtu->crc)) {
		StartSampling();
		if(!USARTCommSend(rtu->top, 4, NULL, WaitForCommandJob))
			SSAdmitJob(rtu, StartSampleJob);
	}
	else
		SSAdmitJob(NULL, WaitForCommandJob);
}

/* Stop Sample. */
void StopSampleJob(void *p) {
	MODBUS_RTU *rtu = p;

	if(crc == *(rtu->crc)) {
		StopSampling();
		if(!USARTCommSend(rtu->top, 4, NULL, WaitForCommandJob))
			SSAdmitJob(rtu, StopSampleJob);
	}
	else
		SSAdmitJob(NULL, WaitForCommandJob);
}

/* Send ADC samples once. */
void SendOnceSampleJob(void *p) {
	static uint8_t i = 1;
	ADCVALUE *adcs = p;
	static ADCVALUE v;
	static SS_PCB pcb;

	/* Have next one ADC value package if this is new sending. */
	if(adcs == NULL) {
		v = *(ADCSampleRead());
		adcs = &v;
	}
	/* Check this is the last one of the continue sending. */
	if(i >= SEND_NUMBER_OF_SAMPLES) {
		i = 0;
		pcb.cb = SendCRC;
		pcb.p = &crc;
	}
	/* Send the ADC value package */
	if(USARTCommSend((uint8_t *)adcs, sizeof(ADCVALUE), \
					 &pcb, ADCSampleReadFinishedJob)) {
		i++;
		crc = UpdateCRC(crc, (uint8_t *)adcs, sizeof(ADCVALUE));
	}
	else {
		/* Re-send again if send failed. */
		SSAdmitJob(adcs, SendOnceSampleJob);
	}
}

/* Send how many samples left. */
void SendLeft(void *p) {
	if(USARTCommSend((uint8_t *)p, 1, &crc, SendCRC))
		crc = _crc16_update(crc, *((uint8_t *)p));
	else
		SSAdmitJob(p, SendLeft);
}

/* Send samples. */
void SendSamplesJob(void *p) {
	MODBUS_RTU *rtu = p;
	static uint8_t left;

	if(crc == *(rtu->crc)) {
		/* Have the amount of the samples left. */
		left = ADCSampleLeft();
		/* Check there are enough samples to be send. */
		if(left >= (SEND_NUMBER_OF_SAMPLES + 1)) {
			if(!USARTCommSend(rtu->top, 2, NULL, SendOnceSampleJob))
				SSAdmitJob(rtu, SendSamplesJob);
		}
		else {
			/* There's not enough samples to be send, and send how many left. */
			rtu->header->fcode |= 0x80;
			if(USARTCommSend(rtu->top, 2, &left, SendLeft))
				crc = NewCRC(rtu->top, sizeof(MODBUS_RTUHEADER));
			else
				SSAdmitJob(rtu, SendSamplesJob);
		}
	}
}

/* Show scheduler status. */
void ShowDebugJob(void *p) {
	MODBUS_RTU *rtu = p;
	static uint8_t buf[9];
	SS_SSTATUS st;

	if(crc == *(rtu->crc)) {
		StopSampling();
		st = SSDebug();

		buf[0] = rtu->header->address;
		buf[1] = rtu->header->fcode;
		buf[2] = st.len;
		buf[3] = st.size;
		buf[4] = st.err;
		buf[5] = st.run;
		buf[6] = ADCSampleLeft();

		crc = NewCRC(buf, 7);

		*((uint16_t *)(buf + 7)) = crc;

		if(!USARTCommSend(buf, sizeof(buf), NULL, WaitForCommandJob))
			SSAdmitJob(NULL, ShowDebugJob);
	}
}

/* Dummy function1. */
void DealJob(void *p) {
	MODBUS_RTU *rtu = p;

	crc = NewCRC((uint8_t *)(rtu->header), 18);
	if(crc == *(rtu->crc)) {
		if(!USARTCommSend(rtu->top, 20, NULL, WaitForCommandJob))
			SSAdmitJob(rtu, DealJob);
	}
	else
		SSAdmitJob(NULL, WaitForCommandJob);
}

/* Default work. */
void DefaultWork(void *p) {
	MODBUS_RTU *rtu = p;

	rtu->crc = (uint16_t *)(rtu->payload + 16);
	if(USARTCommRead(rtu->payload + 2, 16, rtu, DealJob))
		USARTCommSetTimeout(MODBUS_TIMEOUTTIME, NULL, WaitForCommandJob);
	else
		SSAdmitJob(rtu, CheckStationJob);
}

/* Parse command. */
inline void ParseCommand(MODBUS_RTU *rtu) {
	/* Calculate the read RTU header CRC. */
	crc = NewCRC(rtu->top, sizeof(MODBUS_RTUHEADER));
	/* Do the work according to the function code. */
	switch(rtu->header->fcode) {
		case 's':
			/* Start sampling. */
			StartSampleJob(rtu);
			break;
		case 'p':
			/* Stop sampling. */
			StopSampleJob(rtu);
			break;
		case 'r':
			/* Send samples. */
			SendSamplesJob(rtu);
			break;
		case 'd':
			/* Show scheduler status. */
			ShowDebugJob(rtu);
			break;
		default:
			/* Default work. */
			DefaultWork(rtu);
	}
}

/* Check station. */
void CheckStationJob(void *p) {
	MODBUS_RTU *rtu = p;

	/* Check this station is being indexed. */
	if((rtu->header->address == station) || (rtu->header->address == 0))
		ParseCommand(rtu);
	else
		SSAdmitJob(NULL, WaitForCommandJob);
}

/* Wait the command from USART. */
void WaitForCommandJob(void *p) {
	static uint8_t payload[20];
	static MODBUS_RTU rtu;

	/* <------------------------- Payload 20 bytes -------------------------> */
	/* |Address(1 byte)|Func. code(1 byte)|Payload(0 ~ 16 byte)|CRC(2 bytes)| */
	rtu.header = (MODBUS_RTUHEADER *)payload;
	rtu.payload = payload + 2;
	rtu.crc = (uint16_t *)(payload + 2);

	/* 1st and 2nd is header, 3rd and 4th may be payload or CRC,              */
	/* so total length >= 4 bytes.  -> Read 4 bytes first.                    */
	if(USARTCommRead(payload, 4, &rtu, CheckStationJob))
		USARTCommSetShiftTimeout(MODBUS_TIMEOUTTIME, NULL, WaitForCommandJob);
	else
		SSAdmitJob(NULL, WaitForCommandJob);
}
