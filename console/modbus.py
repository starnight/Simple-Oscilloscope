#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''-----------------------------------------------------------------------------
  Copyright (c) 2014 The Simple Oscilloscope Project. All rights reserved.
  Use of this source code is governed by a BSD-style license that can be found
  in the LICENSE file.
-----------------------------------------------------------------------------'''

from crcmod import predefined;
import serial;

def MODBUSCrc(payload):
	'''
	Have MODBUS CRC of the payload.
	Reference from http://crcmod.sourceforge.net/crcmod.predefined.html
	'''
	modbuscrc = predefined.Crc("modbus");
	modbuscrc.update(payload);
	return modbuscrc.crcValue;

def CommandWithCRC(command):
	'''
	Append the MODBUS CRC after the command.
	'''
	return command + MODBUSCrc(command).to_bytes(2, 'little');

class Status:
	'''
	The class to hold the status of Simple Oscilloscope.
	'''
	length = 0
	size = 0;
	error = 0;
	running = 0;
	leftADC = 0;

class ModbusError(Exception):
	'''
	Define the error exception of MODBUS module.
	'''
	def __init__(self, value):
		self.value = value;
	def __str__(self):
		return repr(self.value);

class Modbus(serial.Serial):
	'''
	The MODBUS communication implementation of Simple Oscilloscope Console.
	'''
	__channel = 1;
	__nadcs = 1;

	def __init__(self, port, baud, channel = 1):
		serial.Serial.__init__(self, port, baud, timeout=0.01);
		self.__channel = channel;

	def GetNumChannels(self):
		'''Get the number of ADC channels.'''
		return self.__channel;

	def StartSample(self):
		'''Start Simple Oscilloscope sampling.'''
		res = False;

		try:
			# Send start sampling command with CRC.
			command = b"0s";
			self.write(CommandWithCRC(command));
			
			# Read start sampling feedback with CRC.
			ans = self.read(4);
			crc = ans[-2] + ans[-1]*256;

			# Check returned CRC.
			if MODBUSCrc(ans[:-2]) != crc:
				raise ModbusError("CRC check error.");
			else:
				res = True;
		except:
			raise;
			self.close();

		return res;

	def StopSample(self):
		'''Stop Simple Oscilloscope sampling.'''
		res = False;

		try:
			# Send stop sampling command with CRC.
			command = b"0p";
			self.write(CommandWithCRC(command));
			
			# Read stop sampling feedback with CRC.
			ans = self.read(4);
			crc = ans[-2] + ans[-1]*256;

			# Check returned CRC.
			if MODBUSCrc(ans[:-2]) != crc:
				raise ModbusError("CRC check error.");
			else:
				res = True;
		except:
			raise;
			self.close();

		return res;

	def ReadSample(self):
		'''Read 10 ADC sample packages from Simple Oscilloscope.'''
		try:
			# Send reading ADC samples command CRC.
			command = b"0r";
			self.write(CommandWithCRC(command));
			
			# Read reading ADC samples feedback with CRC.
			ans = self.read(2);
			# Check Simple Oscilloscope response's function code.
			if ans[1] == 0x72:
				# Without exception.
				ans += self.read(2*self.__nadcs*self.__channel + 2);
				adc = [[] for i in range(self.__channel)];
				# Parse the response to ADC sample array.
				for i in range(self.__nadcs):
					for j in range(self.__channel):
						adc[j].append(ans[i*2*self.__channel+j*2+2]+ \
									  ans[i*2*self.__channel+j*2+3]*256);
			else:
				# With exception.  Not enough samples.
				adc = None;
				ans += self.read(3);
			crc = ans[-2] + ans[-1]*256;
			
			# Check returned CRC.
			if MODBUSCrc(ans[:-2]) != crc:
				adc = None;
				raise ModbusError("CRC check error.  {0} ans={1}".format(len(ans), ans));
		except:
			raise;
			self.close();

		return adc;

	def Debug(self):
		'''Read debug information Simple Oscilloscope.'''
		st = Status();
		try:
			# Send debug command with CRC.
			command = b"0d";
			self.write(CommandWithCRC(command));

			# Read debug feedback with CRC.
			ans = self.read(9);
			crc = ans[-2] + ans[-1]*256;
						
			# Check returned CRC.
			if MODBUSCrc(ans[:-2]) != crc:
				st = None;
				raise ModbusError("CRC check error.");
			else:
				st.length = ans[2];
				st.size = ans[3];
				st.error = ans[4];
				st.running = ans[5];
				st.leftADC = ans[6];
		except:
			raise;
			self.close();

		return st;
