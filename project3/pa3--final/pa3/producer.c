/*test machine: csel-keller4250-11
* date: 11/12/19
* name: Eric Hwang, [Mihoko Kawada]
* x500: hwang241, [kawad003]
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>
// pthread.h included in header.h


//Parse the file and store each line in to path
//From project 1
int process_file(char *fname){
	FILE* fp = fopen(fname, "r");

	char line[1024];
	int i = 0;

	if (!fp) {
		printf("Failed to open the file: **************%s*********** \n", fname);
    printf("Error: %s\n", strerror(errno));
	   exit(0);
	}

	//Read the contents and store in lines
	while (fgets(line, 1024, fp)){
		strncpy(lines[i++], line, strlen(line) + 1);
    }
    linecount = i;

		//replacing special characters
		for(int k = 0; k <= linecount; k++){
        for(int j = 0; j < strlen(lines[k]); j++){
            if(isalpha(lines[k][j]) == 0){
                lines[k][j] = ' ';
            }
        }
    }

	fclose(fp);
  if(linecount == 0){
     printf("Empty file!\n");
     exit(0);
  }

	return 0;
}
//Producer funciton
//This function read the input file and insert each line to the shared Queue
//one at a time. It sends the eof notification when reaches the end.
void Producer(void* arg) {

  struct condQueue* cq = (struct condQueue*) arg;
  process_file(cq->fn);

  fprintf (cq->fp, "procuder\n");
  for(int i = 0; i < linecount; i++){
      pthread_mutex_lock(cq->mutex);
      fprintf (cq->fp, "procuder: %d\n", i);

      enQueue(cq->q, lines[i], i);
      if(i == linecount - 1){
        cq->eof = 1;
        fprintf (cq->fp, "procuder: -1\n");
        pthread_cond_broadcast(cq->ept);
      }
      pthread_mutex_unlock(cq->mutex);
  }
}

//extra credit
//This function read the input file and insert each line to the shared Queue
//one at a time. It sends the eof notification when reaches the end.
void boundedProducer(void* arg) {

  struct condQueue* cq = (struct condQueue*) arg;
  process_file(cq->fn);
  int bounded = cq->bound ;
  //int i, j;
	int i = 0;

  fprintf (cq->fp, "procuder\n");

	for(int i = 0; i < linecount; i++){
			pthread_mutex_lock(cq->mutex);
			fprintf (cq->fp, "procuder: %d\n", i);

			enQueue(cq->q, lines[i], i);
			if(i == linecount - 1){
				cq->eof = 1;
				fprintf (cq->fp, "procuder: -1\n");
				pthread_cond_broadcast(cq->ept);
			}
			pthread_mutex_unlock(cq->mutex);
	}
}
