/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

#include <timer.h>

/* Define the counter's top for 10/9ms with 1024 clocks prescaler. */
#define SYSTIMER_TOP10MS	F_CPU/1024/100
#define SYSTIMER_DIVDER		9
#define SYSTIMER_TICK	(SYSTIMER_TOP10MS/SYSTIMER_DIVDER)

/* Intial system timer clock. */
#define SYSTimerInit() ({ \
	/* Initial timer with CTC mode. */ \
	Timer2Init(); \
	/* Set the timer period for 10/9ms. */ \
	Timer2SetTop(SYSTIMER_TICK); \
	/* Enable timer interrupt. */ \
	Timer2EnableOCInterrupt(); })
/* Start system timer. */
#define SYSTimerStart()	Timer2SetPrescaler(TIMER2_CLOCK_PRESCALER_D1024)
/* Stop system timer. */
#define SYSTimerStop()	Timer2Stop()
/* Define the system timer tick event. */
#define SYSTimerTickEvent()	Timer2CompareEvent()

#endif
