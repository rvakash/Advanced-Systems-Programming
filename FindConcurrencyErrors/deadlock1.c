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

void *funct(void *th){

	int rc, fd;
	printf("Thread %d\n", tid++);
	fd = open(DEVICE, O_RDWR);
	sleep(0.6);	
	if(fd == -1) {
		printf("File %s does not exist. Exiting!!\n", DEVICE);
		exit(-1);
	}
	rc = ioctl(fd, E2_IOCMODE2, 1);
	close(fd);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	long i, j;
	pthread_t tid[2];

	for(i = 0; i<2; i++) 
		pthread_create(&tid[i], NULL, funct, (void *) i);
	for(j = 0; j<2; j++)		
		pthread_join(tid[j],NULL);

	return 0;
}