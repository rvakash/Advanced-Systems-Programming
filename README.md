# Advanced-Systems-Programming
EEL 5733/4732 Advanced Systems Programming.

Department of Electrical and Computer Engineering.

University of Florida

This repository contains all the projects that I did as a part of my course work.

### __1. Map-Reduce__
The algorithm contains two important tasks, namely Map and Reduce. Map takes a set of data and converts it into another set of data, where individual elements are broken down into tuples (key/value pairs). Secondly, reduce task, which takes the output from a map as an input and combines those data tuples into a smaller set of tuples. Combiner program makes use of fork, exec, pipe and dup2 system calls and combines the two Map-Reduce programs.

### __2. Producer Consumer Using Mutex__
The problem describes two threads, the producer and the consumer, who share a common, fixed-size buffer used as a queue. The producer's job is to generate data, put it into the buffer, and start again. At the same time, the consumer is consuming the data (i.e., removing it from the buffer), one piece at a time. The solution should be free of race conditions and deadlocks. Thread synchronization primitives such as mutexes, condition variables can be used.

### __3. Producer Consumer Using Semaphore__
Implement the above described Producer Consumer problem using thread synchronization primitives such as semaphores.

### __4. ProducerConsumer_UsingMMAP__
Implement the above described Producer Consumer problem using any thread synchronization primitives. mmap system call with anonymous mapping (no backing file) should be used to set up the shared region.

### __5. Simple Character Device Driver__
Write a simple character device driver which supports read, write, lseek and ioctl commands.

### __6. Find Concurrency Errors in the Driver__
The task here is to find deadlocks and race conditions in the provided character device driver. Write test cases that expose different potential deadlock scenarios. For race conditions, perform a code review and come up with different pairs of critical regions which may run in parallel.

## __7. USB Keyboard Driver__
Introduced two new modes of operation for CAPSLOCK and NUMLOCK in the Linux USB keyboard driver (usbkbd.c). 
* __Mode 1:__ Default behavior of the driver
* __Mode 2:__ This mode is activated when NUMLOCK is pressed and CAPSLOCK is not on. When transitioning to Mode 2, the CAPSLOCK led will be turned on automatically. In Mode 2, the functionality of CAPSLOCK is reversed. i.e. when CAPSLOCK led is on the charaters typed will be in lower case and when CAPSLOCK led is off the characters typed will be in upper case. 
