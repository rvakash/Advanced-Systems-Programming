#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <string.h>

typedef struct Buffer{
	char **Tuples;
	// int count;
	sem_t buf_mutex,empty_count,fill_count;
	int inSlotIndex;
	int outSlotIndex;
} Buffer;
Buffer *buffer;
int numOfSlots;
int numOfReducers, str_idx, str_idx1;
int *done;

char** userToRdrMap = {NULL};

typedef struct { char action; int score; } Rules;
Rules rules[5] = {{'P',50}, {'L',20}, {'D',-10}, {'C',30}, {'S',40}};

struct TopicTotal{
      char userID[20];
      char topic[20];
      int score; };
int size = 0;

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
    int index = buffer[rdrNum].inSlotIndex;
    strcpy(buffer[rdrNum].Tuples[index], tuple);
    // printf("A2B - buffer[%d].Tuples[%d] = %s\n",rdrNum, index, buffer[rdrNum].Tuples[index] );
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

void mapper(){
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
	userToRdrMap = malloc(numOfReducers * sizeof(char *));

    while (1) {
        read = getline(&line, &len, stdin);
        if(read == -1){
            *done = 1;
        // sem_post(&buffer[rdrNum].empty_count);            
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
                sem_wait(&buffer[rdrNum].empty_count);
                sem_wait(&buffer[rdrNum].buf_mutex);
                addToBuffer(rdrNum, buf);
                buffer[rdrNum].inSlotIndex = (buffer[rdrNum].inSlotIndex + 1) % (numOfSlots);
                free(outputTuple);
                free(str3);
                free(str4);
                // printf("completed\n");
                sem_post(&buffer[rdrNum].buf_mutex);
                sem_post(&buffer[rdrNum].fill_count);
            }  
            u++;
            each = strtok(NULL," ,()");
        }
        u = 0;
    }
    for(i=0; i<numOfReducers; i++){
        sem_post(&buffer[i].fill_count);
    }


/*    char *temp = "trail";
    sem_wait(&buffer[1].empty_count);
    sem_wait(&buffer[1].buf_mutex);
	buffer[1].Tuples[0] = temp;
	buffer[1].inSlotIndex = 3;
	buffer[1].outSlotIndex = 4;
    sem_post(&buffer[1].buf_mutex);
    sem_post(&buffer[1].fill_count);
    *done = 1;
*/
}

void reducer(long tid, Buffer *buffer, int *done){
    struct TopicTotal topictotal[50];
    FILE *fp;
    char *each, *userId, *newScore, *topic1;
    char inputTuple[30], outputTuple[50], firstuserId[4];
    int firstTime, equal, ti,j, i, score, index;
    ti = j = i = index = 0;
    equal = firstTime = 1;
    int outindex=0;

    while(1){
        outindex = buffer[tid].outSlotIndex;
        sem_wait(&buffer[tid].fill_count);
        sem_wait(&buffer[tid].buf_mutex);
        if(*done && buffer[tid].inSlotIndex == outindex){
              break;
        }
        strcpy(inputTuple, buffer[tid].Tuples[outindex]);
        char *new = inputTuple;
        buffer[tid].outSlotIndex = (buffer[tid].outSlotIndex + 1) % numOfSlots;
        outindex = buffer[tid].outSlotIndex;
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
        sem_post(&buffer[tid].buf_mutex);
        sem_post(&buffer[tid].empty_count);
    }
    for(j=0; j< index; j++){
        printf("(%s,%s,%d)\n", topictotal[j].userID, topictotal[j].topic, topictotal[j].score);
    }


/*    sem_wait(&buffer[tid].fill_count);
	sem_wait(&buffer[tid].buf_mutex);
	printf("here%s\n", buffer[tid].Tuples[0]);
	printf("here%d\n", buffer[tid].inSlotIndex);
	printf("here%d\n", buffer[tid].outSlotIndex);
	sem_post(&buffer[tid].buf_mutex);
 	sem_post(&buffer[tid].empty_count);
 	if(*done == 1)
 		printf("DONE\n");
*/
}


int main (int argc, char *argv[])
{
  	if(argc != 3) {
    	if(argc < 3)
 	        printf("Insufficient arguments passed\n");
	    else
	        printf("Too many arguments passed\n");
        return 1;
    }
	srand(time(NULL));
	int i, j;
  	long r;
  	char *temp;
    numOfSlots = atoi(argv[1]);
	numOfReducers = atoi(argv[2]);

    buffer = (struct Buffer *)mmap(NULL, numOfReducers * sizeof(struct Buffer), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(buffer == MAP_FAILED){
        printf("EXITING");
        exit(EXIT_FAILURE);
    }
    done = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (done  == MAP_FAILED){
        printf("exiting\n");
    }
    *done = 0;

    for(i=0; i<numOfReducers; i++){
        buffer[i].Tuples = mmap(NULL, numOfSlots * sizeof(char *), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if(buffer[i].Tuples == MAP_FAILED){
            printf("EXITING");
            exit(EXIT_FAILURE);
        }
        for(j=0; j<numOfSlots; j++){
            temp = (char *)mmap(NULL, 30 * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
            temp = strcpy(temp, "");
            buffer[i].Tuples[j] = temp;
        }
        sem_init(&buffer[i].buf_mutex, 1, 1);
  	    sem_init(&buffer[i].fill_count, 1, 0);
  	    sem_init(&buffer[i].empty_count, 1, numOfSlots);
    }

    for(r=0;r<numOfReducers;r++){ // loop will run n times (n=5)
        if(fork() == 0){
            // printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
            reducer(r, buffer, done);
            exit(0);
        }
    }
    mapper();
    for(r=0;r<numOfReducers;r++) // loop will run n times (n=5)
    wait(NULL);
}