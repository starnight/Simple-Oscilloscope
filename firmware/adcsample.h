/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __ADC_SAMPLE_H__
#define __ADC_SAMPLE_H__

#include <stdint.h>
#include "adc.h"

/* Set the default amount of ADC channels. */
#ifndef AMOUNT_OF_ADC_CHANNEL
#define AMOUNT_OF_ADC_CHANNEL	5
#endif

/* Set the default ADC sampling buffer length. */
#ifndef ADC_SAMPLE_BUFLEN
#define ADC_SAMPLE_BUFLEN	20
#endif

/* The packaged ADC values datastructure. */
typedef struct _ADCVALUE {
	uint16_t adc[AMOUNT_OF_ADC_CHANNEL];
} ADCVALUE;

/* Initial the ADC sampling module. */
void ADCSampleInit(void);
/* Start ADC sampling. */
void ADCSampleStart(void);
/* Work of one ADC channel finished need to do. */
void ADCSampleWork(void);
/* Job of all of the ADC channels had sampled once. */
void ADCSampleFinishedJob(void *);
/* Read one package of ADC value. */
ADCVALUE * ADCSampleRead(void);
/* Job of ADC value is read finished. */
void ADCSampleReadFinishedJob(void *);
/* Define one ADC channel finished event. */
#define ADCSampleChannelEvent()	ADCFinishedEvent()
/* Have the number of samples left. */
uint8_t ADCSampleLeft(void);

#endif
