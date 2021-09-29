/*test machine: csel-keller4250-06
* date: 12/6/2019
* name: Eric Hwang, [Mihoko Kawada]
* x500: hwang241, [kawad003]
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <zconf.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include "../include/protocol.h"
//#include "phase1.c"

FILE *logfp;
int ok;
//Alphabet arrays
char letters[26] = {'a','b','c','d','e','f','g',
'h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

char upper_letters[26] = {'A','B','C','D','E','F','G',
'H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

//This funciton trims the newLine \n
void trimNewline(char * str)
{
    int index, i;

    index = -1;

    i = 0;
    while(str[i] != '\0')
    {
        if(str[i] != '\n')
        {
            index= i;
        }

        i++;
    }
    str[index + 1] = '\0';
}

//Parse the file and store each line in to path
//From project 1
char** process_file(char *fname)
{
	FILE* fp = fopen(fname, "r");

	char line[1024];
	int i = 0;
  char **pathlines;

  //dynamically allocating multi array
    pathlines = malloc (1024 * sizeof(char*));
    for(int j = 0; j < 1024; j++){
       pathlines[j] = malloc(1024 * sizeof(char));
    }
	if (!fp) {
		printf("Failed to open the file: **************%s*********** \n", fname);
    printf("errono: %d\n", errno);
    printf("Error: %s\n", strerror(errno));
		return NULL;
	}

	//Read the contents and store in lines
	while (fgets(line, 1024, fp)){
		strncpy(pathlines[i++], line, strlen(line) + 1);
    }
    //eric
    ok = i;
	fclose(fp);

	return pathlines;
}

//This function count the word per letter
int* countletters(char* txtpath, int* test){

    char**check = process_file(txtpath);

    //number of words in the txt file
    int num_of_words = ok;

    for(int i = 0; i < num_of_words; i++){
        //trim the line with \n
        trimNewline(check[i]);
        //eric
        //check the first letter in the word
        for(int j = 0; j < 28; j++){
            if((check[i][0] == letters[j]) || (check[i][0] == upper_letters[j])){
                test[j + 2]++;
            }
        }
    }
    free(check);
    return test;
}

//This is the Map function which returns the list for each alphbet count
//each index in totalcount is corresponding to a-z alphabet
// sets the first and second element to 0 here and modify later in the main
int* Map (char* path, int* array) {

    int* wordcount = (int*)malloc(28* sizeof(int));
    for (int i = 0; i < 28; i++)
      wordcount[i] = 0;

      //trim newline \n
      trimNewline(path);

      countletters(path, array);

    return wordcount;

}

void createLogFile(void) {
    pid_t p = fork();
    if (p==0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", ACCESSPERMS);
    logfp = fopen("log/log_client.txt", "w");
}

int main(int argc, char *argv[]) {
    int mappers;
    char folderName[100] = {'\0'};
    char *server_ip;
    int server_port;

    if (argc == 5) { // 4 arguments
        strcpy(folderName, argv[1]);
        mappers = atoi(argv[2]);
        server_ip = argv[3];
        server_port = atoi(argv[4]);
        if (mappers > MAX_MAPPER_PER_MASTER) {
            printf("Maximum number of mappers is %d.\n", MAX_MAPPER_PER_MASTER);
            printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
            exit(1);
        }

        if(mappers <= 0){
          printf("Minimum number of mappers is 1.\n");
          printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
          exit(1);
        }

    } else {
        printf("Invalid or less number of arguments provided\n");
        printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
        exit(1);
    }

    // create log file
    createLogFile();

    // phase1 - File Path Partitioning
    traverseFS(mappers, folderName);

    // Phase2 - Mapper Clients's Deterministic Request Handling

    //Eric
    pid_t cpid;
    char buffer[100]={'\0'};

    //Master forks Mapper
    for(int j = 0; j < mappers; j++){

      if ((cpid = fork()) == -1) {
          perror("fork");
          return 1;
      }

      if(cpid == 0){

        //cwd contains the current directory path
      	char cwd[1000];
      	if(getcwd(cwd, sizeof(cwd)) != NULL) {

      	}else{
      		perror("getcwd() error");
      		return -1;
      	}

        int mapper_id = j + 1; // mapper id

        //store the path of Mapper_j.txt to buffer
        char* tpath = (char*)malloc(1000* sizeof(char));
        strcpy(tpath, argv[1]);
        strcpy(tpath, cwd);

        strcat(tpath, "\0");

        snprintf(buffer, 100, "%s/MapperInput/Mapper_%d.txt",tpath, j +1);

        free(tpath);

        //--------------------------------------------TCP CONNECT-------------------------------
        // Create a TCP socket.
    		int sockfd = socket(AF_INET , SOCK_STREAM , 0);

    		// Specify an address to connect to (we use the local host or 'loop-back' address).
    		struct sockaddr_in address;
    		address.sin_family = AF_INET;
    		address.sin_port = htons(server_port);
    		address.sin_addr.s_addr = inet_addr(server_ip);

    		// Connect it.
    		if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {

          //log printout for TCP connection
					printf("[%d] open connection\n", mapper_id);
          fprintf(logfp, "[%d] open connection\n", mapper_id);

      		int request[REQUEST_MSG_SIZE]; // 28
      		int response1[RESPONSE_MSG_SIZE]; // 1 1 1
          int response2[LONG_RESPONSE_MSG_SIZE]; //1 1 26

          //initialize integer arrays
          for(int i = 0; i < REQUEST_MSG_SIZE; i++){
            request[i] = 0;
          }


          //CHECKIN
          request[0] = 1;
          request[1] = mapper_id;
          write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);

          read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

          //Eric for test
          printf("[%d] CHECKIN: %d %d\n",mapper_id, response1[1], response1[2]);

          fprintf(logfp,"[%d] CHECKIN: %d %d\n",mapper_id, response1[1], response1[2]);

          //UPDATE_AZLIST

          //process Mapper_j.txt file
          char** paths = process_file(buffer);
          //num_of_txt is the total number of txt files inside each Mapper_j.txt
          int num_of_txt = ok;

          //loop through all the paths in the Mapper_j.txt
					//printf("%d\n", num_of_txt);
          for(int i = 0; i < num_of_txt; i++){

            //initialize integer arrays
            for(int i = 0; i < REQUEST_MSG_SIZE; i++){
              request[i] = 0;
            }

              //store the word count result of each file to integer array
              Map(paths[i], request);
              //set the request command and mapper ID
              request[0] = 2;
              request[1] = mapper_id;

              write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);

              read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);
          }


          //Eric for test
          //This is printing out on terminal
          printf("[%d] UPDATE_AZLIST: %d\n", mapper_id, num_of_txt);

          fprintf(logfp, "[%d] UPDATE_AZLIST: %d\n", mapper_id, num_of_txt);

          free(paths);

          //GET_AZLIST
          request[0] = 3;
          request[1] = mapper_id;

          write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);

          read(sockfd, response2, sizeof(int) * LONG_RESPONSE_MSG_SIZE);

          //Eric for tests
          printf("[%d] GET_AZLIST: %d ", mapper_id, response2[1]);
          for(int i = 2; i < 28; i++){
            printf("%d ",response2[i]);
          }

          fprintf(logfp,"[%d] GET_AZLIST: %d ", mapper_id, response2[1]);
          for(int i = 2; i < 28; i++){
            fprintf(logfp, "%d ",response2[i]);
          }


          //GET_MAPPER_UPDATES
          request[0] = 4;
          request[1] = mapper_id;

          write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE );

          read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

          //Eric for tests
          printf("\n[%d] GET_MAPPER_UPDATES: %d %d\n", mapper_id, response1[1], response1[2]);

          fprintf(logfp, "\n[%d] GET_MAPPER_UPDATES: %d %d\n", mapper_id, response1[1], response1[2]);


          //GET_ALL_UPDATES
          request[0] = 5;
          request[1] = mapper_id;

          write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);

          read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

          //Eric for test
          printf("[%d] GET_ALL_UPDATES: %d %d\n", mapper_id, response1[1], response1[2]);

          fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", mapper_id, response1[1], response1[2]);


          //CHECKOUT
          request[0] = 6;
          request[1] = mapper_id;

          write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);

          read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

          //Eric for tests
          printf("[%d] CHECKOUT: %d %d\n", mapper_id, response1[1],response1[2]);

          fprintf(logfp, "[%d] CHECKOUT: %d %d\n", mapper_id, response1[1],response1[2]);

          //close connection
          close(sockfd);

          //Eric for test
          printf("[%d] close connecton\n", mapper_id);

          fprintf(logfp, "[%d] close connecton\n", mapper_id);

        } else {
          perror("Connection failed!");
        }
        //--------------------------------------------TCP CONNECT-------------------------------
        //exit
        exit(0);

      }//mapper body
    }//end of fork

    pid_t wpid;
    int status = 0;
    //Master wait for children to finish
    //while((wpid == wait(&status)) > 0);

    //Master wait for all the child process to finish
    for(int t = 0; t < mappers; t ++){
        wait(&status);
    }

    // Phase3 - Master Client's Dynamic Request Handling (Extra Credit)
    FILE* file = fopen("commands.txt", "r");
    int i = 0;
    int extra_id =-1;

    //set up for integer arrays
    int request[REQUEST_MSG_SIZE]; // 28
    int response1[RESPONSE_MSG_SIZE]; // 1 1 1
    int response2[LONG_RESPONSE_MSG_SIZE]; //1 1 26

    //initialize integer arrays
    for(int i = 0; i < REQUEST_MSG_SIZE; i++){
      request[i] = 0;
    }

    //while the file reaches the eof
    while (!feof(file)){

      // Create a TCP socket.
      int sockfd = socket(AF_INET , SOCK_STREAM , 0);

      // Specify an address to connect to (we use the local host or 'loop-back' address).
      struct sockaddr_in address;
      address.sin_family = AF_INET;
      address.sin_port = htons(server_port);
      address.sin_addr.s_addr = inet_addr(server_ip);

      //read the integer from the command.txt
      fscanf (file, "%d\n", &i);

      //switch case for check the command
      switch (i) {
        case CHECKIN: //1

          if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){

            printf("[%d] open connection\n", extra_id);
            fprintf(logfp, "[%d] open connection\n", extra_id);

            //CHECKIN
            request[0] = 1;
            request[1] = extra_id;
            write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);
            read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

            //Eric for test
            printf("[%d] CHECKIN: %d %d\n",extra_id, response1[1], response1[2]);
            fprintf(logfp, "[%d] CHECKIN: %d %d\n",extra_id, response1[1], response1[2]);

            //close connection
            close(sockfd);
            printf("[%d] close connection\n", extra_id);
            fprintf(logfp, "[%d] close connection\n", extra_id);

          }else{
            perror("Connection failed!");
          }


          break;

        case UPDATE_AZLIST: //2

          if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){

            printf("[%d] open connection\n", extra_id);
            fprintf(logfp, "[%d] open connection\n", extra_id);

            request[0] = 2;
            request[1] = extra_id;
            write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);
            read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

            //close connection
            close(sockfd);
            fprintf(logfp, "[%d] close connection\n", extra_id);

          }else{
            perror("Connection failed!");
          }
          break;
        case GET_AZLIST: //3
          if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){

            printf("[%d] open connection\n", extra_id);
            fprintf(logfp, "[%d] open connection\n", extra_id);

            request[0] = 3;
            request[1] = extra_id;
            write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);
            read(sockfd, response2, sizeof(int) * LONG_RESPONSE_MSG_SIZE);

            //Eric for tests
            printf("[%d] GET_AZLIST: %d ", extra_id, response2[1]);
            for(int i = 2; i < 28; i++){
              printf("%d ",response2[i]);
            }
            //log printout
            fprintf(logfp,"[%d] GET_AZLIST: %d ", extra_id, response2[1]);
            for(int i = 2; i < 28; i++){
              fprintf(logfp, "%d ",response2[i]);
            }

            //close connection
            close(sockfd);
            printf("\n[%d] close connection\n", extra_id);
            fprintf(logfp, "\n[%d] close connection\n", extra_id);

          }else{
            perror("Connection failed!");
          }
          break;

        case GET_MAPPER_UPDATES: //4

          if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){

            printf("[%d] open connection\n", extra_id);
            fprintf(logfp, "[%d] open connection\n", extra_id);

            request[0] = 4;
            request[1] = extra_id;

            write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE );

            read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

            //Eric for tests
            printf("\n[%d] GET_MAPPER_UPDATES: %d %d\n", extra_id, response1[1], response1[2]);

            fprintf(logfp, "[%d] GET_MAPPER_UPDATES: %d %d\n", extra_id, response1[1], response1[2]);

            //close connection
            close(sockfd);
            fprintf(logfp, "[%d] close connection\n", extra_id);


          }else{
              perror("Connection failed!");
          }

          break;
        case GET_ALL_UPDATES: //5

          if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){

            printf("[%d] open connection\n", extra_id);
            fprintf(logfp, "[%d] open connection\n", extra_id);
            request[0] = 5;
            request[1] = extra_id;

            write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);

            read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

            //Eric for test
            printf("[%d] GET_ALL_UPDATES: %d %d\n", extra_id, response1[1], response1[2]);

            fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", extra_id, response1[1], response1[2]);

            //close connection
            close(sockfd);
            fprintf(logfp, "[%d] close connection\n", extra_id);

          }else{
              perror("Connection failed!");
          }
          break;
        case CHECKOUT: //6
          if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0){
            printf("[%d] open connection\n", extra_id);
            fprintf(logfp, "[%d] open connection\n", extra_id);
            request[0] = 6;
            request[1] = extra_id;

            write(sockfd, request, sizeof(int) * REQUEST_MSG_SIZE);

            read(sockfd, response1, sizeof(int) * RESPONSE_MSG_SIZE);

            //Eric for tests
            printf("[%d] CHECKOUT: %d %d\n", extra_id, response1[1],response1[2]);

            fprintf(logfp, "[%d] CHECKOUT: %d %d\n", extra_id, response1[1],response1[2]);

            //close connection
            close(sockfd);
            fprintf(logfp, "[%d] close connection\n", extra_id);

          }else{
              perror("Connection failed!");
          }
          break;
        default:
          printf("wrong command\n");
          fprintf(logfp,"[%d] wrong command", extra_id);
          break;
        }
      }
      fclose (file);

    fclose(logfp);

    return 0;

}
