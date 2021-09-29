// Add Guard to header file
// Function prototypes for
//			Master process's reporting procedure

#ifndef PHASE_4_H
#define PHASE_4_H
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

void reading_file(char *fname);

void Final(const char *curr_dir);

#endif
