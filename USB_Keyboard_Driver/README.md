# __USB Keyboard Driver__
This file gives a brief overview of the problem, the files included and instructions to run the program
Read ProblemStatement.pdf for the detailed problem description.
* Author : Akash R Vasishta
* UFID   : 53955080

--------------------
### __I. Problem Statement__
Introduce two new modes of operation for CAPSLOCK and NUMLOCK in the Linux USB keyboard driver (usbkbd.c). 
* __Mode 1:__ Default behavior of the driver
* __Mode 2:__ This mode is activated when NUMLOCK is pressed and CAPSLOCK is not on. When transitioning to Mode 2, the CAPSLOCK led will be turned on automatically. In Mode 2, the functionality of CAPSLOCK is reversed. i.e. when CAPSLOCK led is on the charaters typed will be in lower case and when CAPSLOCK led is off the characters typed will be in upper case. 

__Note:__
In Mode 2, the driver should leave the CAPSLOCK led status in a way that will be compatible with Mode 1.

--------------------
### __II. Files Included__
* __usbkbd.c__	 - Implements the modified usb keyboard driver with two modes of operation.
* __usbKeyboard.sh__  - Contains script to unbind the default keyboard driver (usbhid) and bind the new keyboard driver (usbkbd).
* __Makefile__	 - Contains commands to compile the driver.

--------------------
### __III. Instructions to run the module__

Copy the files usbkbd.c, Makefile and usbKeyboard.sh to a virtual linux machine
and follow the below steps:

1. Compile driver module : $ make

2. Load module : $ sudo insmod usbkbd.ko

3. Connect the USB Keyboard and transfer the control to Virtual Machine

4. To pass the control of the keyboard from usbhid to usbkbd driver by running the following script

5. script(root user) : $ ./redirectUSBKeyboard.sh

6. Now press the following combination of keys for testing
	* In MODE1:
	   - CAPSLOCK not pressed   => CAPSLOCK LED turns OFF, letter = Lowercase
	   - CAPSLOCK is pressed    => CPASLOCK LED turns ON,  letter = Uppercase
	
	* In MODE2 : To go to MODE2, press NUMLCOK with CAPSLOCK OFF (Only one way)
	   - The initial behavior in MODE2: CAPSLOCK not pressed => CAPSLOCK LED is ON, NUMLOCK LED is ON, letter = Lowercase
	   - CAPSLOCK is pressed => CAPSLOCK LED is OFF, NUMLOCK LED is ON, letter = Uppercase

	* From MODE2 to MODE1 (two ways)
	   - When CAPSLOCK is not pressed => NUMLOCK is pressed, both CAPSLOCK and NUMLOCK LEDs will be off, letter = Lowercase
	   - When CAPSLOCK is pressed     => NUMLOCK is pressed, CAPSLOCK LED turns ON, NUMLOCK LED turns OFF, letter = Uppercase

7. Unload module : $ sudo rmmod usbkbd

--------------------
### __IV. Description__
Read ProblemStatement.pdf
