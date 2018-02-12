#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct { char action; int score; } Rules;
Rules rules[5] = {{'P',50}, {'L',20}, {'D',-10}, {'C',30}, {'S',40}};

int findScore(Rules* rules, char action){
  int i;
  for(i=0; i<5; i++){
    if(rules[i].action == action)
      return rules[i].score;
  }
  fprintf(stderr, "Action not present! Exiting...\n");
  exit(EXIT_FAILURE);

}

int main(int argc, char *argv[]){
  FILE *fp;
  char inputTuple[50], outputTuple[50];
  fp = fopen(argv[1], "r");
  int score;
  char *each;

  if (fp == NULL)
    exit(EXIT_FAILURE);
  while(fgets(inputTuple, sizeof inputTuple, fp) != NULL){
//    printf("%s ", inputTuple);
    each = strtok(inputTuple," ,()");
    printf("%s ", each);
    each = strtok(NULL," ,()");
    char q = *each;
    score = findScore(rules, q);
    each = strtok(NULL," ,()");
    printf("%s ", each);
    printf("%d \n", score);
//    score = findScore(rules, 'L');
//    printf("%d", score);
  }   

  fclose(fp);

  return 0;
}
