#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''-----------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
-----------------------------------------------------------------------------'''

import threading;
import matplotlib.pyplot as plt
import matplotlib.animation as animation

def UpdateLine(num, lines, sc):
	'''Draws the lines on the Scope.'''
	sc.mutex.acquire();
	for i in range(len(lines)):
		lines[i].set_xdata(range(len(sc.data[i])));
		lines[i].set_ydata(sc.data[i]);
	plt.draw();
	sc.mutex.release();
	return lines;

class Scope:
	'''
	The implementation of oscilloscope window.
	'''
	data = None;
	interval = None;
	mutex = None;
	__fig = None;
	__lines = None;
	__ani = None;
	__xrange = None;
	__yrange = None;

	def __init__(self):
		self.interval = 100;
		self.__xrange = [0, 400];
		self.__yrange = [0, 1024];
		self.mutex = threading.Lock();

	def GetXRange(self):
		'''Get X-axis range.'''
		return self.__xrange;

	def StartScope(self, scope):
		'''Start the scope and define the enviroment.'''
		self.__fig = plt.figure();
		self.__lines = [plt.plot([],[])[0] for i in range(len(self.data))];
		plt.xlim(self.__xrange);
		plt.ylim(self.__yrange);
		plt.xlabel('x');
		plt.title('test');
		__ani = animation.FuncAnimation(self.__fig,
										UpdateLine,
										fargs=(self.__lines, scope,),
										interval=self.interval,
										blit=True);
		plt.show();
