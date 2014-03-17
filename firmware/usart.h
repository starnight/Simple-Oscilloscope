/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __AVR_USART_H__
#define __AVR_USART_H__

#if !defined(AVR_PARTNO) || (AVR_PARTNO==atmega328) || (AVR_PARTNO==atmega328p)
#include "platform/usart328.h"
#elif AVR_PARTNO==atmega8
#include "platform/usart8.h"
#elif AVR_PARTNO==atmega128
#include "platform/usart128.h"
#endif

#endif
