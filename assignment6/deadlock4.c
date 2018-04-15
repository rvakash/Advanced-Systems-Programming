#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define CDRV_IOC_MAGIC 'Z'
#define DEVICE "/dev/a5"

#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

static pthread_barrier_t barrier;

void *funct(void *arg){
	int fd1,tmp;
	fd1 = open("DEVICE", O_RDWR);

	tmp = ioctl(fd1,E2_IOCMODE2,2);

	close(fd1);
	pthread_exit(NULL);
}

void *func2(void *arg){
	int fd2,tmp1;
	fd2 = open(DEVICE, O_RDWR);

	pthread_barrier_wait(&barrier);

	tmp1 = ioctl(fd2,E2_IOCMODE1,1);

	close(fd2);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	int barrier, i;
	pthread_t tid1;
	pthread_t tid2[2];

	barrier = pthread_barrier_init(&barrier, NULL, 2);

	pthread_create(&(tid1), NULL, funct, NULL);
	for(i = 0; i<2; i++){
		pthread_create(&tid2[i],NULL,func2,NULL);
	}
	for(i = 0; i<2; i++){
		pthread_join(tid2[i],NULL);
	}
	return 0;
}