/*test machine: csel-keller4250-11
* date: 11/12/19
* name: Eric Hwang, [Mihoko Kawada]
* x500: hwang241, [kawad003]
*/
/*
header.h, header for all source files
it will:
- structure definition
- global variable, lock declaration (extern)
- function declarations
*/

#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "header.h"

#include <pthread.h>
#include <ctype.h>

// utilis
extern int count[26]; //global int array for final result
extern char lines[10000][1024]; // store each line
extern int linecount;// count the total number of lines

extern char letters[];
extern char upper_letters[];

extern pthread_mutex_t lock;
//Producer
void Producer(void* arg);

//extra credit
void boundedProducer(void* arg);

//consumer
int makeargv(const char *s, const char *delimiters, char ***argvp);
void countalpha (char* li);
void Consumer (void* arg);
void boundedConsumer (void* arg);

//QNode
// Node structure cotains data which has the information for line
struct QNode {
    char* data;
    struct QNode* next;
    int linenum;
};

// Queue structure
struct Queue {
    struct QNode *head, *tail;
    //eric
    int nodecount;
};
//condQueeue contains package
struct condQueue {
  struct Queue* q;
  int cid;
  int eof;
  int bound;
  FILE* fp;
  char* fn;
  pthread_cond_t* full;
  pthread_cond_t* ept;
  pthread_mutex_t* mutex;
};

struct QNode* newNode(char* k, int lineno);
struct Queue* createQueue();
void enQueue(struct Queue* q, char* k, int lineno);
void deQueue(struct Queue* q);

#endif
