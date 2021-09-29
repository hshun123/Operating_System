#ifndef __DFS_STACK_H
#define __DFS_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

//Implement a stack for DFS
typedef struct stack_for_dfs{
  int trg;
  struct stack_for_dfs* link;
}stack_for_dfs;

stack_for_dfs* top;

int isEmpty(){
  if(top == NULL) return 1;
  else return 0;
}

void push (int g){
  stack_for_dfs* temp = (stack_for_dfs*)malloc(sizeof(stack_for_dfs));
  temp -> trg = g;
  temp -> link = top;
  top = temp;
}

int pop(){

  int item;
  stack_for_dfs* temp = top;

  if(isEmpty()){
    printf("\n\n Stack is empty!\n");
    return item; // Be careful
  }
  else{
    item = temp -> trg;
    top = temp -> link;
    free(temp);
    return item;
  }
}

#endif
