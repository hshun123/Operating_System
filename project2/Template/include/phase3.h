// Add Guard to header file
// Function prototype for 
//			reduce function that will receive list from all the Mappers to create a single list
//			the single list is then written to ReducerResult.txt

#ifndef PHASE_3_H
#define PHASE_3_H

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

void Reduce (const char *curr_dir, int* list);


#endif
