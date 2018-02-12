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

int numOfSlots;
int numOfReducers, str_idx, str_idx1;
//char** buffer;
char* temp;
char* temp1;
char** userToRdrMap = {NULL};
int *inSlotIndex;
int *outSlotIndex;
int done = 0;

struct TopicTotal{
	char userID[20];
	char topic[20];
	int score; };
int size = 0;


typedef struct { char action; int score; } Rules;
Rules rules[5] = {{'P',50}, {'L',20}, {'D',-10}, {'C',30}, {'S',40}};

typedef struct Buffer{
	char **Tuples;
	pthread_mutex_t mtx;
	pthread_cond_t  condE;
	pthread_cond_t  condF;
	int count;
} Buffer;
Buffer buffer[50];

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
      userToRdrMap = malloc(numOfReducers * sizeof(char *));
      inSlotIndex = (int *) calloc(numOfReducers, sizeof(int));

      for(i=0; i<numOfReducers; i++){
    	  buffer[i].Tuples = malloc(numOfSlots * sizeof(char *));
    	  buffer[i].mtx = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    	  buffer[i].condE = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    	  buffer[i].condF = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    	  buffer[i].count = 0;
      }

      while (1) {
		read = getline(&line, &len, stdin);
		if(read == -1){
			  done = 1;
			  sleep(5);
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
			    pthread_mutex_lock(&buffer[rdrNum].mtx);
			    while(buffer[rdrNum].count == numOfSlots){
				   pthread_cond_wait(&buffer[rdrNum].condF, &buffer[rdrNum].mtx);
			    }
			    addToBuffer(rdrNum, buf);
			    inSlotIndex[rdrNum] = (inSlotIndex[rdrNum] + 1) % numOfSlots;
	            free(outputTuple);
	            free(str3);
	            free(str4);
	            pthread_mutex_unlock(&buffer[rdrNum].mtx);
	            pthread_cond_signal(&buffer[rdrNum].condF);
	        }
  	        each = strtok(NULL," ,()");
	        u++;
	    }
	    u = 0;
	  }
	  for(i=0; i<numOfReducers; i++){
		  pthread_cond_signal(&buffer[i].condF);
	  }

   pthread_exit(NULL);
}

void *Reducer(void *threadid){
   long tid;
   tid = (long)threadid;
//   printf("I am Reducer thread #%ld!\n", tid);
     struct TopicTotal topictotal[50];
     FILE *fp;
     char *each;
     char *userId, *newScore;
     char inputTuple[30], outputTuple[50];
     char firstuserId[4];
     int firstTime = 1;
     int ti = 0;
     int j = 0;
     int i;
     int score, index;
     char *topic1;
     index = 0;
     int equal = 1;
     outSlotIndex = (int *) calloc(numOfReducers, sizeof(int));
     while(1){

       pthread_mutex_lock(&buffer[tid].mtx);
       while(buffer[tid].count == 0 && done == 0){
    	   printf("In reducer. done = %d\n", done);
    	   pthread_cond_wait(&buffer[tid].condF, &buffer[tid].mtx);
       }
       if(done && buffer[tid].count == 0){

    	  break;
       }
       while(buffer[tid].count > 0){
           strcpy(inputTuple, buffer[tid].Tuples[outSlotIndex[tid]]);
           buffer[tid].count = buffer[tid].count - 1;
           outSlotIndex[tid] = (outSlotIndex[tid] + 1) % numOfSlots;
           each = strtok(inputTuple," ,()");
           userId = each;
           each = strtok(NULL,",()");
           topic1 = each;

           each = strtok(NULL," ,()");
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
       }
       pthread_mutex_unlock(&buffer[tid].mtx);
       pthread_cond_signal(&buffer[tid].condF);
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
	pthread_t tidMpr;
	long r;
	int i;
	numOfSlots = atoi(argv[1]);
	numOfReducers = atoi(argv[2]);
	pthread_t tidRdr[numOfReducers];

//Create Mapper Thread
	pthread_create(&tidMpr, NULL, Mapper, NULL);

//Create Reducer Threads
	for(r = 0; r < numOfReducers; r++){
		usleep(3000);
		pthread_create(&tidRdr[r], NULL, Reducer, (void *)r);
	}

	free(temp);
	pthread_exit(NULL);
}
