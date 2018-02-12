#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int slot[4];
int slot2[4];

typedef struct Object{
	int array[10];
	pthread_mutex_t mtx;
	pthread_cond_t  condE;
	pthread_cond_t  condF;
	int count;
} Object;
Object object[50];
int done;

void *Producer(void *threadid){
	int i, value, index = 0;
	size_t len = 0;
	ssize_t read;
	char * line = NULL;
	char *each;
	while(1){
		  read = getline(&line, &len, stdin);
		  if(read == -1){
			  done = 1;
			  for(i=0; i<4; i++){
//				  printf("____________HERE_________________\n");
				  pthread_cond_signal(&object[i].condF);
				  pthread_cond_signal(&object[i].condE);
			  }
			  break;
		  }
	      each = strtok(line,",");
		  while(each != NULL){
			  value = atoi(each);
			  switch (value){
			  case 1 ... 6:
			  	  index = 0;
			  	  break;
			  case 7 ... 14:
  			  	  index = 1;
  			  	  break;
			  case 15 ... 22:
  			  	  index = 2;
  			  	  break;
			  case 23 ... 30:
  			  	  index = 3;
  			  	  break;
			  }
			  pthread_mutex_lock(&object[index].mtx);
			  while(object[index].count == 10){
					pthread_cond_wait(&object[index].condF, &object[index].mtx);
			  }
//			  printf("%d", value);
			  object[index].array[slot[index]] = value;
			  object[index].count = object[index].count + 1;
  			  slot[index] = (slot[index] + 1) % 10;
  	          pthread_cond_signal(&object[index].condE);
  	          pthread_mutex_unlock(&object[index].mtx);

		      each = strtok(NULL,",");
		  }

	}
	pthread_exit(NULL);
}
void *Consumer(void *threadid){
	   long tid;
	   tid = (long)threadid;
	   int i;
    while(1){
        pthread_mutex_lock(&object[tid].mtx);
        while(object[tid].count == 0 && done == 0){
     	   pthread_cond_wait(&object[tid].condE, &object[tid].mtx);
        }

        if(done && object[tid].count == 0){
 		  for(i=0; i<4; i++){
 			  pthread_cond_signal(&object[i].condE);
 			  pthread_cond_signal(&object[i].condF);
 		  }
     	  pthread_mutex_unlock(&object[tid].mtx);
     	  break;
        }
        printf("thread = %ld, value = %d\n", tid, object[tid].array[slot2[tid]]);
        object[tid].count = object[tid].count - 1;
        pthread_cond_signal(&object[tid].condF);
        pthread_mutex_unlock(&object[tid].mtx);
        slot2[tid] = (slot2[tid] + 1) % 10;

    }
	pthread_exit(NULL);
}


int main (int argc, char *argv[])
{
	pthread_t tidPr;
	pthread_t tidCr[4];
	long r;
//Create Producer Thread
	pthread_create(&tidPr, NULL, Producer, NULL);
////Create Consumer Threads
	for(r = 0; r < 4; r++){
		pthread_create(&tidCr[r], NULL, Consumer, (void *)r);
	}
//Wait for Producer Thread to finish
	pthread_join(tidPr, NULL);
////Wait for Consumer Threads to finish
	for(r = 0; r < 4; r++){
		pthread_join(tidCr[r], NULL);
	}
	pthread_exit(NULL);
}
