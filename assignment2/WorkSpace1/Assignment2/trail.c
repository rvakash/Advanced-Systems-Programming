//Author : Akash R Vasishta
//UFID : 53955080
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int dimB;
int dimA;
char** buffer;
char* temp;


void *thread(void *threadid){
	char *buf;//[30] = {};
	size_t len = 0;
	ssize_t read;
	char *line = NULL;
	char *each;

   printf("Hello World!.\n");
   while ((read = getline(&line, &len, stdin)) != -1) {

	   printf("%s || \n", line);
	   each = strtok(line," ,()");
       printf("************************%s ", each);
	   while(each != NULL){
	           buf = each;
//	           strcpy(buffer[0][0], buf);
	           printf("%s", buf);
	       each = strtok(NULL," ,()");
	   }

   }
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
	pthread_t tidMpr;
	long r;
	int i;
	dimB = atoi(argv[1]);
	dimA = atoi(argv[2]);
	pthread_t tidRdr[dimA];
	buffer = malloc(dimA * sizeof(char*));
	temp = malloc(dimA * dimB * sizeof(char));
	for (i = 0; i < dimA; i++) {
	  buffer[i] = temp + (i * dimB);
	}
//Create thread Thread
	pthread_create(&tidMpr, NULL, thread, NULL);

	free(temp);
	free(buffer);

	pthread_exit(NULL);
}
