#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//typedef struct TopicTotal topictotal[50];
struct TopicTotal{ char topic[20]; int score; };
int size = 0;

int findScore(struct TopicTotal topictotal[50], char* topic1, int size){
  int i;
  int ti = 0;
  int equal = 0;
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
    while(*topic1 != '\0'){
//    printf(" | topictotal[%d].topic[%d] = %c === *topic1++ = %c ", i, ti, topictotal[i].topic[ti], *topic1);
      if(topictotal[i].topic[ti++] == *topic1++)
        equal = 1;
      else{
        equal = 0;
        break;
      }
    }
    if (equal == 1){
//      printf("Inside if. Topic found.\n");
      return topictotal[i].score;
    }
    ti = 0;
    topic1 = temp;
  }
  return -1;
}

int updateScore(struct TopicTotal topictotal[50], char* topic1, int size){
  int i;
  int equal = 0;
  int ti = 0;
  char* temp = topic1;
  for(i=0; i<size; i++){
/*    if(topictotal[i].topic == topic){
      topictotal[i].score = topictotal[i].score + 1;
//      size = size + 1;
    }
*/
    while(*topic1 != '\0'){
      if(topictotal[i].topic[ti++] == *topic1++)
        equal = 1;
      else{
        equal = 0;
        break;
      }
    }
    if (equal == 1){
      topictotal[i].score = topictotal[i].score + 1;
    }
    topic1 = temp;
    ti = 0;
  }
}

int main(int argc, char *argv[]){
//  struct TopicTotal *topictotal = malloc(sizeof(struct TopicTotal) * 2);
  struct TopicTotal topictotal[50];
  FILE *fp;
  char *each;
  char *userId, *newScore;
  char inputTuple[50], outputTuple[50];
  char firstLetter;
  fp = fopen(argv[1], "r");
  int firstTime = 1;
  int ti = 0;
  int j = 0;
//  struct TopicTotal *topictotal = malloc(sizeof(struct TopicTotal) * 2);
  int score, index;
  char *topic1;
  index = 0;
//  topictotal[0].topic = "initial";
//  topictotal[0].score = 0;
  
  if (fp == NULL)
    exit(EXIT_FAILURE);
  while(fgets(inputTuple, sizeof inputTuple, fp) != NULL){
//    printf("zzzzzzzzzzzzzzzzz\n");
//    inputTuple[strlen(inputTuple)-1] = '\0';
//    printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);

    each = strtok(inputTuple," ,()");
//    printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
    topic1 = each;
//    printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);

//    if(each != userId){
//      printf("New User Id entered. Old User Id details are - \n");
//    }
//    userId = each;
    if(firstTime){
      firstLetter = *each;
      firstTime = 0;
    }
    
    if(*each == firstLetter){
//      printf("%s ", topic1);
      each = strtok(NULL," ,()");
      newScore = each;
//      printf("%s\n", each);
//      printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
//      size = sizeof(topictotal)/sizeof(topictotal[0]);
      score = findScore(topictotal, topic1, size);
//      printf("Back to main.");
      if(score == -1){
//        printf("Topic not found. \n");
//	while(*topic1 != '\0'){
//		topictotal[index].topic[ti++] = *topic1++;
//	}
        strcpy(topictotal[index].topic, topic1);
        ti = 0;
//        topictotal[index].topic = topic1;
        topictotal[index].score = 1;
        size = size + 1;
//        printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
        index = index + 1;
//        printf("Added new topic.\n");
      } else {
//        printf("Topic found. Updating score\n");
        updateScore(topictotal, topic1, size);
      }

    } else {
//      printf("New Topic\n");
      
      for(j; j< index; j++){
        printf("%s  %d\n", topictotal[j].topic, topictotal[j].score);
      }
      firstLetter = *each;
      printf("%s ", each);
      topic1 = each;
      each = strtok(NULL," ,()");
      printf("%s\n", each);  
      score = findScore(topictotal, topic1, size);
//      printf("Back to main.");
      if(score == -1){
//        printf("Topic not found. \n");
//	while(*topic1 != '\0'){
//		topictotal[index].topic[ti++] = *topic1++;
//	}
        strcpy(topictotal[index].topic, topic1);
        ti = 0;
//        topictotal[index].topic = topic1;
        topictotal[index].score = 1;
        size = size + 1;
//        printf("topictotal[%d].topic = %s\n", index, topictotal[index].topic);
        index = index + 1;
//        printf("Added new topic.\n");
      } else {
//        printf("Topic found. Updating score\n");
        updateScore(topictotal, topic1, size);
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
  }
  for(j; j< index; j++){
    printf("%s  %d\n", topictotal[j].topic, topictotal[j].score);
  }
//  free(topictotal);
  fclose(fp);
  return 0;
}



