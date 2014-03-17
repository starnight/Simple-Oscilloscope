#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''-----------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
-----------------------------------------------------------------------------'''

import os, signal;
import threading;
import time;
from modbus import Modbus, ModbusError;
from scope import Scope;

global data;
global f;
global ReadingADC;

def ReadADC(modbus, scope):
	'''Function to read ADC samples from oscilloscope.'''
	global data;
	global ReadingADC;

	if ReadingADC == False:
		newADC = None;
		ReadingADC = True;
		try:
			newADC = modbus.ReadSample();
			if newADC is not None:
				scope.mutex.acquire();
				for i in range(modbus.GetNumChannels()):
					# Append the new ADC value to data.
					data[i] += newADC[i];
					# Bind the value for scope to present.
					scope.data[i] = data[i][-scope.GetXRange()[1]:];
				scope.mutex.release();
		except:
			raise;
		finally:
			ReadingADC = False;

def ReadThread(modbus, scope, sec):
	'''Thread to read ADC samples from oscilloscope.'''
	while True:
		t = threading.Thread(target=ReadADC, args=(modbus, scope));
		time.sleep(sec);
		t.start();

def WriteThread(scope):
	'''Thread to write ADC samples to a file.'''
	global data;
	global f;

	# Wait for there is data.
	while len(data[0]) < 10:
		Index = 0;

	# Write data.
	while True:
		time.sleep(0.1);
		newIndex = len(data[0]);
		# Write the data in the range between Index and newIndex.
		for j in range(Index, newIndex):
			buf = [];
			for i in range(len(data)):
				buf.append(data[i][j]);
			strbuf = ",".join("{:5}".format(x) for x in buf);
			f.write(strbuf + "\n");

		# Delete the old data if data is longer enough.
		if len(data[0]) > (newIndex + scope.GetXRange()[1]):
			scope.mutex.acquire();
			for i in range(len(data)):
				del data[i][:newIndex];
			scope.mutex.release();
			Index = 0;
		else:
			Index = newIndex;

def KeyThread(modbus, scope):
	'''Thread to get command from console.'''
	global f;

	key = input("Press Enter to continue...\n> ");
	f.close();
	print("bye-bye");
	os.kill(os.getpid(), signal.SIGINT)

if __name__ == '__main__':
	global data;
	global f;
	global ReadingADC;

	modbus = Modbus("/dev/ttyUSB0", 115200, 5);
	scope = Scope();
	data = [[] for i in range(modbus.GetNumChannels())];
	rt = threading.Thread(target=ReadThread, args=(modbus, scope, 0.01));
	wt = threading.Thread(target=WriteThread, args=(scope,));
	kt = threading.Thread(target=KeyThread, args=(modbus, scope,));
	filename = "data-" + time.strftime("%Y%m%d%H%M%S") + ".txt";
	f = open(filename, "w");

	# Bind data to scope.
	scope.data = data;
	# Send start sample command to oscilloscope.
	modbus.StartSample();

	ReadingADC = False;

	# Start threads.
	rt.start();
	wt.start();
	kt.start();
	scope.StartScope(scope);
