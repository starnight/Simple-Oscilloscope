/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __AVR_ATMEGA8_USART_H__
#define __AVR_ATMEGA8_USART_H__

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/* Set the default clock rate. */
#ifndef F_CPU
#define F_CPU 11059200
#endif

/* Set the default USART baud rate. */
#ifndef USART_BAUD
#define USART_BAUD 38400
#endif

/* Set the default USART double rate. */
#ifndef USART_DOUBLE_RATE
#define USART_DOUBLE_RATE	0
#endif

/* Calculate USART UBRR. */
#if USART_DOUBLE_RATE
#define GetUBRR()	(F_CPU/8/USART_BAUD - 1)
#else
#define GetUBRR()	(F_CPU/16/USART_BAUD - 1)
#endif

/* Define the USART operation mode. */
#define USART_ASYNCHRONOUS_OPERATION	0
#define USART_SYNCHRONOUS_OPERATION		1
/* Set the default USART operation mode. */
#ifndef USART_OPERATION_MODE
#define USART_OPERATION_MODE	USART_ASYNCHRONOUS_OPERATION
#endif

/* Define the USART parity mode. */
#define USART_DISABLED_PARITY	0
#define USART_EVEN_PARITY		2
#define USART_ODD_PARITY		3
/* Set the default USART parity mode. */
#ifndef USART_PARITY_MODE
#define USART_PARITY_MODE	USART_DISABLED_PARITY
#endif

/* Define the USART stop bits. */
#define USART_1_STOP_BIT	0
#define USART_2_STOP_BITS	1
/* Set the default USART stop bits. */
#ifndef USART_STOP_BIT
#define USART_STOP_BIT	USART_1_STOP_BIT
#endif

/* Define the USART character size. */
#define USART_5_DATA_BITS	0
#define USART_6_DATA_BITS	1
#define USART_7_DATA_BITS	2
#define USART_8_DATA_BITS	3
/* Set the default USART data bits. */
#ifndef USART_DATA_BITS
#define USART_DATA_BITS	USART_8_DATA_BITS
#endif

/* Enable TX Complete Interrupt. */
#define USARTEnableTXCInterrupt()	(UCSRB |= (1 << TXCIE))
/* Enable RX Complete Interrupt. */
#define USARTEnableRXCInterrupt()	(UCSRB |= (1 << RXCIE))
/* Disable TX Complete Interrupt. */
#define USARTDisableTXCInterrupt()	(UCSRB &= ~(1 << TXCIE))
/* Disable RX Complete Interrupt. */
#define USARTDisableRXCInterrupt()	(UCSRB &= ~(1 << RXCIE))
/* Initial USART. */
#define USARTInit() ({ \
	/* Set baud rate */ \
	UBRRH = (uint8_t)(GetUBRR() >> 8); \
	UBRRL = (uint8_t)GetUBRR(); \
	/* Double USART transmition rate. */ \
	UCSRA = (UCSRA & (~(1 << U2X))) | (USART_DOUBLE_RATE << U2X); \
	/* Enable receiver and transmitter */ \
	UCSRB = (1 << RXEN) | (1 << TXEN); \
	/* Set frame format: asynchronous, 8 data, no parity, 1 stop bit */ \
	UCSRC = (1 << URSEL) | (USART_OPERATION_MODE << UMSEL) | \
			(USART_PARITY_MODE << UPM0) | \
			(USART_STOP_BIT << USBS) | \
			(USART_DATA_BITS << UCSZ0); })
/* Send 1 byte through USART. */
#define USARTSend1Byte(data)	(UDR = data)
/* Read 1 byte from USART. */
#define USARTRead1Byte()	(UDR)
/* Send 1 byte through USART with blocking. */
#define USARTSend1ByteBlocking(data)	({ \
	/* Wait for empty transmit buffer */ \
	while (!(UCSRA & (1<<UDRE))); \
	/* Put data into buffer, sends the data */ \
	USARTSend1Byte(data); })
/* Read 1 byte from USART with blocking. */
#define USARTRead1ByteBlocking()	({ \
	/* Wait for data to be received */ \
	while(!(UCSRA & (1<<RXC))); \
	/* Get and return received data from buffer */ \
	USARTRead1Byte(); })
/* Define the interrupt service routine for one byte had send through USART. */
#define USARTTXCompleteEvent()	ISR(USART_TXC_vect)
/* Define the interrupt service routine for one byte had read from USART. */
#define USARTRXCompleteEvent()	ISR(USART_RXC_vect)

#endif
