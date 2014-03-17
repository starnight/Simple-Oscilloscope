/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __MODBUS_H__
#define __MODBUS_H__

/* MODBUS RTU Header. */
typedef struct _MODBUS_rtu_header {
	uint8_t address;
	uint8_t fcode;
} MODBUS_RTUHEADER;

/* MODBUS RTU structure. */
typedef struct {
	union {
		uint8_t *top;
		MODBUS_RTUHEADER *header;
	};
	uint8_t *payload;
	uint16_t *crc;
} MODBUS_RTU;

/* Set the default MODBUS time out ticks. */
#ifndef MODBUS_TIMEOUTTIME
#define MODBUS_TIMEOUTTIME USARTCOMM_TIMEOUTTIME
#endif

#define SEND_NUMBER_OF_SAMPLES	1

/* Check station. */
void CheckStationJob(void *);
/* Wait the command from USART. */
void WaitForCommandJob(void *);

#endif
