#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct { char action; int score; } TopicTotal;


int findScore(TopicTotal* topictotal, char action){
  int i;
  for(i=0; i<5; i++){
    if(topictotal[i].action == action)
      return topictotal[i].score;
  }
  fprintf(stderr, "Action not present! Exiting...\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
  FILE *fp;
  char *each;
  char *userId, *topic, *newScore;
  char inputTuple[50], outputTuple[50];
  fp = fopen(argv[1], "r");

  if (fp == NULL)
    exit(EXIT_FAILURE);
  while(fgets(inputTuple, sizeof inputTuple, fp) != NULL){
    each = strtok(inputTuple," ,()");
    if(each != userId){
      printf("New User Id entered. Old User Id details are - \n");
    }
    userId = each;
    printf("%s ", userId);
    each = strtok(NULL," ,()");
    topic = each;
    printf("%s ", topic);
    each = strtok(NULL," ,()");
    newScore = each;
    printf("%s \n", newScore);
  }
  fclose(fp);
  return 0;
}
