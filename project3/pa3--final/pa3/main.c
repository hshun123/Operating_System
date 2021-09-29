/*test machine: csel-keller4250-11
* date: 11/12/19
* name: Eric Hwang, [Mihoko Kawada]
* x500: hwang241, [kawad003]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "header.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <semaphore.h>

// pthread.h included in header.h
int main(int argc, char *argv[]){

 // argument count check
 if (argc < 4 || argc > 5) {
		 printf("Wrong number of args, expected at least 4 and at most 5, given %d\n", argc - 1);
	 exit(1);
 }


   //-p case
	 if( !strncmp(argv[3], "-p", 2) && atoi(argv[1]) > 0 && argc == 4 ){

     //setup
     int m = atoi(argv[1]);

  	 FILE* f = fopen("log.txt", "a");
  	 FILE* final = fopen("result.txt", "w");

  	 pthread_t producer;
  	 pthread_t consumer[m];

  	 struct condQueue* cq = (struct condQueue*) malloc(sizeof(struct condQueue));
  	 cq-> q = createQueue(); // create the shared queue
  	 cq->cid	= 0;
  	 cq->q->nodecount = 0;
  	 cq->fn = argv[2];
  	 cq->fp = f;
  	 cq->eof = 0;
  	 cq-> ept = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  	 cq->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
     //initialize the condition variables and lock
  	 pthread_cond_init(cq->ept, NULL);
  	 pthread_mutex_init(cq->mutex, NULL);

				//Launch producer
				pthread_create(&producer, NULL, (void*) Producer, (void*) cq);
				// Launch consumers.
				for (int i = 0; i < m; ++i) {
					//updating the cosumer id
					cq -> cid = i;
					 //start m consumer threads
					pthread_create(&consumer[i], NULL, (void*) Consumer, (void*) cq);
				}
				//Joining back
				 pthread_join(producer, NULL);
				 for(int i = 0; i < m; i++){
					 pthread_join(consumer[i], NULL);
				 }

         for(int k = 0; k <26; k++){
           fprintf (final, "%c: %d\n",letters[k], count[k]);
         }

    //-bp case
	 }else if(!strncmp(argv[3], "-bp", 3) && atoi(argv[1]) > 0 && argc == 5){

     //setup
     int m = atoi(argv[1]);

  	 FILE* f = fopen("log.txt", "a");
  	 FILE* final = fopen("result.txt", "w");

  	 pthread_t producer;
  	 pthread_t consumer[m];

  	 struct condQueue* cq = (struct condQueue*) malloc(sizeof(struct condQueue));
  	 cq-> q = createQueue(); // create the shared queue
  	 cq->cid	= 0;
  	 cq->q->nodecount = 0;
  	 cq->fn = argv[2];
  	 cq->fp = f;
  	 cq->eof = 0;
     //extra credit
  	 cq->bound = atoi(argv[4]);

  	 cq-> ept = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
     //
     cq-> full = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  	 cq->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));

     //initialize the condition variables and lock
     pthread_cond_init(cq->ept, NULL);
     pthread_cond_init(cq->full, NULL);
  	 pthread_mutex_init(cq->mutex, NULL);

		 //Launch producer
		 pthread_create(&producer, NULL, (void*) boundedProducer, (void*) cq);
		 // Launch consumers.
		 for (int i = 0; i < m; ++i) {
			 //updating the cosumer id
			 cq -> cid = i;
				//start m consumer threads
			 pthread_create(&consumer[i], NULL, (void*) boundedConsumer, (void*) cq);
		 }
		 //Joining back
			pthread_join(producer, NULL);
			for(int i = 0; i < m; i++){
				pthread_join(consumer[i], NULL);
			}
      for(int k = 0; k <26; k++){
        fprintf (final, "%c: %d\n",letters[k], count[k]);
      }
      //-b case
	 }else if(!strncmp(argv[3], "-b", 2) && atoi(argv[1]) > 0 && argc == 5){

     //setup
     int m = atoi(argv[1]);

  	 FILE* f = fopen("log.txt", "a");
  	 FILE* final = fopen("result.txt", "w");

  	 pthread_t producer;
  	 pthread_t consumer[m];

  	 struct condQueue* cq = (struct condQueue*) malloc(sizeof(struct condQueue));
  	 cq-> q = createQueue(); // create the shared queue
  	 cq->cid	= 0;
  	 cq->q->nodecount = 0;
  	 cq->fn = argv[2];
  	 cq->fp = f;
  	 cq->eof = 0;
     //extra credit
  	 cq->bound = atoi(argv[4]);
  	 cq-> ept = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));

     cq-> full = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  	 cq->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));

     //initialize the condition variables and lock
  	 pthread_cond_init(cq->ept, NULL);
     pthread_cond_init(cq->full, NULL);
  	 pthread_mutex_init(cq->mutex, NULL);

     //Launch producer
		 pthread_create(&producer, NULL, (void*) boundedProducer, (void*) cq);
		 // Launch consumers.
		 for (int i = 0; i < m; ++i) {
			 //updating the cosumer id
			 cq -> cid = i;
				//start m consumer threads
			 pthread_create(&consumer[i], NULL, (void*) boundedConsumer, (void*) cq);
		 }
		 //Joining back
			pthread_join(producer, NULL);
			for(int i = 0; i < m; i++){
				pthread_join(consumer[i], NULL);
			}
		 remove("log.txt");

     for(int k = 0; k <26; k++){
       fprintf (final, "%c: %d\n",letters[k], count[k]);
     }

	 }else{
		 printf("wrong argument\n"); //handling the wrong input
	 }

 return 0;
}
