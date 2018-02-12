#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int numOfSlots;
int numOfReducers;
char **userToRdrMap;
char* temp1;


// 1: The index of the next free pointer. 2: The total number of pointers available.
int str_idx, numOfReducers;



int findRdrNum(char *userId){
	int i;
    for (i = 0; i < str_idx; i++) {
        // If string at index i == string we're looking for.
        if (strcmp(userId, userToRdrMap[i]) == 0) {
            return i;
        }
    }

    return -1;
}
// Copies given string into strings. If necessary, strings is resized.
void addToUser (char *userId) {
    char *copy = NULL;

    // 1. Ensure we have space. Otherwise resize strings array.
    if (str_idx >= numOfReducers) {
        numOfReducers = numOfReducers * 2;
        userToRdrMap = realloc(userToRdrMap, numOfReducers * sizeof(char *));
    }

    // 2. Allocate space for copy of string. Print an error if it failed.
    copy = malloc(strlen(userId) * sizeof(char));
    if (copy == NULL) {
        printf("Error: Allocation failure in \"installString\"!\n");
        exit(EXIT_FAILURE);
    }

    // 3. Copy the contents over.
    copy = strcpy(copy, userId);

    // 4. Place the pointer (copy) at str_idx (next available spot), and post-increment str_idx.
    userToRdrMap[str_idx++] = copy;
}


int main (int argc, char *argv[])
{
	numOfSlots = atoi(argv[1]);
	numOfReducers = atoi(argv[2]);
	int i;
	char *userId[numOfReducers];
	userId[0] = "2234";
	userId[1] = "2222";
	userId[2] = "3223";
	userId[3] = "2222";
	int rdrNum;

//	userToRdrMap = malloc(numOfReducers * sizeof(char*));
/*	temp1 = malloc(numOfReducers * 4 * sizeof(char));
	for (i = 0; i < numOfReducers; i++) {
	  userToRdrMap[i] = temp1 + (i * 4);
	}
*/

//    numOfReducers = numOfReducers;
    userToRdrMap = malloc(numOfReducers * sizeof(char *));

	printf("%s\n", userId[0]);
	for(i=0; i<numOfReducers; i++){
		rdrNum = findRdrNum(userId[i]);
		if(rdrNum > -1){
			printf("The User ID \"%s\" exists at index %d!\n", userId[i], rdrNum);
		} else{
			addToUser(userId[i]);
			printf("New User ID %s added!\n", userId[i]);
		}
//		strcpy(userToRdrMap[rdrNum], userId[i]);
//		printf("%d", rdrNum);
	}

    for (i = 0; i < str_idx; i++) {
        free(userToRdrMap[i]);
    }
    free(userToRdrMap);
	return EXIT_SUCCESS;
}
