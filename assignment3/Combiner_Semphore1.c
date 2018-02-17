//Author : Akash R Vasishta
//UFID : 53955080
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>

int numOfSlots;
int numOfReducers, str_idx, str_idx1;
//char** buffer;
char* temp;
char* temp1;
char** userToRdrMap = {NULL};
int *inSlotIndex;
int outSlotIndex[100];
int done = 0;
pthread_t *producers;
pthread_t *consumers;
int *buf,buf_pos=-1,prod_count,con_count,buf_len;

struct TopicTotal{
	char userID[20];
	char topic[20];
	int score; };
int size = 0;


typedef struct { char action; int score; } Rules;
Rules rules[5] = {{'P',50}, {'L',20}, {'D',-10}, {'C',30}, {'S',40}};

typedef struct Buffer{
	char **Tuples;
	int count;
	sem_t buf_mutex,empty_count,fill_count;
} Buffer;
Buffer buffer[100];
sem_t m, avail, items;

int findScoreFromAction(Rules* rules, char action){
  int i;
  for(i=0; i<5; i++){
    if(rules[i].action == action)
      return rules[i].score;
  }
  fprintf(stderr, "Action not present! Exiting...\n");
  exit(EXIT_FAILURE);

}

void addToUser (char *userId) {
    char *copy = NULL;
    if (str_idx >= numOfReducers) {
        numOfReducers = numOfReducers * 2;
        userToRdrMap = realloc(userToRdrMap, numOfReducers * sizeof(char *));
    }
    copy = malloc(strlen(userId) * sizeof(char));
    copy = strcpy(copy, userId);
    userToRdrMap[str_idx++] = copy;
}

int findRdrNum(char *userId){
	int i;
    for (i = 0; i < str_idx; i++) {
        if (strcmp(userId, userToRdrMap[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void addToBuffer (int rdrNum, char *tuple) {
    char *copy = NULL;
    copy = malloc(strlen(tuple) * sizeof(char));
    copy = strcpy(copy, tuple);
    buffer[rdrNum].Tuples[inSlotIndex[rdrNum]] = copy;
    buffer[rdrNum].count = buffer[rdrNum].count + 1;
}

void printBuffer (struct Buffer buffer[numOfReducers]) {

	int i, j;
	for(i=0; i<numOfReducers; i++){
		for(j=0;j<numOfSlots; j++){
			printf("Mapper Printing buffer = %s", buffer[i].Tuples[j]);
		}
		printf("\n");
	}
	printf("done printing buffer\n");
}

int findScore(struct TopicTotal topictotal[50], char* userId, char* topic1, int size){
  int i;
  int ti = 0;
  int equal = 0;
  int equaluser = 0;
  char *tempuser = userId;
  char *temp = topic1;
  for(i=0; i<size; i++){
    while(*userId != '\0'){
      if(topictotal[i].userID[ti++] == *userId++)
        equaluser = 1;
      else{
        equaluser = 0;
        break;
      }
    }
    ti = 0;
    userId = tempuser;
    if(equaluser == 1){
      while(*topic1 != '\0'){
        if(topictotal[i].topic[ti++] == *topic1++)
          equal = 1;
        else{
          equal = 0;
          break;
        }
      }
      if (equal == 1){
        return topictotal[i].score;
      }
      ti = 0;
      topic1 = temp;
    }
  }
  return -1;
}

int updateScore(struct TopicTotal topictotal[50], char* userId, char* topic1, char *newScore, int size){
  int i;
  int equal = 0;
  int ti = 0;
  int equaluser = 0;
  char *tempuser = userId;
  char* temp = topic1;
  for(i=0; i<size; i++){
    while(*userId != '\0'){
      if(topictotal[i].userID[ti++] == *userId++)
        equaluser = 1;
      else{
        equaluser = 0;
        break;
      }
    }
    ti = 0;
    userId = tempuser;
    if(equaluser == 1){
      while(*topic1 != '\0'){
        if(topictotal[i].topic[ti++] == *topic1++)
          equal = 1;
        else{
          equal = 0;
          break;
        }
      }
      if (equal == 1){
        topictotal[i].score = topictotal[i].score + atoi(newScore);
      }
      topic1 = temp;
      ti = 0;
    }
  }
}

void *Mapper(void *threadid){
	  FILE *fp;
	  char inputTuple[30] = {};
	  char buf[30] = {};
	  char* oB;
	  oB = "(";
	  char* cB;
	  char* co = ",";
	  cB = ")";
	  int score;
	  char *each;
	  char *userId;
	  size_t len = 0;
	  ssize_t read;
	  char * line = NULL;
	  char *eacharray;
	  int u = 0;
	  char *outputTuple;
	  int pad = 0;
	  int i, j, rdrNum = 0;
	  long r;
	  pthread_t tidRdr[numOfReducers];
      userToRdrMap = malloc(numOfReducers * sizeof(char *));
      inSlotIndex = (int *) calloc(numOfReducers, sizeof(int));


    while (1) {
		    read = getline(&line, &len, stdin);
		    if(read == -1){
			      done = 1;
			      break;
		    }
	      each = strtok(line," ,()");
	      while(each != NULL){
  	        if(u%3 == 0){
    	          outputTuple = (char *) malloc(1 + strlen(each)+ strlen(oB) );
	              strcpy(outputTuple, oB);
	              strcat(outputTuple, each);
	              userId = each;
	          } else if(u%3 == 1){
  	            char q = *each;
	              score = findScoreFromAction(rules, q);
	          } else if(u%3 == 2){
	              char * str3 = (char *) malloc(1 + strlen(outputTuple)+ strlen(co) );
	              strcpy(str3, outputTuple);
	              strcat(str3, co);
	              char *str4 = (char *) malloc(1 + strlen(str3)+ strlen(each) );
  	            strcpy(str4, str3);
	              strcat(str4, each);
	              for(pad = strlen(each); pad<15; pad++)
	                strcat(str4," ");
	              sprintf(buf, "%s,%d)\n", str4, score);
			          rdrNum = findRdrNum(userId);
		  	        if(rdrNum > -1){
			          } else{
			  	          addToUser(userId);
				            rdrNum = findRdrNum(userId);
    			      }
//			    pthread_mutex_lock(&buffer[rdrNum].mtx);
  		      		// sem_wait(&buffer[rdrNum].buf_mutex);
	  		       //  sem_wait(&avail);
		  	       //  sem_wait(&m);
//			    while(buffer[rdrNum].count == numOfSlots){
//				   pthread_cond_wait(&buffer[rdrNum].condF, &buffer[rdrNum].mtx);
		     			  // printf("here1\n");
				  	    sem_post(&buffer[rdrNum].buf_mutex);
			      	  sem_wait(&buffer[rdrNum].empty_count);
					      // sem_wait(&buffer[rdrNum].buf_mutex);
//			    }
      			    addToBuffer(rdrNum, buf);
			          // sem_post(&m);
			          // sem_post(&items);
			          inSlotIndex[rdrNum] = (inSlotIndex[rdrNum] + 1) % (numOfSlots);
// 	            printf("rdrNum = %d, count = %d\n", rdrNum, buffer[rdrNum].count);
	              free(outputTuple);
	              free(str3);
	              free(str4);
	    		      sem_post(&buffer[rdrNum].buf_mutex);
	    		      sem_post(&buffer[rdrNum].fill_count);
//	            pthread_mutex_unlock(&buffer[rdrNum].mtx);
//	            pthread_cond_signal(&buffer[rdrNum].condF);
	          }
	          u++;
  	        each = strtok(NULL," ,()");
	      }
	      u = 0;
	  }
	  for(i=0; i<numOfReducers; i++){
//		  pthread_cond_signal(&buffer[i].condF);
  		  sem_post(&buffer[i].fill_count);
	  }
	  // printf("Finished mappe");
//	  printBuffer(buffer);

    pthread_exit(NULL);
}

void *Reducer(void *threadid){
    long tid;
    tid = (long)threadid;
    struct TopicTotal topictotal[50];
    FILE *fp;
    char *each, *userId, *newScore, *topic1;
    char inputTuple[30], outputTuple[50], firstuserId[4];
    int firstTime, equal, ti,j, i, score, index;
    ti = j = i = index = 0;
    equal = firstTime = 1;
    while(1){

//         pthread_mutex_lock(&buffer[tid].mtx);
 		   //  sem_wait(&buffer[tid].buf_mutex);
		    // sem_wait(&items);
		    // sem_wait(&m);
//         while(buffer[tid].count == 0 && done == 0){
//    	     pthread_cond_wait(&buffer[tid].condF, &buffer[tid].mtx);
  			// sem_post(&buffer[i].buf_mutex);
//        printfBuffer(buffer);
        // for()
        // printf("Thread %ld, checking for wait\n", tid);
        sem_wait(&buffer[tid].fill_count);
        // printf("Thread %ld, came out of wait\n", tid);
   			sem_wait(&buffer[tid].buf_mutex);
   			// printf("here\n");
//         }
        if(done && buffer[tid].count == 0){
    	      break;
        }
        // while(buffer[tid].count > 0){
            strcpy(inputTuple, buffer[tid].Tuples[outSlotIndex[tid]]);
			      sem_post(&m);
			      sem_post(&avail);
            char *new = inputTuple;
            buffer[tid].count = buffer[tid].count - 1;
            outSlotIndex[tid] = (outSlotIndex[tid] + 1) % numOfSlots;
            each = strtok_r(new," ,()", &new);
            userId = each;
            each = strtok_r(new," ,()", &new);
            topic1 = each;
            each = strtok_r(new," ,()", &new);
            newScore = each;
            score = findScore(topictotal, userId, topic1, size);
            if(score == -1){
                strcpy(topictotal[index].userID, userId);
                strcpy(topictotal[index].topic, topic1);
                ti = 0;
                topictotal[index].score = atoi(newScore);
                size = size + 1;
                index = index + 1;
            } else {
                updateScore(topictotal, userId, topic1, newScore, size);
            }
        // }
//         pthread_mutex_unlock(&buffer[tid].mtx);
//         pthread_cond_signal(&buffer[tid].condF);
   		  sem_post(&buffer[tid].buf_mutex);
 		    sem_post(&buffer[tid].empty_count);
    }
    for(j=0; j< index; j++){
        printf("(%s,%s,%d)\n", topictotal[j].userID, topictotal[j].topic, topictotal[j].score);
    }

  pthread_exit(NULL);
}


int main (int argc, char *argv[])
{
// Checking for input arguments
  	if(argc != 3) {
	    	if(argc < 3)
			      printf("Insufficient arguments passed\n");
	      else
	        	printf("Too many arguments passed\n");
            printf("usage: pass an input text file containing words\n");
            return 1;
    }
	  srand(time(NULL));

  	pthread_t tidMpr;
	  long r;
	  int i;
	  numOfSlots = atoi(argv[1]);
	  numOfReducers = atoi(argv[2]);
	  pthread_t tidRdr[numOfReducers];

    for(i=0; i<numOfReducers; i++){
  	    buffer[i].Tuples = malloc(numOfSlots * sizeof(char *));
  	    buffer[i].count = 0;
        sem_init(&buffer[i].buf_mutex,0,1);
  	    sem_init(&buffer[i].fill_count,0,0);
  	    sem_init(&buffer[i].empty_count,0,numOfSlots);
    }
	  sem_init(&m,0,1);
  	sem_init(&avail,0,numOfSlots);
  	sem_init(&items,0,0);

//Create Mapper Thread
	  pthread_create(&tidMpr, NULL, Mapper, NULL);

//Create Reducer Threads
	  for(r = 0; r < numOfReducers; r++){
		    usleep(5000);
		    pthread_create(&tidRdr[r], NULL, Reducer, (void *)r);
	  }

	  free(temp);
	  pthread_exit(NULL);
}
