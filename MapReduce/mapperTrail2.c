//Final working
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

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
  char inputTuple[30] = {};
  char buf[30] = {};
  char* oB;
  oB = "(";
  char* cB;
  char* co = ",";
  cB = ")";
  fp = fopen(argv[1], "r");
  int score;
  char *each;
  size_t len = 0;
  ssize_t read;
  char * line = NULL;
  char *eacharray;
  int u = 0;
  char *outputTuple;
  int pad = 0;
  int g;
//  char space = "";
  if (fp == NULL)
    exit(EXIT_FAILURE);
  while ((read = getline(&line, &len, fp)) != -1) {

//    printf("%s || \n", line);
    each = strtok(line," ,()");
    while(each != NULL){

      if(u%3 == 0){
        outputTuple = (char *) malloc(1 + strlen(each)+ strlen(oB) );
        strcpy(outputTuple, oB);
        strcat(outputTuple, each);
//        printf("%s ", each);
      } else if(u%3 == 1){
        char q = *each;
        score = findScore(rules, q);
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
        free(outputTuple);
        free(str3);
        free(str4);
      }
      each = strtok(NULL," ,()");
      u++;
    }
//    char q = *each;
//    score = findScore(rules, q);
//    each = strtok(NULL," ,()");

    u = 0;
  }   
//  printf("\0");
  fclose(fp);

  return 0;
}
