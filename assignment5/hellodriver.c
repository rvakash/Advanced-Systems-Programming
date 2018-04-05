#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char *DEVICE = "/dev/tux0";
static char msg[11] = "Hello TUX!!";
static char *read_buf;

int main(int argc, char *argv[]) {
	int fd = open(DEVICE, O_RDWR);
	if(fd == -1) {
		printf("Device %s does not exist \n", DEVICE);
		exit(-1);
	} else{
		printf("opened the device successfully\n");
	}
	int nbytes = write(fd, msg, 11);
	printf("Number of bytes written = %d\n", nbytes);
	int rbytes = read(fd, read_buf, 11);
	// read_buf[rbytes] = '\0';
	printf("Read from tux = %s\n", read_buf);
	printf("rbytes from tux = %d\n", rbytes);
	close(fd);
	return 0;
}