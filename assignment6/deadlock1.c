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

int a=0;
void *my_testing(void *args);

int main(int argc, char *argv[]) {

	int i;
	
	pthread_t tid[2];
	for(i=1;i<3;i++) 
		pthread_create(&tid[i],NULL,&my_testing,NULL);

	
	for(i=1;i<3;i++)		
		pthread_join(tid[i],NULL);

	printf("Threads exiting\n");
	
	return 0;
}


void *my_testing(void *args) {

	char dev_path[20];
	char write_buf[100];
	sprintf(dev_path, "%s", DEVICE);
	int rc,mode,fd;
	a++;
	printf("Thread %d entered\n",a);
	fd = open(dev_path, O_RDWR);
	
	if(fd == -1) {
		printf("File %s either does not exist or has been locked by another ""process\n", DEVICE);
		exit(-1);
	}


	if(a==1) {
		write(fd, write_buf, sizeof(write_buf));
	}
	if(a==2) {
		write(fd, write_buf, sizeof(write_buf));
	}

	usleep(500000);

	rc = ioctl(fd, E2_IOCMODE2, 1);

	close(fd);
	pthread_exit(NULL);
}
