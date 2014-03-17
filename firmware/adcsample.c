/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#include "datastructure/datastructure.h"
#include "scheduler/scheduler.h"
#include "adcsample.h"

/* Have a ADC memory buffer. */
static ADCVALUE _adcBuf[ADC_SAMPLE_BUFLEN];
/* Make the ADC memory buffer act as a ring buffer. */
SDS_RING adcBuf;
uint8_t isPopped = 0;
uint8_t channel = 0;

/* Initial the ADC sampling module. */
void ADCSampleInit(void) {
	/* Initial and bind the ADC memory buffer. */
	SDSInitRing(&adcBuf, ADC_SAMPLE_BUFLEN, &_adcBuf);
	/* Initial the ADC hardware. */
	ADCInit();
	/* Enable ADC complete interrupt. */
	ADCEnableInterrupt();
}

/* Start ADC sampling. */
void ADCSampleStart(void) {
	ADCSampleOnce(channel);
}

/* Job of all of the ADC channels had sampled once. */
void ADCSampleFinishedJob(void *data) {
	/* Push one package of ADC samples into the memory buffer. */
	if(SDSPushRing(&adcBuf, data, sizeof(ADCVALUE)) == SDS_BUFFEROVERFLOW) {
		/* Pop the oldest and push the lastest, if the buffer is full. */
		SDSPopRing(&adcBuf);
		isPopped = 1;
		SDSPushRing(&adcBuf, data, sizeof(ADCVALUE));
	}
}

/* Work of one ADC channel finished need to do. */
void ADCSampleWork(void) {
	static ADCVALUE adcs;
	
	adcs.adc[channel] = ADCGetResult();

	/* Change to next ADC channel. */
	channel += 1;
	/* Check all ADC channels are gone through. */
	if(channel >= AMOUNT_OF_ADC_CHANNEL) {
		/* All ADC channels have sampled finished.                       */
		/* Admit the ADC finished callback function with sampled values. */
		channel = 0;
		ADCSampleFinishedJob(&adcs);
	}
	ADCChooseChannel(channel);
}

/* Read one package of ADC samples. */
ADCVALUE * ADCSampleRead(void) {
	uint8_t pos;

	/* Calculate the 2nd next ADC values position. */
	if(adcBuf.inpos < (adcBuf.len - 1))
		pos = adcBuf.inpos + 1;
	else
		pos = adcBuf.inpos + 1 - adcBuf.len;
	isPopped = 0;

	return _adcBuf + pos;
}

/* Job of ADC value is read finished. */
void ADCSampleReadFinishedJob(void *p) {
	SS_PCB *pcb = p;

	(isPopped) ? (isPopped = 0) : SDSPopRing(&adcBuf);
	SSAdmitJob(pcb->p, pcb->cb);
}

/* Have the number of samples left. */
uint8_t ADCSampleLeft(void) {
	return SDSSize(&adcBuf);
}
