/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#include <stdint.h>
#include <avr/interrupt.h>
#include "scheduler/scheduler.h"
#include "systimer.h"
#include "adcsample.h"
#include "usartcommunicate.h"
#include "process.h"
#include "modbus.h"

/* Timer interrupt each 10ms. */
SYSTimerTickEvent() { SYSTimerTickWork(); }
/* USART TX complete interrupt. */
USARTCommSendEvent() { USARTCommSendWork(); }
/* USART RX complete interrupt. */
USARTCommReadEvent() { USARTCommReadWork(); }
/* ADC interrupt. */
ADCSampleChannelEvent() { ADCSampleWork(); }
/* For useless interrupt. */
ISR(BADISR_vect) {}

int main(void) {
	/* Inital system timer module. */
	SYSTimerInit();
	/* Initial the ADC sampling module. */
	ADCSampleInit();
	/* Initial the USART communication module. */
	USARTCommInit();
	/* Initial the simple scheduler (System scheduler). */
	SSInit();

	USARTCommSendBlocking((uint8_t *)"Initial finished!\r\n", 19);

	/* Admit the wait command job for reading from USART. */
	SSAdmitJob(NULL, WaitForCommandJob);

	/* Enable the global interrupt. */
	sei();

	/* Start system timer. */
	SYSTimerStart();

	/* Going to the simple scheduler main loop. */
	SSMainLoop();

	return 0;
}
