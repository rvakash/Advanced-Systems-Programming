//Author : Akash R Vasishta
//UFID : 53955080
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int numOfSlots;
int numOfReducers, str_idx, str_idx1;
//char** buffer;
char* temp;
char* temp1;
char** userToRdrMap = {NULL};
int *inSlotIndex;
int *outSlotIndex;

struct TopicTotal{ char userID[20]; char topic[20]; int score; };
int size = 0;


typedef struct { char action; int score; } Rules;
Rules rules[5] = {{'P',50}, {'L',20}, {'D',-10}, {'C',30}, {'S',40}};

typedef struct Buffer{ char **Tuples; } Buffer;
Buffer buffer [50];

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

void addToBuffer (int rdrNum, char *tuple) {
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
    buffer[rdrNum].Tuples[inSlotIndex[rdrNum]] = copy;
}

void printBuffer (struct Buffer buffer[numOfReducers]) {

	int i, j;
	for(i=0; i<numOfReducers; i++){
		for(j=0;j<numOfSlots; j++){
			printf("%s", buffer[i].Tuples[j]);
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

void *Mapper(void *threadid){
//	  struct userRdrMap userRdr[numOfReducers];
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
//	  struct Buffer buffer[numOfReducers];

	//  char space = "";
      userToRdrMap = malloc(numOfReducers * sizeof(char *));
      inSlotIndex = (int *) calloc(numOfReducers, sizeof(int));

      for(i=0; i<numOfReducers; i++){
    	  buffer[i].Tuples = malloc(numOfSlots * sizeof(char *));
      }
	  if (fp == NULL)
	    exit(EXIT_FAILURE);
	  while ((read = getline(&line, &len, stdin)) != -1) {
	    	printf("Here1\n");

	//    printf("%s || \n", line);
	    each = strtok(line," ,()");
	    while(each != NULL){
	    	printf("Here2\n");
	      if(u%3 == 0){
	        outputTuple = (char *) malloc(1 + strlen(each)+ strlen(oB) );
	        strcpy(outputTuple, oB);
	        strcat(outputTuple, each);
	        userId = each;
	//        printf("%s ", each);
	      } else if(u%3 == 1){
	        char q = *each;
	        score = findScoreFromAction(rules, q);
	      } else if(u%3 == 2){
	        char * str3 = (char *) malloc(1 + strlen(outputTuple)+ strlen(co) );
	        strcpy(str3, outputTuple);
	//    printf("%s-", str3);
	        strcat(str3, co);
	        char *str4 = (char *) malloc(1 + strlen(str3)+ strlen(each) );
	        strcpy(str4, str3);
	//        printf("$%s$", each);
	        strcat(str4, each);
	//////////////////////////////

	        for(pad = strlen(each); pad<15; pad++)
	          strcat(str4," ");
	//////////////////////////////
	//    printf("%s", str4);
	        sprintf(buf, "%s,%d)\n", str4, score);
	//    printf(",%d)\n", score);
	        printf("%s", buf);
			rdrNum = findRdrNum(userId);
			if(rdrNum > -1){
//				printf("The User ID \"%s\" exists at index %d!\n", userId, rdrNum);
			} else{
				addToUser(userId);
//				printf("New User ID %s added!\n", userId);
				rdrNum = findRdrNum(userId);
			}
			printf("rdrNum = %d ", rdrNum);
			printf("inSlotIndex[%d] = %d\n", rdrNum, inSlotIndex[rdrNum]);
			addToBuffer(rdrNum, buf);
//			buffer[rdrNum].tuple[inSlotIndex[rdrNum]] = buf;
			inSlotIndex[rdrNum] = (inSlotIndex[rdrNum] + 1) % numOfSlots;
//	        rdrNum = findRdrNum(userId);
	        free(outputTuple);
	        free(str3);
	        free(str4);

	      }
	    	printf("Here3\n");
	      each = strtok(NULL," ,()");
	      u++;
	    }
	//    char q = *each;
	//    score = findScore(rules, q);
	//    each = strtok(NULL," ,()");
    	printf("Here4\n");
	    u = 0;
	  }
	  done = 1;
	  printBuffer(buffer);
	  printf("Last\n");
	  for (i = 0; i < str_idx; i++) {
	      free(userToRdrMap[i]);
	  }
	  for(i=0; i<numOfReducers; i++){
		  for(j=0; j<numOfSlots; j++)
			  free(buffer[i].Tuples[j]);
	  }
	  printf("LAST0\n");
//	  for(i=0; i<numOfReducers; i++)
//		  free(buffer[i]);
	  printf("LAST0.5\n");
	  free(userToRdrMap);
	  free(inSlotIndex);
	  printf("LAST1\n");
   pthread_exit(NULL);
}

void *Reducer(void *threadid){
   long tid;
   tid = (long)threadid;
   printf("Hello World! I am Reducer thread #%ld!\n", tid);
   //  struct TopicTotal *topictotal = malloc(sizeof(struct TopicTotal) * 2);
     struct TopicTotal topictotal[50];
     FILE *fp;
     char *each;
     char *userId, *newScore;
     char inputTuple[30], outputTuple[50];
     char firstuserId[4];
   //  fp = fopen(argv[1], "r");
     int firstTime = 1;
     int ti = 0;
     int j = 0;
   //  struct TopicTotal *topictotal = malloc(sizeof(struct TopicTotal) * 2);
     int score, index;
     char *topic1;
     index = 0;
     int equal = 1;
     outSlotIndex = (int *) calloc(numOfReducers, sizeof(int));

     while(buffer[tid].Tuples[outSlotIndex[tid]] != NULL ){
       outSlotIndex[tid] = (outSlotIndex[tid] + 1) % numOfSlots;

       inputTuple = buffer[tid].Tuples[outSlotIndex[tid]];
//     while(fgets(inputTuple, sizeof inputTuple, stdin) != NULL){
/*
   //  while(read(STDIN_FILENO, inputTuple, sizeof(inputTuple))){
   //    while(fscanf(stdin, "%*s %99[^\n]", inputTuple) != EOF){
   //    printf("zzzzzzzzzzzzzzzzz input = %s\n", inputTuple);
   //    inputTuple[strlen(inputTuple)-1] = '\0';
   //    printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
*/

       each = strtok(inputTuple," ,()");
       userId = each;
   //    printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
       each = strtok(NULL,",()");
       topic1 = each;

         each = strtok(NULL," ,()");
         newScore = each;
   //      printf("%s\n", each);
   //      printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
   //      size = sizeof(topictotal)/sizeof(topictotal[0]);
         score = findScore(topictotal, userId, topic1, size);
   //      printf("Back to main.");
         if(score == -1){
/*
   //        printf("Topic not found. \n");
   //	while(*topic1 != '\0'){
   //		topictotal[index].topic[ti++] = *topic1++;
   //	}
*/
           strcpy(topictotal[index].userID, userId);
           strcpy(topictotal[index].topic, topic1);
           ti = 0;
   //        topictotal[index].topic = topic1;
           topictotal[index].score = atoi(newScore);
           size = size + 1;
   //        printf("//1%s %s %d\n", topictotal[index].userID, topictotal[index].topic, topictotal[index].score);
           index = index + 1;
   //        printf("Added new topic.\n");
         } else {
   //        printf("Topic found. Updating score\n");
           updateScore(topictotal, userId, topic1, newScore, size);
   //        printf("**1%s %s %d\n", topictotal[index].userID, topictotal[index].topic, topictotal[index].score);
         }

     }
     for(j; j< index; j++){
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
/*	pthread_t tidRdr[numOfReducers];
	buffer = malloc(numOfReducers * sizeof(char*));
	temp = malloc(numOfReducers * numOfSlots * sizeof(char));
	for (i = 0; i < numOfReducers; i++) {
	  buffer[i] = temp + (i * numOfSlots);
	}
*/
//	userToRdrMap = malloc(sizeof(char*) * numOfReducers);
/*	userToRdrMap = malloc(numOfReducers * sizeof(char*));
	temp1 = malloc(numOfReducers * 5 * sizeof(char));
	for (i = 0; i < numOfReducers; i++) {
	  userToRdrMap[i] = temp1 + (i * 5);
	}
*/
//Create Mapper Thread
	pthread_create(&tidMpr, NULL, Mapper, NULL);
/*
//Create Reducer Threads
	for(r = 0; r < numOfReducers; r++){
		pthread_create(&tidRdr[r], NULL, Reducer, (void *)r);
	}
*/
//Wait for Mapper Thread to finish
	pthread_join(tidMpr, NULL);
/*
//Wait for Reducer Threads to finish
	for(r = 0; r < numOfReducers; r++){
		pthread_join(tidRdr[r], NULL);
	}
*/
	free(temp);
//	free(buffer);

	pthread_exit(NULL);
}
