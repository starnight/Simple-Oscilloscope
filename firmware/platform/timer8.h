/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __AVR_ATMEGA8_TIMER_H__
#define __AVR_ATMEGA8_TIMER_H__

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/* Choose prescaler factor for timer#2 clock. */
#define TIMER2_NO_CLOCK_SOURCE			0
#define TIMER2_CLOCK_NO_PRESCALER		1
#define TIMER2_CLOCK_PRESCALER_D8		2
#define TIMER2_CLOCK_PRESCALER_D32		3
#define TIMER2_CLOCK_PRESCALER_D64		4
#define TIMER2_CLOCK_PRESCALER_D128		5
#define TIMER2_CLOCK_PRESCALER_D256		6
#define TIMER2_CLOCK_PRESCALER_D1024	7

/* Enable timer#2 output compare match interrupt. */
#define Timer2EnableOCInterrupt()	(TIMSK |= (1 << OCIE2))
/* Disable timer#2 output compare match interrupt. */
#define Timer2DisableOCInterrupt()	(TIMSK &= ~(1 << OCIE2))
/* Initial timer#2 with CTC mode. */
#define Timer2Init() ({ \
	/* Set timer#2 CTC and OC2 disconnected mode.*/ \
	TCCR2 = (1 << WGM21) | (0 << WGM20) | (0 << COM20); \
	/* Zero timer#2 counter. */ \
	TCNT2 = 0; })
/* Set the clock prescaler for timer#2 clock. */
#define Timer2SetPrescaler(prescaler)	(TCCR2 = (TCCR2 & 0xF8) | prescaler)
/* Set the counting TOP for timer#2. */
#define Timer2SetTop(top)	(OCR2 = top - 1)
/* Stop timer#2 counting. */
#define Timer2Stop()	Timer2SetPrescaler(TIMER2_NO_CLOCK_SOURCE)
/* Define the interrupt service routine for timer#2 comparing matched. */
#define Timer2CompareEvent()	ISR(TIMER2_COMP_vect)

#endif
