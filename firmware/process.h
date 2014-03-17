/*------------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
------------------------------------------------------------------------------*/

#ifndef __PROCESS_H__
#define __PROCESS_H__

/* Work of one timer tick need to do. */
void SYSTimerTickWork(void);
void StartSampling(void);
void StopSampling(void);

#endif
