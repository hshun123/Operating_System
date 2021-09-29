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
#include <semaphore.h>
#include <ctype.h>
// pthread.h included in header.h

// implement shared queue, final histogram here..

//lock for global variable
//pthread_mutex_t lock;

// utilis
int count[26]; //global int array
char lines[10000][1024];
int linecount;

char letters[] =
{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z' };

char upper_letters[]=
{'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z'};

//This function make a new node contains data and line number passing
struct QNode* newNode(char* k, int lineno)
{
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->data = k;
    temp -> linenum = lineno;
    temp->next = NULL;
    return temp;
};

//This function create a Queue
struct Queue* createQueue()
{
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->head = q->tail = NULL;
    return q;
};

// This function add the information k, lineno to the given Queue q
void enQueue(struct Queue* q, char* k, int lineno)
{
    // Create a new LL node
    struct QNode* temp = newNode(k, lineno);

    // If queue is empty
    if (q->tail == NULL) {
        q->head = q->tail = temp;
        q->nodecount++;
        return;
    }

    // Add the new node at the end of queue
    q->tail->next = temp;
    q->tail = temp;
    q->nodecount++; // increase the node count
}

// Function to remove a line from given queue q
void deQueue(struct Queue* q)
{
    // If queue is empty, return NULL.
    if (q->head == NULL){
        //  q-> nodecount--;
          return;
    }

    q->head = q->head->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->head == NULL)
        q->tail = NULL;

    q-> nodecount--; // decrease the node count
}
