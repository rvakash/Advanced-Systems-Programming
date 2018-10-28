#include <linux/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEVICE "/dev/a5"

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

#define MODE1 1 
#define MODE2 2
int tid =0;

void *funct(void *args){
	int r,fd;
	printf("Thread %d\n", tid++);
	fd = open(DEVICE, O_RDWR);
	sleep(0.6);
	r = ioctl(fd, E2_IOCMODE1, 1);
	close(fd);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){

	int i, r, fd;
	pthread_t tid[2];
	fd = open(DEVICE, O_RDWR);
	r = ioctl(fd, E2_IOCMODE2, 2);
	sleep(0.5);
	for(i = 0; i<2; i++) 
		pthread_create(&tid[i],NULL,&funct,NULL);  
	for(i = 0; i<2; i++)		
		pthread_join(tid[i],NULL);	
	return 0;
}