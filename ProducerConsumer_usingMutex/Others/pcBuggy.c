#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMTHRDS 44
pthread_t callThd[NUMTHRDS];
pthread_mutex_t lock;
pthread_cond_t full;
pthread_cond_t empty;

#define MAXLINESIZE 1000
#define BUFSIZE 15
int buffer[BUFSIZE];
int in =0;
int out = 0;
int numItems = 0;
int done = 0;
int numProduced = 0;
int numConsumed = 0;

// This implementation is buggy because done is used to flag no more data when it only means one of the producers has seen end of input
// So if there are any producers waiting to insert their data, consumers may mistake and empty && done condition as a sufficient condition
void *producer(void *t) {
  long tid = (long) t;
  char *line = malloc(500);
  ssize_t len;
  sleep(3);
  while (1) {
    pthread_mutex_lock(&lock);
    if (done) {
       printf("producer %ld is exiting\n",tid);
       pthread_mutex_unlock(&lock);      
       break;
    }
    int read = getline(&line, &len, stdin);
    printf("Producer %ld read %d\n",tid,read); 
    printf("%s\n",line);
    if  (read == -1) {
        done = 1;
        pthread_cond_broadcast(&empty); 
        pthread_cond_broadcast(&full); 
        printf("Producer %ld signalled done!\n",tid);
        pthread_mutex_unlock(&lock);
        break;
    } 
    while (numItems == BUFSIZE) { 
      printf("producer %ld waiting on full\n",tid);
      pthread_cond_wait(&full, &lock);
    }      
    printf("Producer %ld inserting %d\n",tid,read); 
    buffer[in] = read;
    in = (in + 1) % BUFSIZE;
    numItems++; 
    numProduced++;
    printf("numItems = %d numProduced = %d\n", numItems, numProduced);
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&lock);
  }
  //pthread_exit((void *)0);
  return t;
}

void *consumer(void *t) {
  long tid = (long)t;
  while (1) {
    pthread_mutex_lock(&lock);
    while (numItems == 0 && !done) {
       printf("consumer %ld waiting on empty\n",tid);
       pthread_cond_wait(&empty, &lock);
    }
    if (numItems == 0 && done) {
      printf("consumer %ld exiting\n", tid);
       pthread_mutex_unlock(&lock);
       break;  
    }
    printf("Consumer %ld %d\n",tid, buffer[out]);      
    out = (out + 1) % BUFSIZE;
    numItems--; 
    numConsumed++;
    printf("numItems = %d numConsumed = %d\n", numItems, numConsumed);
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&lock);  
  }
  //pthread_exit((void *)0);
  return t;
}

int main (int argc, char *argv[])
{
long i;
void *status;
pthread_attr_t attr;

setbuf(stdout, NULL);

/* Assign storage and initialize values */

 

pthread_mutex_init(&lock, NULL);
pthread_cond_init(&full, NULL);
pthread_cond_init(&empty, NULL);
         
/* Create threads  */
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

for(i=0;i<NUMTHRDS;i++)
  {
    if (i < NUMTHRDS/2)
       pthread_create(&callThd[i], &attr, producer, (void *)i); 
    else pthread_create(&callThd[i], &attr, consumer, (void *)i); 
   }


/* Wait on the other threads */

for(i=0;i<NUMTHRDS;i++) {
  pthread_join(callThd[i], &status);
  printf("main joined thread %ld\n",i);
  }
/* After joining, print out the results and cleanup */

printf ("Num produced =  %d \n", numProduced);
printf ("Num consumed =  %d \n", numConsumed);
pthread_mutex_destroy(&lock);
pthread_cond_destroy(&full);
pthread_cond_destroy(&empty);
pthread_attr_destroy(&attr);

pthread_exit(NULL);
}   
