#include "phase4.h"

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
/* Final Result
	1)	The master process reads the "ReducerResult.txt"
	2) 	Print the contents to standard output which is redirected to "FinalResult.txt"
			i.e., on using printf, the statement should be written to "FinalResult.txt"
*/

//pathlines is a globla variable that stores each line of ReducerResult.txt
char pathlines[1000][1000];

//Parse the file and store each line in to path
//From project 1
void reading_file(char *fname)
{
	FILE* fp = fopen(fname, "r");
	char line[100];
	int i = 0;


	if (!fp) {
		printf("In the reading_file Failed to open the file: %s \n", fname);
	}

	//Read the contents and store in lines
	while (fgets(line, 100, fp)){
		strncpy(pathlines[i++], line, strlen(line) + 1);
    }
    //eric
	fclose(fp);
 	}


//This function read the results from ReducerResult file
// and report it to FinalResult.txt using file redirection
void Final(const char *curr_dir){
  char gpath[1000] = {'\0'};
  char rpath[1000] = {'\0'};
  int report[1000];
  int*final = (int*)malloc(26* sizeof(int));


  strcpy(gpath, curr_dir);
  strcat(gpath, "/FinalResult.txt");

  strcpy(rpath, curr_dir);
  strcat(rpath, "/ReducerResult.txt");

	//fd_of_reduce is the file descripter of FinalResult.txt file
  int fd_of_reduce = open(gpath,O_CREAT | O_WRONLY, 0755);
	//int fd_of_reduce = open("/FinalResult.txt",O_CREAT | O_WRONLY, 0755);

	//read lines from ReducerResult.txt
  reading_file(rpath);
	//reading_file("/ReducerResult.txt");
  //using dup2 to redirect standard output to file
  dup2 (fd_of_reduce, 1);

  //reporting here standard output
  for(int i=0; i < 26; i ++){
    printf("%s", pathlines[i]);
  }
}
