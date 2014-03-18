Simple Oscilloscope
===================

It is a simple digital oscilloscope.

![Simple Oscilloscope Screenshot](https://raw.github.com/starnight/Simple-Oscilloscope/master/img/screenshot.png)

* 100Hz sampling rate.
* 0 ~ +5V (According to power supply) ADC input voltage.
* 1 ~ 6/8 ADC channels which is defined in **Makefile**.
  * 6-channel 10-bit ADC in ATMega328P PDIP Package.
  * 8-channel 10-bit ADC in ATMega328P TQFP and QFN/MLF package.
  * ATMega328P [datasheet](http://www.atmel.com/Images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet.pdf).

Have the project
----------------

You can have it from the GitHub: [https://github.com/starnight/Simple-Oscilloscope](https://github.com/starnight/Simple-Oscilloscope "Simple Oscilloscope")

Or, clone the project from GitHub.

```
git clone https://github.com/starnight/Simple-Oscilloscope.git
```

Directory
---------

* firmware: The firmware of Simple Oscilloscope.
* console: The program draws the signals' wave on the computer screen.
* circuit: Ones can build the circuit according to the schematic.

Prepare the Software Environment
--------------------------------

* git
* avr-gcc
* avrdude
* pathon3
* pyserial (python library)
* crcmod (python library)
* matplotlib (python library)

Build Steps
-----------

1. Have the project by downloading from [web site](https://github.com/starnight/Simple-Oscilloscope) or git cloning.
2. Build the circuit according to the schematic in the circuit directory.
3. Buid the firmware and flash it into MCU ATMega328.
  1. Make sure there are [avr-gcc](http://www.nongnu.org/avr-libc/user-manual/install_tools.html) and [avrdude](http://www.nongnu.org/avrdude/) in the build environment.
  2. Change to the firmware directory.
  3. Ones must edit the **Makefile** according to the development environment.
      For example, set **AVRDUDE_PROGRAMMER** in **Makefile** according to the ISP tool.
  4. Have the dependent library by ``` make dep ```.
  5. Compile the firmware by ``` make ```.
  6. Connect the programmer and program the MCU by ``` make program ```.
  7. Disconnect the programmer.
  8. Connect the MCU and computer with the serial port and then power on.
4. Excute the main.py with paython3 in console directory.
  1. Make sure the serial port is connected between MCU and computer.
  2. Check the serial port device name / path list in the system.
  3. Change to the console directory.
  4. Edit the serial port parameter going to be opened in main.py.
  5. Excute ``` python3 main.py ``` or ``` ./main.py ```.

License
-------

Simple Oscilloscope's code and circuit uses the BSD license, see our '[LICENSE.md](https://github.com/starnight/Simple-Oscilloscope/blob/master/LICENSE.md "LICENSE.md")' file.
