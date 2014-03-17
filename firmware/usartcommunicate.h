/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __USART_COMMUNICATE_H__
#define __USART_COMMUNICATE_H__

#include <stdint.h>
#include "scheduler/scheduler.h"
#include "usart.h"

/* Set the default USART time out ticks. */
#ifndef USARTCOMM_TIMEOUTTIME
#define USARTCOMM_TIMEOUTTIME	50
#endif

/* Initial the USART communication module. */
#define USARTCommInit()	USARTInit()
/* Send a memory buffer asynchronously. */
uint8_t USARTCommSend(uint8_t *, uint8_t, void *, SS_CB);
/* Read into a memory buffer asynchronously. */
uint8_t USARTCommRead(uint8_t *, uint8_t, void *, SS_CB);
/* Work of USART TX had sent a byte need to do. */
void USARTCommSendWork(void);
/* Work of USART RX had read a byte need to do. */
void USARTCommReadWork(void);
/* Send a memory buffer with blocking. */
uint8_t USARTCommSendBlocking(uint8_t *, uint8_t);
/* Read into memory buffer with blocking. */
uint8_t USARTCommReadBlocking(uint8_t *, uint8_t);
/* Define one byte had send through USART event. */
#define USARTCommSendEvent()	USARTTXCompleteEvent()
/* Define one byte had read from USART event. */
#define USARTCommReadEvent()	USARTRXCompleteEvent()
/* Set time out time and the callback function. */
void USARTCommSetTimeout(uint8_t, void *, SS_CB);
/* Set time out time and the callback function after read first byte. */
void USARTCommSetShiftTimeout(uint8_t, void *, SS_CB);
/* Work of system tick to check time out. */
void USARTCommTimeoutWork(void);

#endif
