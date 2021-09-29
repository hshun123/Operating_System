// Add Guard to header file
// Function prototypes for
//			map function that will go through text files in the Mapper_i.txt to get the (letter, wordcount)

#ifndef PHASE_2_H
#define PHASE_2_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <zconf.h>
#include <sys/types.h>

void trimNewline(char * str);
char** process_file(char *fname);
void countletters(char* txtpath);
int* sumcounts(int* nat, int* ad);
int* Map (char* filepath);

#endif
