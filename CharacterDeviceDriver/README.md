# __Simple Character Device Driver__
This file gives a brief overview of the problem and instructions to run the program
Read ProblemStatement.pdf for the detailed problem description.
* Author : Akash R Vasishta
* UFID   : 53955080

### __Problem Statement__
Write a simple character device driver which supports read, write, lseek and ioctl commands

### __Instructions to run the module__

Copy the files char_driver.c, Makefile and userapp.c to a virtual Linux machine
and follow the below steps:

1. Compile driver module : $ make

2. Load module : $ sudo insmod char_driver.ko NUM_DEVICES=<num_devices>

3. Test driver :
	* Compile userapp : $ make app
	* Run userapp : $ sudo ./userapp <device_number>			
		where device_number identifies the id number of the device to be tested.   
Note : userapp has to be executed with sudo privilege as the device files
		   in /dev/ are created in the driver with root privileges.
		   
4. Unload module : $ sudo rmmod char_driver

The binaries provided are the result of executing the above instructions. They can be used to run the module.
