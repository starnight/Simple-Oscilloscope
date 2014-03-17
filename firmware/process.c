/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#include <stdint.h>
#include "adcsample.h"
#include "usartcommunicate.h"
#include "systimer.h"
#include "process.h"

/* Flag to do sample or not. */
static uint8_t isSampling = 0;

/* Work of one timer tick need to do. */
void SYSTimerTickWork(void) {
	static uint8_t c = 0;

	/* Check need to ADC sample each tick. */
	if((isSampling) && (c < AMOUNT_OF_ADC_CHANNEL)) {
		/* Have all ADC channels sample around once. */
		ADCSampleStart();
	}
	/* Check USART communication time out each tick. */
	if(c < SYSTIMER_DIVDER) {
		c++;
	}
	else {
		c = 0;
		USARTCommTimeoutWork();
	}
}

/* Set the sampling flag. */
void StartSampling(void) {
	isSampling = 1;
}

/* Clear the sampling flag. */
void StopSampling(void) {
	isSampling = 0;
}
