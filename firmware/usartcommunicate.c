/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#include "datastructure/datastructure.h"
#include "usartcommunicate.h"

/* Have the TX & RX queue container to hold the status of TX & RX buffer. */
static SDS_QUEUE txq, rxq;
/* Have the PCB to hold the callback function pointer & parameters after  */
/* sending & reading finished.                                            */
SS_PCB txPCB, rxPCB;
/* Have the timeout callback function pointer & parameter in case. */
SS_PCB toPCB;
/* Have the counter for timeout counting. */
uint8_t toCount = 0;

/* Flag to represent the USART TX is used or not. */
uint8_t isSending = 0;
/* Flag to represent the USART RX is used or not. */
uint8_t isReading = 0;
/* Flag to represent the USART communication time out. */
uint8_t isEnableTimeout = 0;

/* Send a memory buffer asynchronously. */
uint8_t USARTCommSend(uint8_t *m, uint8_t len, void *p, SS_CB cb) {
	/* Flag to represent this sending is submitted or not. */
	uint8_t isSubmitted = 1;

	/* Check the USART TX is not used. */
	if(isSending == 0) {
		isSending = 1;
		USARTEnableTXCInterrupt();
		/* Save the parameters and the callback function.  */
		txPCB.p = p;
		txPCB.cb = cb;
		/* Bind the memory buffer with the queue. */
		SDSInitQueue(&txq, len, m);
		SDSSetIndex(&txq, len, 0);
		/* Send first byte through USART. */
		USARTSend1Byte(m[0]);
		/* Pop the first element of the queue. */
		SDSPopQueue(&txq);
	}
	else {
		/* USART TX is used, so this sending is failed. */
		isSubmitted = 0;
	}

	return isSubmitted;
}

/* Read into a memory buffer asynchronously. */
uint8_t USARTCommRead(uint8_t *m, uint8_t len, void *p, SS_CB cb) {
	/* Flag to represent this reading is submitted or not. */
	uint8_t isSubmitted = 1;

	/* Check the USART RX is not used. */
	if(isReading == 0) {
		isReading = 1;
		USARTEnableRXCInterrupt();
		/* Save the parameters and the callback function. */
		rxPCB.p = p;
		rxPCB.cb = cb;
		/* Bind the memory buffer with the queue. */
		SDSInitQueue(&rxq, len, m);
	}
	else {
		/* USART RX is used, so this sending is failed. */
		isSubmitted = 0;
	}

	return isSubmitted;
}

/* Event of USART had sent all of the bytes. */
inline void USARTCommSendFinishedEvent(void) {
	USARTDisableTXCInterrupt();
	if(txPCB.cb != NULL)
		/* Admit the callback function for USART sending finished. */
		SSAdmitJob(txPCB.p, txPCB.cb);
	/* Release the USART TX used flag. */
	isSending = 0;
}

/* Event of USART had read all of the bytes. */
inline void USARTCommReadFinishedEvent(void) {
	USARTDisableRXCInterrupt();
	if(rxPCB.cb != NULL)
		/* Admit the callback function for USART reading finished. */
		SSAdmitJob(rxPCB.p, rxPCB.cb);
	/* Release the USART RX used flag. */
	isReading = 0;
}

/* Work of USART TX had sent a byte need to do. */
void USARTCommSendWork(void) {
	/* Error code. */
	static uint8_t err = 0;
	uint8_t *m;

	USARTDisableTXCInterrupt();

	/* Have the next element to send through USART. */
	m = SDSFrontQueue(&txq, 1, &err);
	
	/* Check have the next element. */
	if(err == SDS_OK) {
		/* Send the element through USART. */
		USARTSend1Byte(*m);
		/* Pop the element. */
		SDSPopQueue(&txq);
	}
	else {
		/* All of the elements are sent through USART. */
		USARTCommSendFinishedEvent();
	}

	USARTEnableTXCInterrupt();
}

/* Work of USART RX had read a byte need to do. */
void USARTCommReadWork(void) {
	/* Error code. */
	static uint8_t err = 0;
	uint8_t m;

	USARTDisableRXCInterrupt();

	if(isReading) {
		/* Save the element read from USART. */
		m = USARTRead1Byte();
		err = SDSPushQueue(&rxq, &m, 1);

		/* Check have read enough elements. */
		if(SDSSize(&rxq) >= rxq.len) {
			/* All of the elements are read from USART. */
			USARTCommReadFinishedEvent();
		}
	}
	/* Chage the status from wait first byte to count time out instantly. */
	if(isEnableTimeout == 2)
		isEnableTimeout = 1;

	USARTEnableRXCInterrupt();
}

/* Send a memory buffer with blocking. */
uint8_t USARTCommSendBlocking(uint8_t *m, uint8_t len) {
	uint8_t i = 0;
	
	/* Check the USART sending channel is not used. */
	if(isSending == 0) {
		isSending = 1;
		/* Send memoery bytes through USART with blocking. */
		for(i = 0; i < len; i++)
			USARTSend1ByteBlocking(m[i]);
		isSending = 0;
	}

	return i;
}

/* Read into memory buffer with blocking. */
uint8_t USARTCommReadBlocking(uint8_t *m, uint8_t len) {
	uint8_t i = 0;

	/* Check the USART reading channel is not used. */
	if(isReading == 0) {
		isReading = 1;
		/* Read memoery bytes from USART with blocking. */
		for(i = 0; i < len; i++) {
			m[i] = USARTRead1ByteBlocking();
		}
		isReading = 0;
	}

	return i;
}

/* Set time out time and the callback function. */
void USARTCommSetTimeout(uint8_t t, void *tp, SS_CB tcb) {
	/* Save the timeout parameters and the callback function. */
	toPCB.p = tp;
	toPCB.cb = tcb;
	toCount = t;
	/* Set the flag status to count time out instantly. */
	isEnableTimeout = 1;
}

/* Set time out time and the callback function after read first byte. */
void USARTCommSetShiftTimeout(uint8_t t, void *tp, SS_CB tcb) {
	/* Save the timeout parameters and the callback function. */
	toPCB.p = tp;
	toPCB.cb = tcb;
	toCount = t;
	/* Set the flag status to count time out after read first byte. */
	isEnableTimeout = 2;
}

/* Event of USART communication time out. */
inline void USARTCommTimeoutEvent(void) {
	rxPCB = toPCB;
	isEnableTimeout = 0;
	USARTCommReadFinishedEvent();
}

/* Work of system tick to check time out. */
void USARTCommTimeoutWork(void) {
	if((isEnableTimeout & 0x01) && ((--toCount) <= 0))
		USARTCommTimeoutEvent();
}
