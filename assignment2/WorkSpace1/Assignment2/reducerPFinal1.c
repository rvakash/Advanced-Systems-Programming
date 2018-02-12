//Final Working
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
//typedef struct TopicTotal topictotal[50];
struct TopicTotal{ char userID[20]; char topic[20]; int score; };
int size = 0;

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

int main(int argc, char *argv[]){
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
//  topictotal[0].topic = "initial";
//  topictotal[0].score = 0;

//  if (fp == NULL)
//    exit(EXIT_FAILURE);
  while(fgets(inputTuple, sizeof inputTuple, stdin) != NULL){
//  while(read(STDIN_FILENO, inputTuple, sizeof(inputTuple))){
//    while(fscanf(stdin, "%*s %99[^\n]", inputTuple) != EOF){
//    printf("zzzzzzzzzzzzzzzzz input = %s\n", inputTuple);
//    inputTuple[strlen(inputTuple)-1] = '\0';
//    printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);

    each = strtok(inputTuple," ,()");
    userId = each;
//    printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
    each = strtok(NULL,",()");
    topic1 = each;
//    printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
//    printf("$%s$\n", each);
//    if(each != userId){
//      printf("New User Id entered. Old User Id details are - \n");
//    }
//    userId = each;
/*    if(firstTime){
//      printf("Coming here everytime\n");
//      firstuserId = userId;
      strcpy(firstuserId, userId);
      firstTime = 0;

    }
*/
//    char *firstuserId_ptr = &firstuserId[0];
//    printf("%c%c\n", *userId, firstLetter);
//    printf("%s %s\n", userId, firstuserId);
//    if(strcmp(userId, firstuserId_ptr) == 0){
//      printf("old user\n");
//      printf("%s ", userId);
//      printf("$%s$", topic1);
      each = strtok(NULL," ,()");
      newScore = each;
//      printf("%s\n", each);
//      printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
//      size = sizeof(topictotal)/sizeof(topictotal[0]);
      score = findScore(topictotal, userId, topic1, size);
//      printf("Back to main.");
      if(score == -1){
//        printf("Topic not found. \n");
//	while(*topic1 != '\0'){
//		topictotal[index].topic[ti++] = *topic1++;
//	}
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

/*
//    } else {
//      printf("New userId\n");

//      for(j; j< index; j++){
//        printf("(%s,%s,%d)\n", topictotal[j].userID, topictotal[j].topic, topictotal[j].score);
//      }

      //      firstuserId = userId;
//      strcpy(firstuserId, userId);
//      printf("%s ", each);
//      topic1 = each;
      each = strtok(NULL," ,()");
      newScore = each;
//      printf("%s\n", each);
      score = findScore(topictotal, userId, topic1, size);
//      printf("Back to main.");
      if(score == -1){
//        printf("New user. Topic not found. \n");
//	while(*topic1 != '\0'){
//		topictotal[index].topic[ti++] = *topic1++;
//	}
        strcpy(topictotal[index].userID, userId);
        strcpy(topictotal[index].topic, topic1);
        ti = 0;
//        topictotal[index].topic = topic1;
        topictotal[index].score = atoi(newScore);
        size = size + 1;
//        printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
//        printf("//2%s %s %d\n", topictotal[index].userID, topictotal[index].topic, topictotal[index].score);
        index = index + 1;
//        printf("Added new topic.\n");
      } else {
//        printf("Topic found. Updating score\n");
//        printf("2");
        updateScore(topictotal, userId, topic1, newScore, size);

//        printf("**2%s %s %d\n", topictotal[index].userID, topictotal[index].topic, topictotal[index].score);
      }
//      printf("End of else\n");
    }

//    printf("%c ", firstLetter);
//   each = strtok(NULL," ,()");
//    topic = each;
//    printf("%s \n", each);

//    each = strtok(NULL," ,()");
//    newScore = each;
//    printf("%s \n", newScore);
//    printf("xxxxxxxxxxxxxxxxxxxxxxx\n");
*/
  }
  for(j; j< index; j++){
    printf("(%s,%s,%d)\n", topictotal[j].userID, topictotal[j].topic, topictotal[j].score);
  }
//  free(topictotal);
//  fclose(fp);
  return 0;
}
