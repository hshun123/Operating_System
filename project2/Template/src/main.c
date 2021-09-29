#include "main.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	// argument count check

	if (argc < 3) {

		printf("Wrong number of args, expected 2, given %d", argc - 1);
		exit(1);
	}

	//cwd contains the current directory path
	char cwd[1000];
	if(getcwd(cwd, sizeof(cwd)) != NULL) {

	}else{
		perror("getcwd() error");
		return -1;
	}


	//m is the # of Mapper
	int m = atoi(argv[2]);

	//just make a function call to code in phase1.c
	//phase1 - Data Partition Phase
	int empty = traverse(argv[1]);
	if (empty == 0){
		printf("The folder is Empty\n");
		return 0;
	}

	//process(argv[1], m);
	process(cwd, m);

	//create pipes
	int  fds[2*m];

	for(int i = 0; i < m; i++){
		pipe(fds + (2*i));
	}

	//just make a function call to code in phase2.c
	//phase2 - Map Function

	pid_t cpid;
	char buffer[100]={'\0'};
	int buf[1000];
	int nbytes;
	int* result[26];
	int*finalcount = (int*)malloc(26* sizeof(int));

	//Initializing finalcount
	for(int i=0; i < 26; i++){
		result[i] = 0;
		finalcount[i] = 0;
	}

	//Master forks Mapper
	for(int j = 0; j < m; j++){

		if ((cpid = fork()) == -1) {
        perror("fork");
        return 1;
    }

		if(cpid > 0 ){
			wait(NULL);
		}
		else{

			char* tpath = (char*)malloc(1000* sizeof(char));
			strcpy(tpath, argv[1]);
			strcpy(tpath, cwd);

			strcat(tpath, "\0");

			snprintf(buffer, 100, "%s/MapperInput/Mapper_%d.txt",tpath, j);

			//free(tpath);
			//close read side of pipe
			close(fds [2 * j]);

			//printf("In the main: %s\n", buffer);

			//store the list for each Mapper_i.txt to data array
			int*data = malloc(26*sizeof(int) + 1);
			data = Map(buffer);

			//write to the pipe
			//write(fds[j][1], data, sizeof(data));
			write(fds[(2*j) + 1], data, 26 * sizeof(int));

			//close the write side of the pipe
			close(fds[(2*j) +1]);
			exit(0);
		}
	}


	//fork reducer here
	if ((cpid = fork()) == -1) {
			perror("fork");
			return 1;
	}

	if(cpid > 0 ){
		wait(NULL);
	}
	else{
		for(int k = 0; k < m; k++){

				close(fds[(2 * k) +1]);
				nbytes = read(fds[(2*k)], buf, 26 * sizeof(int) + 1);
				finalcount = sumcounts(finalcount, buf);

				close(fds[(2 * k)]);
		}

		//just make a function call to code in phase3.c
		//phase3 - Reduce Function

		Reduce(cwd, finalcount);
		exit(0); //exit
	}

	//phase4
	//wait for all processes to reach this point
	//just make a function call to code in phase4.c
	//master process reports the final output

	Final(cwd);

	return 0;

}
