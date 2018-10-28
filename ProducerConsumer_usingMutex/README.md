# __Producer Consumer using Mutex__
This file gives a brief overview of the problem, the files included and instructions to run the program
Read ProblemStatement.pdf for the detailed problem description.
* Author : Akash R Vasishta
* UFID   : 53955080

--------------------
### __I. Problem Statement__
Implement the Producer Consumer problem using multiple threads. The problem describes two threads, the producer and the consumer, who share a common, fixed-size buffer used as a queue. The producer's job is to generate data, put it into the buffer, and start again. At the same time, the consumer is consuming the data (i.e., removing it from the buffer), one piece at a time. The problem is to make sure that the producer won't try to add data into the buffer if it's full and that the consumer won't try to remove data from an empty buffer. The solution should be free of race conditions and deadlocks. Thread synchronization primitives such as mutexes, condition variables and/or semaphores can be used.

<img src="http://androidsrc.net/wp-content/uploads/2015/02/PCP_AndroidSRC.net_.png" width="350" height="250" /><img src="https://pages.mtu.edu/~shene/NSF-3/e-Book/SEMA/DIAGRAM-producer.jpg" width="350" height="250" />

--------------------
### __II. Files Included__
* __Combiner.c__	 - Implements Producer-Consumer design to the previous mapper reducer problem
* __Makefile__ 	 - Consists of all necessary commands to run the "Combiner" program in one step
* __input.txt__	 - This file has a list of inputs to the "Combiner" program

--------------------
### __III. How to run the Combiner__
1. $ make
2. $ ./Combiner \<Number of Slots\> \<Number of unique UserIDs\> \<Input file\>

--------------------
## __IV. Description__
Read ProblemStatement.pdf
