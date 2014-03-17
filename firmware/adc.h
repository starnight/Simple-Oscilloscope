/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __AVR_ADC_H__
#define __AVR_ADC_H__

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/* Choose the voltage reference. */
#define ADC_AREF_WITH_OUTREF	0
#define ADC_AREF_WITH_AVCC		1
#define ADC_AREF_WITH_INTERNAL	3
/* Set the default ADC reference. */
#ifndef ADC_REFERENCE
#define ADC_REFERENCE	ADC_AREF_WITH_INTERNAL
#endif

/* Choose for ADC data right or left adjust */
#define ADC_WITH_R_ADJ	0
#define ADC_WITH_L_ADJ	1
/* Set the default ADC data adjust. */
#ifndef ADC_ADJUST
#define ADC_ADJUST		ADC_WITH_R_ADJ
#endif

/* Choose prescaler factor for ADC clock. */
#define ADC_CLOCK_PRESCALER_D2		1
#define ADC_CLOCK_PRESCALER_D4		2
#define ADC_CLOCK_PRESCALER_D8		3
#define ADC_CLOCK_PRESCALER_D16		4
#define ADC_CLOCK_PRESCALER_D32		5
#define ADC_CLOCK_PRESCALER_D64		6
#define ADC_CLOCK_PRESCALER_D128	7
/* Set the default prescaler factor for ADC clock. */
#ifndef ADC_CLOCK_PRESCALER_FACTOR
#define ADC_CLOCK_PRESCALER_FACTOR	ADC_CLOCK_PRESCALER_D32
#endif

/* Initial ADC. */
#define ADCInit() ({ \
	/* Set the ADC reference and data register adjust. */ \
	ADMUX = (ADC_REFERENCE << REFS0) | (ADC_ADJUST << ADLAR); \
	/* Enable ADC and set the clock prescaler for ADC clock. */ \
	ADCSRA = (1 << ADEN) | (ADC_CLOCK_PRESCALER_FACTOR << ADPS0); })
/* Choose an ADC channel. */
#define ADCChooseChannel(ch)	(ADMUX = (ADMUX & 0xE0) | ch)
/* Start ADC once. */
#define ADCSampleOnce(ch)	{ADCChooseChannel(ch); ADCSRA |= (1 << ADSC);}
/* Get the ADC register value. */
#define ADCGetResult()	(ADC)
/* ADC the channel with blocking. */
#define ADCSampleBlocking(ch) ({ \
	ADCSampleOnce(ch); \
	/* Wait until the ADC is finished. */ \
	while(ADCSRA & (1 << ADSC)); \
	ADCGetResult(); })
/* Enable ADC interrupt. */
#define ADCEnableInterrupt()	(ADCSRA |= (1 << ADIE))
/* Define the interrupt service routine for one ADC finished. */
#define ADCFinishedEvent()	ISR(ADC_vect)

#endif
