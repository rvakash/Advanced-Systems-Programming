#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc,char *argv[])
{
	int fd[2],childpid, childpid2;
	char buf[80];
	FILE *fp;                				// pointer to the file
	// Checking for input arguments
	if(argc != 2) {
		if(argc < 2)
			printf("Insufficient arguments passed\n");
	        else
        		printf("Too many arguments passed\n");
        	printf("usage: pass an input text file containing words\n");
        	return 1;
    	}
	// Opening the input file containing Words
	fp = fopen(argv[1], "r");
        if(fp == NULL) {
            printf("Cannot open the file.\n");
            printf("Input a valid text file..\n");
            exit(1);
        }
	fclose(fp);

	if(pipe(fd)<0)
	printf("\n can't create pipe \n");
	if((childpid=fork())<0)
		printf("can't fork");
	else if(childpid==0)
	{
//		printf("\n child process. Calling reducerP1");
//		printf("\n fd[0]=%d ,fd[1]=%d \n",fd[0],fd[1]);
//		printf("\n Enter the  data \n");
//		scanf("%[^\n]",buf);
		dup2(fd[0],STDIN_FILENO);
		close(fd[1]);
		execlp("./reducerPFinal1.out", "./reducerPFinal1.out", NULL);
		printf("Exec error\n"); 	    						// executed by child 1 		
	}
	else if(childpid > 0)
	{
//		wait();
//		sleep(1);
//		printf("\n parent process reading from the pipe");
//		read(fd[0],buf,sizeof(buf));
//		printf("\n buf=%s \n",buf);
		if((childpid2=fork())<0)
			printf("can't fork");
		else if(childpid2==0){
//			sleep(1);
			printf("Inside child process 2. Reading from the pipe");
			dup2(fd[1], STDOUT_FILENO);
			close(fd[0]);
			execlp("./mapperTrail2.out", "./mapperTrail2.out", argv[1], NULL);
//			read(fd[0],buf,sizeof(buf));
//			printf("\n buf=%s \n",buf);
		}
		else if(childpid2>0){
			close(fd[0]);
			close(fd[1]);
			waitpid(-1,NULL,0);
		}
		else
			printf("Second fork failed\n");

		
	}
	else
		printf("First fork failed");
	close(fd[0]);
	close(fd[1]);
	wait(NULL);
	wait(NULL);
	return 0;
}
