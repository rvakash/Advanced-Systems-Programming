Assignment 6 : USB Keyboard Driver

Files included :
usbkbd.c
redirectUSBKeyboard.sh
Makefile

Instructions on how the module will be tested:

Copy the files usbkbd.c, Makefile and redirectUSBKeyboard.sh to a virtual linux machine
and follow the following steps:

1) Compile driver module : $ make

2) Load module : $ sudo insmod usbkbd.ko

3) Connect the USB Keyboard and transfer the control to Virtual Machine

4) To pass the control of the keyboard from usbhid to usbkbd driver by running the following script

5) script(root user) : $ ./redirectUSBKeyboard.sh

6) Now press the following combination of keys for testing
	a. In MODE1:
	   - CAPSLOCK not pressed   => CAPSLOCK LED turns OFF, letter = Lowercase
	   - CAPSLOCK is pressed    => CPASLOCK LED turns ON,  letter = Uppercase
	
	b. In MODE2 : To go to MODE2, press NUMLCOK with CAPSLOCK OFF (Only one way)
	   - The initial behavior in MODE2: CAPSLOCK not pressed => CAPSLOCK LED is ON, NUMLOCK LED is ON, letter = Lowercase
	   - CAPSLOCK is pressed => CAPSLOCK LED is OFF, NUMLOCK LED is ON, letter = Uppercase

	c. From MODE2 to MODE1 (two ways)
	   - When CAPSLOCK is not pressed => NUMLOCK is pressed, both CAPSLOCK and NUMLOCK LEDs will be off, letter = Lowercase
           - When CAPSLOCK is pressed     => NUMLOCK is pressed, CAPSLOCK LED turns ON, NUMLOCK LED turns OFF, letter = Uppercase

7) Unload module : $ sudo rmmod usbkbd
