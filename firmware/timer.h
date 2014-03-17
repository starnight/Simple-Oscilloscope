/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __AVR_TIMER_H__
#define __AVR_TIMER_H__

#if !defined(AVR_PARTNO) || (AVR_PARTNO==atmega328) || (AVR_PARTNO==atmega328p)
#include "platform/timer328.h"
#elif AVR_PARTNO==atmega8
#include "platform/timer8.h"
#elif AVR_PARTNO==atmega128
#include "platform/timer128.h"
#endif

#endif
