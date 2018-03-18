
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h> /* Declares errno and defines error constants */
#include <semaphore.h>


// #include "tlpi_hdr.h"
// #define _BSD_SOURCE

//global variables
int BUFFER_SIZE = 0;
int* done;


// structure definations:
struct node{
        int val;
        char c;
        struct node* next;
};

struct tuple{
        int id;
        int score;
        char topic[16];
        struct tuple* next;
};



// structure for shared buffer:
struct buffer{
        struct tuple* tup;
        int count, in, out;
        sem_t sem;
        sem_t full;
        sem_t empty;
};



void errExit(char *string){
    printf("%s\n",string);
}

//Functions related to consumer:

//function to print tuples linked list
void tupprint(struct tuple* head){
        // printf("End of Thread, printing linkedlist\n");
        int count =0;
        if(head == NULL)
                printf("Nothing to print\n");
        while(1){
                count++;
                printf("(%04d,%s,%d)\n",head->id,head->topic,head->score );
                if (head->next ==NULL)
                        break;
                head = head->next;
        }
        // printf("count = %d\n",count);
}

struct tuple* insertTuple (struct tuple* head,struct tuple* temp){
        // printf("inserting tuple:(%04d,%s,%d)\n",temp->id,temp->topic,temp->score);

        struct tuple* iter=(struct tuple *)head;
        if(head==NULL)
                head = temp;
        else{
                while(1){

                        if(strcmp(iter->topic,temp->topic) == 0){
                                iter->score+=temp->score;
                                // free(temp->topic);
                                // free(temp);
                                break;
                        }

                        if(iter->next == NULL){
                                iter->next = temp;
                                break;
                        }
                        iter = iter->next;
                }

        }
        return head;
}


// function to copy tuples
void tupcpy(struct tuple* out,struct tuple* in){
        out->id=in->id;
        out->score = in->score;
        strcpy(out->topic,in->topic);
        out->next = NULL;

}



// reducer thread function
void reducer (void* var){

        int s;
        struct buffer* buff = (struct buffer *) var;
        struct tuple* head = NULL;
        struct tuple* temp = NULL;
        struct tuple* tpr = buff->tup;
        // printf("reducer started\n");
        while (1){


                if (sem_wait(&(buff->sem)) == -1)       //acquire buffer lock
                                    errExit("sem_wait");
                

                while(buff->count==0 && !(*done)){
                    if (sem_post(&(buff->sem)) == -1)               // release buffer lock
                        errExit("sem_post");
                    if (sem_wait(&(buff->empty)) == -1)               // release buffer lock
                        errExit("sem_post");
                    if (sem_wait(&(buff->sem)) == -1)       //acquire buffer lock
                        errExit("sem_wait"); 
                }
                        

                while(buff->count){
                        temp = (struct tuple *) malloc(sizeof(struct tuple));
                        
                        //create a copy of tuple from the buffer

                        tupcpy(temp,&tpr[buff->out]);
                        temp->next = NULL;

                        //insert tuple into a linkedlist
                        head = insertTuple(head,temp);

                        //move the out pointer
                        buff->out = (buff->out+1)%BUFFER_SIZE;

                        //decrement buffer counter
                        buff->count--;
                }

                if (sem_post(&(buff->sem)) == -1)               // release buffer lock
                        errExit("sem_post");
                if (sem_post(&(buff->full)) == -1)               // release buffer lock
                        errExit("sem_post");
                if(*done)
                        break;


        }
        
        tupprint(head);
        exit(0);
}







// Functions related to producer:
int searchKey(struct node* rules, char c){
        while(rules){
                if(rules->c== c){
                        return rules->val;
                }
                else
                        rules = rules->next;
        }
        return -1;
}

int idtoindex(int* arr, int id,int count){

        for(int i=0;i<count;i++){

                if(arr[i]==id)
                        return i;
        }

        return -1;
}

static void* mapper(void *temp){

        struct node * rules = (struct node *) temp;
        struct buffer *bfrptr,*ptrbfptr[500];
        struct tuple *tplptr;
        char buff[10000];
        int threadCount=0,arr[500],k,s;
        pthread_t tid[500];
        FILE *fp = fopen("input.txt","r");

        while(fgets(buff,10000,stdin)){

                if(strlen(buff)==0){
                        printf("empty file\n" );
                        exit(1);
                }

                char* token;

                token=strtok(&buff[1],",");

                struct tuple* tup = (struct tuple *) malloc(sizeof(struct tuple));

                while (token){
                        tup->id=atoi(token);

                        token = strtok(NULL,",");
                        tup->score = searchKey(rules,token[0]);

                        token = strtok(NULL,")");
                        strcpy(tup->topic,token);

                        token = strtok(NULL,"(");
                        if(token)
                                token = strtok(NULL,",");

                        // printf("(%04d,%s,%d)\n",tup->id,tup->topic,tup->score);
                        k = idtoindex(arr,tup->id,threadCount);
                        // printf("k=%d\n",k );
                        if(k == -1){
                                //create buffer

                                // addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
                                
                                // bfrptr = (struct buffer *) malloc(sizeof(struct buffer));
                                bfrptr = (struct buffer *) mmap(NULL, sizeof(struct buffer), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
                                bfrptr->tup = (struct tuple *) mmap(NULL, sizeof(struct tuple), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
                                // bfrptr-> tup = (struct tuple *) malloc (sizeof(struct tuple));
                                
                                //initialize variables
                                bfrptr->count = 0;
                                bfrptr->in = 0;
                                bfrptr->out = 0;

                                //initialize all the semaphores
                                if (sem_init(&(bfrptr->sem), 1/*shared*/, 1) == -1)
                                    errExit("sem_init");
                                if (sem_init(&(bfrptr->full), 1/*shared*/, 0) == -1)
                                    errExit("sem_init");
                                if (sem_init(&(bfrptr->empty), 1/*shared*/, 0) == -1)
                                    errExit("sem_init");

                                
                                //add buffer to ptrbuffptr
                                ptrbfptr[threadCount] = bfrptr;
                                arr[threadCount] = tup->id;

                                //fork and create reducer process
                                
                                switch (fork()) {           /* Parent and child share mapping */
                                    case -1:
                                        errExit("fork");

                                    case 0:                     /* Child: increment shared integer and exit */
                                        // printf("Reducer process started, id = %d\n", threadCount);
                                        reducer(bfrptr);

                                }
                                threadCount++;


                                // if(pthread_create(&tid[threadCount++], NULL, reducer, (void *)bfrptr)!=0){
                                //          printf("pthread_create");
                                //          exit(1);
                                //  }



                                //push data into buffer

                                if (sem_wait(&(bfrptr->sem)) == -1)       //acquire buffer lock
                                    errExit("sem_wait");            

                                while(bfrptr->count == BUFFER_SIZE){
                                    if (sem_post(&(bfrptr->sem)) == -1)               // release buffer lock
                                        errExit("sem_post");
                                    if (sem_wait(&(bfrptr->full)) == -1)               // release buffer lock
                                        errExit("sem_post");
                                    if (sem_wait(&(bfrptr->sem)) == -1)       //acquire buffer lock
                                    errExit("sem_wait"); 
                                }
                                

                                 tplptr=bfrptr->tup;
                                 tupcpy(&tplptr[bfrptr->in],tup);
                                 bfrptr->in = (bfrptr->in+1)%BUFFER_SIZE;
                                 bfrptr->count++;
                                
                                if (sem_post(&(bfrptr->sem)) == -1)               // release buffer lock
                                    errExit("sem_post");
                                if (sem_post(&(bfrptr->empty)) == -1)       //acquire buffer lock
                                    errExit("sem_wait"); 




                        }
                        else{
                                //push data into kth buffer for the kth thread to handle it.

                                bfrptr = ptrbfptr[k];
                                if (sem_wait(&(bfrptr->sem)) == -1)       //acquire buffer lock
                                    errExit("sem_wait"); 

                                while(bfrptr->count == BUFFER_SIZE){
                                    if (sem_post(&(bfrptr->sem)) == -1)               // release buffer lock
                                        errExit("sem_post");
                                    if (sem_wait(&(bfrptr->full)) == -1)               // release buffer lock
                                        errExit("sem_post");
                                    if (sem_wait(&(bfrptr->sem)) == -1)       //acquire buffer lock
                                    errExit("sem_wait"); 
                                }

                                tplptr=bfrptr->tup;
                                tupcpy(&tplptr[bfrptr->in],tup);
                                bfrptr->in = (bfrptr->in+1)%BUFFER_SIZE;
                                bfrptr->count++;

                                if (sem_post(&(bfrptr->sem)) == -1)               // release buffer lock
                                        errExit("sem_post");
                                if (sem_post(&(bfrptr->empty)) == -1)       //acquire buffer lock
                                     errExit("sem_wait"); 

                        }
                }
        }
       
        *done = 1;
       
        //wake up all reducer threads
        for(int i=0;i<threadCount;i++){
            bfrptr = ptrbfptr[i];
            if (sem_post(&(bfrptr->empty)) == -1)       //acquire buffer lock
                errExit("sem_wait"); 
    
        //         s = pthread_cond_signal(&(bfrptr->cond));
        //         // s = pthread_join(tid[i], NULL);
        //         // printf("thread joined %d\n", i);
         }

        // pthread_exit(NULL);

        printf("total reducer count=%d\n", threadCount);
}




// Main function
int main(int argc, char const *argv[])
{

    if (argc<2){
            printf("Error: Not enough arguments\n");
            exit(1);                
    }
    BUFFER_SIZE = atoi(argv[1]);
    done = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *done = 0;
	if(BUFFER_SIZE==0){
		printf("Error: Buffer Size 0!");
		exit(1);
	}
        FILE *fp;
        fp = fopen("rules.txt","r");

        char buff[10];
        char* token;

        struct node* rules=NULL;
        struct node* start=NULL;

        //loop to load the rules ie P=50
        while (1){
                if(!fgets(buff,10,fp))
                        break;

                struct node* temp = (struct node* ) malloc(sizeof(struct node));

                token=strtok(buff,"=");

                temp->c = token[0];

                temp->val = atoi(strtok(NULL,"\0"));
                temp->next = NULL;

                if(!start){
                        start = temp;
                        rules = start;
                }
                else {
                        rules->next = temp;
                        rules = rules->next;
                }

        }
        fclose(fp);
        rules = start;

        // pthread_t tid;
        mapper(rules);
        // int s = pthread_create(&tid, NULL, mapper, (void *)rules);


     
        // if (s != 0){
  
        //     printf("Error creating thread");
        //     fflush(stdout);
        //     exit(1);
        // }


        // pthread_exit(NULL);        
        return 0;
}
