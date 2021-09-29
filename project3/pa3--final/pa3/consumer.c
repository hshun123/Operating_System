/*test machine: csel-keller4250-11
* date: 11/12/19
* name: Eric Hwang, [Mihoko Kawada]
* x500: hwang241, [kawad003]
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

//#include "utils.c"
#include <errno.h>
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
// pthread.h included in header.h

//parsing each line with this function and return the linked-list unbounded buffer number of tokens for each line
int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
			printf("Fail to parse because of NULL");
      return -1;
   }
   *argvp = NULL;
   snew = s + strspn(s, delimiters);         /* snew is real start of string */
   if ((t = malloc(strlen(snew) + 1)) == NULL)
      return -1;
   strcpy(t, snew);
   numtokens = 0;
   if (strtok(t, delimiters) != NULL)     /* count the number of tokens in s */
      for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++) ;

                             /* create argument array for ptrs to the tokens */
   if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
      error = errno;
      free(t);
      errno = error;
      return -1;
   }
                        /* insert pointers to tokens into the argument array */
   if (numtokens == 0)
      free(t);
   else {
      strcpy(t, snew);
      **argvp = strtok(t, delimiters);
      for (i = 1; i < numtokens; i++)
          *((*argvp) + i) = strtok(NULL, delimiters);
    }
    *((*argvp) + numtokens) = NULL;             /* put in final NULL pointer */
    return numtokens;
}

//count each word of first letter
void countalpha (char* li){
  char **myargv;
  char delim[] = " ";
  int numtokens;

  if (! ((numtokens = makeargv(li, delim, &myargv)) == -1)){
    for(int i = 0; i < numtokens; i++){
      //check the first letter in the word
      for(int j = 0; j < 26; j++){
          if((myargv[i][0] == letters[j]) || (myargv[i][0] == upper_letters[j])){
              count[j]++;
          }
      }//for j
    }//for i
  }//if
}

//Consumer function
//THis function repeatedly check the queue for a new package, if there
//are any packages it works on counting the words. It synchronizes the result.
void Consumer (void* arg){
   struct condQueue* cq = (struct condQueue*) arg;
   pthread_mutex_lock(cq->mutex);
   //checks if it's EOF
   while(cq->eof == 0){
     if(cq->q->nodecount == 0){
         pthread_cond_wait(cq->ept, cq->mutex);
     }else{
        fprintf (cq->fp, "consumer %d\n", cq -> cid);
        fprintf (cq->fp, "consumer %d: %d\n", cq-> cid, cq-> q -> head -> linenum );
        countalpha(cq -> q -> head ->data);
        deQueue(cq->q);
     }
   }//if there's data in queue when it's EOF
   while(cq->q->nodecount > 0){
       fprintf (cq->fp, "consumer %d\n", cq -> cid);
       fprintf (cq->fp, "consumer %d: %d\n", cq-> cid, cq-> q -> head -> linenum );
       countalpha(cq -> q -> head ->data);
       deQueue(cq->q);
   }
  pthread_mutex_unlock(cq->mutex);
}

//extra credit
//THis function repeatedly check the queue for a new package, if there
//are any packages it works on counting the words. It synchronizes the result.

void boundedConsumer (void* arg){
  struct condQueue* cq = (struct condQueue*) arg;
  pthread_mutex_lock(cq->mutex);
  //checks if it's EOF
  while(cq -> eof == 0){
    if(cq -> q -> nodecount == 0){
        pthread_cond_wait(cq->ept, cq->mutex);
    }else{
       fprintf (cq -> fp, "consumer %d\n", cq -> cid);
       fprintf (cq -> fp, "consumer %d: %d\n", cq-> cid, cq-> q -> head -> linenum );
       countalpha(cq -> q -> head ->data);
       deQueue(cq -> q);
    }
  }//if there's data in queue when it's EOF
  while(cq -> q -> nodecount > 0){
      fprintf (cq -> fp, "consumer %d\n", cq -> cid);
      fprintf (cq -> fp, "consumer %d: %d\n", cq-> cid, cq-> q -> head -> linenum );
      countalpha(cq -> q -> head ->data);
      deQueue(cq -> q);
  }
  pthread_mutex_unlock(cq -> mutex);
}
