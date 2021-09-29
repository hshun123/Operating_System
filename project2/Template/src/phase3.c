#include "phase3.h"

#include<stdio.h>
#include <dirent.h>
#include<unistd.h>
#include<fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <zconf.h>
 #include <sys/types.h>
// You are free to use your own logic. The following points are just for getting started
/* Reduce Function
	1)	The reducer receives the list from 'm' mappers via pipes
	2)	Ensure proper closing of pipe ends
	3) 	Combine the lists to create a single list
	4) 	Write the list to "ReducerResult.txt" in the current folder
*/
//This function create the ReducerResult.txt file in current directory
//And, write the list required that is read from phase2
void Reduce (const char *curr_dir, int* list){

  char upper[26] = {'A','B','C','D','E','F','G',
  'H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

  char final[100] = {'\0'};

  char gpath[1000] = {'\0'};
  strcpy(gpath, curr_dir);
  strcat(gpath, "/ReducerResult.txt");

  FILE *fp = fopen(gpath, "w");
  //FILE *fp = fopen("/ReducerResult.txt", "w");
  if (fp == NULL) {
    printf("Error! Could not open file\n");
    exit(-1); // must include stdlib.h
  }

  for (int i = 0; i < 26; i++) {
    snprintf(final, 100, "%c %d\n", upper[i], list[i]);
    fputs(final, fp);
  }

}
