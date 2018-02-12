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
int numOfSlots;
int numOfReducers, str_idx, str_idx1;
char** userToRdrMap = {NULL};

struct TopicTotal{ char userID[20]; char topic[20]; int score; };
int size = 0;

typedef struct { char action; int score; } Rules;
Rules rules[5] = {{'P',50}, {'L',20}, {'D',-10}, {'C',30}, {'S',40}};

typedef struct Object{
	char **Tuples;
	pthread_mutex_t mtx;
	pthread_cond_t  condE;
	pthread_cond_t  condF;
	int count;
} Object;
Object object[50];
int done;


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
    if (str_idx >= 4) {
//        numOfReducers = numOfReducers * 2;
        userToRdrMap = realloc(userToRdrMap, 8 * sizeof(char *));
    }
    copy = malloc(strlen(userId) * sizeof(char));
///*    if (copy == NULL) {
//        printf("Error: Allocation failure in \"installString\"!\n");
//        exit(EXIT_FAILURE);
//    }
//*/
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

void addToObject (int rdrNum, char *tuple) {
    char *copy = NULL;
/*    if (str_idx1 >= numOfSlots) {
        numOfSlots = numOfSlots * 2;
        buffer[rdrNum] = realloc(buffer[rdrNum], numOfSlots * sizeof(char *));
    }
*/
    copy = malloc(strlen(tuple) * sizeof(char));
///*    if (copy == NULL) {
//        printf("Error: Allocation failure in \"installString\"!\n");
//        exit(EXIT_FAILURE);
//    }
//*/
    copy = strcpy(copy, tuple);
    object[rdrNum].Tuples[slot[rdrNum]] = copy;
    object[rdrNum].count = object[rdrNum].count + 1;
}

void printObject (struct Object object[4]) {

	int i, j;
	for(i=0; i<4; i++){
		for(j=0;j<10; j++){
			printf("%s", object[i].Tuples[j]);
		}
		printf("\n");
	}
	printf("done printing object\n");
}

int findScore(struct TopicTotal topictotal[50], char* userId, char* topic1, int size){
  int i;
  int ti = 0;
  int equal = 0;
  int equaluser = 0;
  char *tempuser = userId;
  char *temp = topic1;
//  printf("Inside findScore. topic1 = %s, size = %d\n", topic1, size);
//  printf("[0] = %s\n", topictotal[0].topic);
  for(i=0; i<size; i++){
//    printf(" | topictotal[%d].topic = %s | ", i, topictotal[i].topic);
/*    if(topictotal[i].topic == topic1){
      printf("Inside if. Topic found.\n");
      return topictotal[i].score;
    }
*/
    while(*userId != '\0'){
//    printf(" | topictotal[%d].topic[%d] = %c === *topic1++ = %c ", i, ti, topictotal[i].topic[ti], *topic1);
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
//      printf(" | topictotal[%d].topic[%d] = %c === *topic1++ = %c ", i, ti, topictotal[i].topic[ti], *topic1);
        if(topictotal[i].topic[ti++] == *topic1++)
          equal = 1;
        else{
          equal = 0;
          break;
        }
      }
      if (equal == 1){
//        printf("Inside if. Topic found.\n");
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
/*    if(topictotal[i].topic == topic){
      topictotal[i].score = topictotal[i].score + 1;
//      size = size + 1;
    }
*/
    while(*userId != '\0'){
//    printf(" | topictotal[%d].topic[%d] = %c === *topic1++ = %c ", i, ti, topictotal[i].topic[ti], *topic1);
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
//	printf("newScore = %d", atoi(newScore));
        topictotal[i].score = topictotal[i].score + atoi(newScore);
//        printf("**1%s %s %d\n", topictotal[i].userID, topictotal[i].topic, topictotal[i].score);
      }
      topic1 = temp;
      ti = 0;
    }
  }
}


void *Producer(void *threadid){
	int i, value, index = 0;
	size_t len = 0;
	ssize_t read;
	char * line = NULL;
	char *each;
	  FILE *fp;
	  char inputTuple[30] = {};
	  char buf[30] = {};
	  char* oB;
	  oB = "(";
	  char* cB;
	  char* co = ",";
	  cB = ")";
	  int score;
	  char *userId;
	  char *eacharray;
	  int u = 0;
	  char *outputTuple;
	  int pad = 0;
	  int j, rdrNum;
      userToRdrMap = malloc(4 * sizeof(char *));
      for(i=0; i<4; i++){
    	  object[i].Tuples = malloc(numOfSlots * sizeof(char *));
    	  object[i].mtx = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    	  object[i].condE = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    	  object[i].condF = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    	  object[i].count = 0;
      }

	while(1){
		  read = getline(&line, &len, stdin);
		  if(read == -1){
			  done = 1;
			  for(i=0; i<4; i++){
				  printf("____________HERE_________________\n");
				  pthread_cond_signal(&object[i].condF);
				  pthread_cond_signal(&object[i].condE);
			  }
			  break;
		  }
//		  printf("line = %s\n", line);
	      each = strtok(line," ,()");
		  while(each != NULL){
//			  printf("here");
			  if(u%3 == 0){
//				   printf("First if \n");
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
			       printf("Mapper Output = %s\n", buf);
			       rdrNum = findRdrNum(userId);
			       if(rdrNum > -1){
			       } else{
			    	   addToUser(userId);
			    	   rdrNum = findRdrNum(userId);
			       }
			       pthread_mutex_lock(&object[rdrNum].mtx);
			       while(object[rdrNum].count == 10){
//			    	   printf("here");
			    	   pthread_cond_wait(&object[rdrNum].condF, &object[rdrNum].mtx);
			       }
//			  printf("%d", value);
//			       object[rdrNum].array[slot[rdrNum]] = value;
			       addToObject(rdrNum, buf);
//			       object[rdrNum].count = object[rdrNum].count + 1;
			       slot[rdrNum] = (slot[rdrNum] + 1) % 10;
			       pthread_cond_signal(&object[rdrNum].condE);
			       pthread_mutex_unlock(&object[rdrNum].mtx);
			       free(outputTuple);
			       free(str3);
			       free(str4);
			  }
			  each = strtok(NULL," ,()");
 			  u++;
		  }
		  u = 0;

	}
	for (i = 0; i < str_idx; i++) {
	    free(userToRdrMap[i]);
	}
	for(i=0; i<4; i++){
	    for(j=0; j<10; j++)
		   free(object[i].Tuples[j]);
	}

	free(userToRdrMap);
//    free(slot);
	printf("Finished Mapper execution. Now Exiting\n");
 	pthread_exit(NULL);
}
void *Consumer(void *threadid){
	long tid;
	tid = (long)threadid;
	int i, score, rdrNum, firstTime, ti, j, equal, index;
    printf("Hello World! I am Reducer thread #%ld!\n", tid);
    struct TopicTotal topictotal[50];
    FILE *fp;
    char *each, *userId, *newScore, *topic1;
    char inputTuple[30], outputTuple[50], firstuserId[4];
    firstTime = ti = j = equal = 1;
    rdrNum = index = 0;
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
        object[tid].count = object[tid].count - 1;
        strcpy(inputTuple, object[tid].Tuples[slot2[tid]]);
        slot2[tid] = (slot2[tid] + 1) % 10;
        pthread_cond_signal(&object[tid].condF);
        pthread_mutex_unlock(&object[tid].mtx);

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
    for(j=0; j< index; j++){
      printf("(%s,%s,%d)\n", topictotal[j].userID, topictotal[j].topic, topictotal[j].score);
    }

	pthread_exit(NULL);
}


int main (int argc, char *argv[])
{

//	numOfSlots = atoi(argv[1]);
//	numOfReducers = atoi(argv[2]);
	pthread_t tidPr;
	pthread_t tidCr[4];
	long r;
//Create Producer Thread
	pthread_create(&tidPr, NULL, Producer, NULL);
////Create Consumer Threads
//	for(r = 0; r < 4; r++){
//		pthread_create(&tidCr[r], NULL, Consumer, (void *)r);
//	}
//Wait for Producer Thread to finish
	pthread_join(tidPr, NULL);
////Wait for Consumer Threads to finish
//	for(r = 0; r < 4; r++){
//		pthread_join(tidCr[r], NULL);
//	}
	pthread_exit(NULL);
}
