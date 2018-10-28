Name: Akash R Vasishta
UFID: 53955080

Deadlock scenarios:

1) Thread 1 stuck in open and Thread 2 stuck in ioctl(line 154 in assignment6.c)
Spawn 2 threads. Both are in mode1 and will call open() and increment count1 to 2. T1 will down(sem1), up(sem1) and then down(sem2) and return. T2 will down(sem1), up(sem1) and then try to down(sem2) but T1 is holding sem2, so T2 is stuck. T1 then calls ioctl to change mode to mode2. Since count1 > 1, it will go into the while loop and go to the wait queue1 waiting for count1 == 1. Both T1 and T2 are stuck.
This same deadlock situation can be achieved by running it on multiple processes.

2) Thread 1 and Thread 2 stuck in wait queue2 (line 174 in assignment6.c)
Main Thread opens() in mode1 and count1=1. Main thread calls ioctl for mode2 and count1=0. Spawn 2 threads. Both call open() and will make count2=2. Both the thread will call ioctl to change mode to 1. Since count2>1, T1 will wait in queue2 for count2==1 (releasing sem1). T2 will take sem1 and also wait in queue2 for count2==1. So a deadlock has occured.

3) Main thread call open() 2 times (line 49)
Main thread calls open() in mode 1 once and takes sem2 and returns. It again calls open() and gets stuck trying to down(sem2) which it had acquired previously. This is a deadlock.

4) Main thread calls open() in mode 1 and then changes the mode to mode2 calling ioctl(). 2 threads are spawned and then using pthread_barrier_wait() the threads are synchronized and both the threads enter ioctl() at the same time to change to mode 1. This causes a deadlock.


Race Condition scenarios:

1) Concurrent Read/write() by 2 or more threads
Critical Region - 
e2_read() in mode 1 --> ret = count - copy_to_user(buf, devc->ramdisk, count);
e2_write() in mode 1 --> ret = count - copy_from_user(devc->ramdisk, buf, count);

Semaphore that should be help sem1.
When a process opens() and then spawns 2 threads which share the file descriptor. If the threads call read and write simultaneously we are calling down(sem1) and up(sem1) before actually reading or writing from/to the ramdisk. This is a potential data race.

2) Updating count1 and count2 variables by 2 or more threads
Critical Region - 
e2_open() in mode 1 and mode 2
////////////////////////////////////////////////
    down_interruptible(&devc->sem1);
    if (devc->mode == MODE1) {
        devc->count1++;
        up(&devc->sem1);
        down_interruptible(&devc->sem2);
        return 0;
    }
    else if (devc->mode == MODE2) {
        devc->count2++;
    }
    up(&devc->sem1);
////////////////////////////////////////////////
e2_release() in mode 1 and mode 2
////////////////////////////////////////////////
    down_interruptible(&devc->sem1);
    if (devc->mode == MODE1) {
        devc->count1--;
        if (devc->count1 == 1)
            wake_up_interruptible(&(devc->queue1));
			up(&devc->sem2);
    }
    else if (devc->mode == MODE2) {
        devc->count2--;
        if (devc->count2 == 1)
            wake_up_interruptible(&(devc->queue2));
    }
    up(&devc->sem1);
/////////////////////////////////////////////////

Semaphore held during the update - sem1
count1 and count2 is a critical region of the code because multiple threads are using it as a condition variable to come out of wait queues 1 and 2. We are calling down(sem1) and then updating the count1 and count2 before calling up(sem1). This region is protected by the locks and hence no data race exists.

3) 2 or more threads perform concurrent writes
Critical Region - 
e2_write() in mode 2 --> ret = count - copy_from_user(devc->ramdisk, buf, count);
Writing into the ramdisk by 2 or more threads concurrently is a data race condition. This region is protected by holding the lock sem1. We are doing down(sem1) and then in mode2 we are writing into the ramdisk and then up(sem1). 

4) 2 or more threads perform ioctl to change mode to mode 2
Critical Region -
e2_ioctl() - case E2_IOCMODE2:
///////////////////////////////////////////////////
				if (devc->mode == MODE2) {
					up(&devc->sem1);
					break;
				}
				if (devc->count1 > 1) {
					while (devc->count1 > 1) {
						up(&devc->sem1);
					    wait_event_interruptible(devc->queue1, (devc->count1 == 1));
						down_interruptible(&devc->sem1);
					}
				}
				devc->mode = MODE2;
                devc->count1--;
                devc->count2++;
////////////////////////////////////////////////////
We are trying to read from count1 and mode. This is a critical region and it is protected by sem1. If 2 or more threads try to change mode to mode 2 without the sem1 semaphore it becomes a data race. we are calling down(sem1) and then reading or updating count1 and count2 and mode and then calling up(sem1). This has prevented a data race from occuring.

