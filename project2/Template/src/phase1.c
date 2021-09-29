#include "phase1.h"
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
/* 	Data Partitioning Phase - Only Master process involved
	1) 	Create 'MapperInput' folder
	2) 	Traverse the 'Sample' folder hierarchy and insert the text file paths
		to Mapper_i.txt in a load balanced manner
	3) 	Ensure to keep track of the number of text files for empty folder condition
*/

//paths stores each txt files
char* paths [10000];
int filecount = 0; //count the number of files

//This function identify the symbolic links
//From lab exercise
void print_softlinks(const char *curr_dir, char *entry_name) {
    char this_entry[1024] = {'\0'};
    snprintf(this_entry, sizeof(this_entry), "%s/%s", curr_dir, entry_name);

    struct stat entry_stat;
    //lstat is used for symbolic
    if (lstat(this_entry, &entry_stat) == -1) {
        perror("stat error");
        exit(EXIT_FAILURE);
    }
    printf("\t**** SYMBOLIC LINK ****\n");
    printf("\tI-node number:            %ld\n", (long) entry_stat.st_ino);
    printf("\tLink count:               %ld\n", (long) entry_stat.st_nlink);
    printf("\tOwnership:                UID=%ld   GID=%ld\n", (long) entry_stat.st_uid, (long) entry_stat.st_gid);
    printf("\tFile size:                %lld bytes\n", (long long) entry_stat.st_size);
}

//your_recursive_func traverse every directory and store each txt file into paths
int traverse(const char *curr_dir) {
    DIR *dir;
    struct dirent *dir_entry;


    if (!(dir = opendir(curr_dir)))
        return -1;

    if (readdir(dir) == NULL){
      printf("Not exist\n");
    }

    while ((dir_entry = readdir(dir)) != NULL) {

        if (dir_entry->d_type == DT_DIR) {

            if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) {
                continue;
            }

            char next_path[1024] = {'\0'};
            snprintf(next_path, sizeof(next_path), "%s/%s", curr_dir, dir_entry->d_name);

            traverse(next_path);

        } else if (dir_entry->d_type == DT_REG) {

          char* dpath = (char*)malloc(1000* sizeof(char));


          strcpy(dpath, curr_dir);
          strcat(dpath, "/");
          strcat(dpath, dir_entry -> d_name);
          strcat(dpath, "\n");
          strcat(dpath, "\0");

          //printf("dpath: %ld with %s\n", strlen(dpath), dpath);
          paths [filecount] = dpath;

          filecount++;

        } else if (dir_entry->d_type == DT_LNK) {
          printf("%s%s\n", "Name:", dir_entry->d_name);

          print_softlinks(curr_dir, dir_entry->d_name);

        } else {

        }
    }
    closedir(dir);
    return filecount;
}

//This funciton create the MapperInput directory
// And store files in to each Mapper_i.txt
void process (const char *curr_dir, int m) {

  int fd[m];

  char gpath[1000] = {'\0'};
  strcpy(gpath, curr_dir);
  strcat(gpath, "/MapperInput");
  //printf("cur: %s\n", curr_dir);
  mkdir(gpath, ACCESSPERMS);
  //mkdir("MapperInput", ACCESSPERMS);

  for(int i = 0; i < m; i ++){
    char buf[100];
    char* tpath = (char*)malloc(1000* sizeof(char));
    strcpy(tpath, gpath);
    strcat(tpath, "\0");

    snprintf(buf, 100, "%s/Mapper_%d.txt", tpath, i);

    fd[i] = open(buf, O_CREAT | O_WRONLY, 0766);

    free(tpath);
}

// This loop stores txt files into Mapper_i.txt using round robin
for(int j = 0; j < filecount; j++){

  size_t s = write(fd[j%m], paths[j], strlen(paths[j]));

}

  for (unsigned i = 0; i < m; i++)
    close(fd[i]);

}
