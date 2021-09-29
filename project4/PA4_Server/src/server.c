/*test machine: csel-keller4250-06
* date: 12/6/2019
* name: Eric Hwang, [Mihoko Kawada]
* x500: hwang241, [kawad003]
*/

#define _DEFAULT_SOURCE
#define NUM_ARGS 0

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include "../include/protocol.h"


#define MAX_CONCURRENT_CLIENTS 50

//global arrays and variables
int azList[26];
int updateStatus[MAX_MAPPER_PER_MASTER + 1][3];
int response[RESPONSE_MSG_SIZE];
int resAndArray[LONG_RESPONSE_MSG_SIZE];
int currentConn = 0;
pthread_mutex_t currentConn_lock;
pthread_mutex_t azList_lock;

//structure for threadArg
struct threadArg {
	int clientfd;
	char * clientip;
	int clientport;
};

//Function used in pthread_create
void * threadFunction(void * arg) {
	struct threadArg * tArg = (struct threadArg *) arg;
	int readbuf[REQUEST_MSG_SIZE];

	pthread_mutex_init(&azList_lock, NULL);
	int flag = RSP_OK;

  while(flag == RSP_OK){

		 read(tArg->clientfd, readbuf, sizeof(int)*LONG_RESPONSE_MSG_SIZE);
		 int command = readbuf[0];
	 	 int mapperID = readbuf[1];

		 //mapper ID has to be bigger than 0 or -1 for extra credit
		 if(mapperID==0 || mapperID < -1){
			 response[0]= command;
			 response[1]= RSP_NOK;
			 response[2]= mapperID;
			 write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
			 printf("wrong mapperID %d\n", mapperID);
			 flag = RSP_NOK; //break & close
			 break;

		 }else{ //in while
			 //switch case to check the request command
			 switch(command){
	 			case CHECKIN:
					if(mapperID > 0){
						if(updateStatus[mapperID][0] == mapperID ){//checks if mapperId exists
							if(updateStatus[mapperID][2] == 0){//check if the mapperID has beed checkedIn
								printf("[%d] CHECKIN\n", mapperID);
								updateStatus[mapperID][2]= 1; //update check-in to 1
								response[0]= command;
								response[1]= RSP_OK;
								response[2]= mapperID;
								write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
							}else{ //error handling
								response[0]= command;
								response[1]= RSP_NOK;
								response[2]= mapperID;
								write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE); //error
								printf("CHECKIN: mapperID %d has already checked in\n", mapperID );
							}
						}else{//mapperID doesn't exist yet
							printf("[%d] CHECKIN\n", mapperID);
							updateStatus[mapperID][0] = mapperID;
							updateStatus[mapperID][2]= 1;
							response[0]= command;
							response[1]= RSP_OK;
							response[2]= mapperID;
							write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						}
					}else{ // extra credit
						response[0]= command;
						response[1]= RSP_NOK;
						response[2]= mapperID;
						write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						printf("[%d] CHECKIN\nMapper PID(%d) should be greater than 0\n", mapperID, mapperID);
						flag = RSP_NOK; //break & close
					}
	 				break;

	 			case UPDATE_AZLIST:

					if(mapperID > 0){
						//error handling
						//client needs to be checked in
		 				if(updateStatus[mapperID][0]!= mapperID || updateStatus[mapperID][2]== 0){
		 					response[0]= command;
		 					response[1]= RSP_NOK;
		 					response[2]= mapperID;
		 					write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
							printf("UPDATE_AZLIST: mapperID %d hasn't yet checked in\n", mapperID );
		 				}else{
							//adding 1 to the updateStatus
			 				updateStatus[mapperID][1] +=1;
			 				//mutex lock
			 				pthread_mutex_lock(&azList_lock);
			 				for(int i =0; i< ALPHABETSIZE; i++){
			 					azList[i] += readbuf[i+2];//adding the received array to azList
			 				}
			 				response[0]= command;
			 				response[1]= RSP_OK;
			 				response[2]= mapperID;
			 				write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
			 			  //mutex unlock
			 				pthread_mutex_unlock(&azList_lock);
						}

					}else{// extra credit
						response[0]= command;
						response[1]= RSP_NOK;
						response[2]= mapperID;
						write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						flag = RSP_NOK; //break & cl
					}
	 				break;

	 			case GET_AZLIST://+extra credit

					if(mapperID >0){
						//error handling
						//client needs to be checked in
						if(updateStatus[mapperID][0]!= mapperID || updateStatus[mapperID][2]== 0){
							response[0]= command;
							response[1]= RSP_NOK;
							response[2]= mapperID;
							write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
							printf("GET_AZLIST: mapperID %d hasn't yet checked in\n", mapperID );
						}else{

								printf("[%d] GET_AZLIST\n", mapperID);
								resAndArray[0]= command;
								resAndArray[1]= RSP_OK;
								//mutex lock
								pthread_mutex_lock(&azList_lock);
								for(int k =0; k< ALPHABETSIZE; k++){
										resAndArray[k+2] = azList[k];
								}
													//mutex unlock
								write(tArg->clientfd, (void *)resAndArray, sizeof(int)*LONG_RESPONSE_MSG_SIZE);
								pthread_mutex_unlock(&azList_lock);
						}

					}else{// extra credit
						printf("[%d] GET_AZLIST\n", mapperID);
						resAndArray[0]= command;
						resAndArray[1]= RSP_OK;
						//mutex lock
						pthread_mutex_lock(&azList_lock);
						for(int k =0; k< ALPHABETSIZE; k++){
							resAndArray[k+2] = azList[k];
						//	printf("%d ",resAndArray[k+2] );
						}
						write(tArg->clientfd, (void *)resAndArray, sizeof(int)*LONG_RESPONSE_MSG_SIZE);
						//mutex unlock
						pthread_mutex_unlock(&azList_lock);
						flag = RSP_NOK; //break & close
					}

	 				break;

	 			case GET_MAPPER_UPDATES:
					if(mapperID >0){
						//error handling
						if(updateStatus[mapperID][0]!= mapperID || updateStatus[mapperID][2]== 0){
							response[0]= command;
							response[1]= RSP_NOK;
							response[2]= updateStatus[mapperID][1];
							write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
							printf("GET_MAPPER_UPDATES: mapperID %d hasn't yet checked in\n", mapperID );
						}else{
							printf("[%d] GET_MAPPER_UPDATES\n", mapperID);
			 				response[0]= command;
			 				response[1]= RSP_OK;
			 				response[2]= updateStatus[mapperID][1];
			 				write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						}

					}else{// extra credit
						response[0]= command;
						response[1]= RSP_NOK;
						response[2]= 0;
						write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						printf("[%d] GET_MAPPER_UPDATES\nMapper PID(%d) should be greater than 0\n", mapperID, mapperID);
						flag = RSP_NOK; //break & close
					}

	 				break;

	 			case GET_ALL_UPDATES://+extra credit
					if(mapperID > 0){
						//error handling
						if(updateStatus[mapperID][0]!= mapperID || updateStatus[mapperID][2]== 0){
							response[0]= command;
							response[1]= RSP_NOK;
							response[2]= mapperID;
							write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
							printf("GET_ALL_UPDATES: mapperID %d hasn't yet checked in\n", mapperID );
						}else{

		 					printf("[%d] GET_ALL_UPDATES\n", mapperID);
		 					int sum=0;
		 					for(int j=0; j < 33;j++){
		 						sum += updateStatus[j][1];
		 					}
		 					response[0]= command;
		 					response[1]= RSP_OK;
		 					response[2]= sum;
		 					write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						}
					}else{// extra credit
						printf("[%d] GET_ALL_UPDATES\n", mapperID);
						int sum=0;
						for(int j=0; j < 33;j++){
							sum += updateStatus[j][1];
						}
						response[0]= command;
						response[1]= RSP_OK;
						response[2]= sum;
						write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						flag = RSP_NOK; //break & close
					}
	 				break;

	 			case CHECKOUT://CHECKOUT
				if(mapperID > 0){
 					//error handling
					if(updateStatus[mapperID][0]!= mapperID || updateStatus[mapperID][2]== 0){
						response[0]= command;
						response[1]= RSP_NOK;
						response[2]= mapperID;
						write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						printf("GET_ALL_UPDATES: mapperID %d hasn't yet checked in\n", mapperID );
					}else{
		 					printf("[%d] CHECKOUT\n", mapperID);
		 					updateStatus[mapperID][2]=0;
		 					response[0]= command;
		 					response[1]= RSP_OK;
		 					response[2]= mapperID;
		 					write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
		 					flag = RSP_NOK;
						}
					}else{// extra credit
						response[0]= command;
						response[1]= RSP_NOK;
						response[2]= mapperID;
						write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
						printf("[%d] CHECKOUT\nMapper PID(%d) should be greater than 0\n", mapperID, mapperID);
						flag = RSP_NOK; //break & close
					}
					break;

	 			default: //wrong command
	 					response[0]= command;
	 					response[1]= RSP_NOK;
	 					response[2]= mapperID;
	 					write(tArg->clientfd, response, sizeof(int)*RESPONSE_MSG_SIZE);
	 					printf("[%d] Wrong Command ID %d\n", mapperID, command );
	 				break;
	 			}
				//printf("the end of switch, %d\n", mapperID );
		 }
		 //printf("the end of while loop %d command: %d\n",  mapperID, command);
	 }//end while

	close(tArg->clientfd);
	printf("close connection from %s:%d\n", tArg->clientip ,	tArg->clientport  );
	free(tArg);
	pthread_mutex_lock(&currentConn_lock);
	currentConn--;
	pthread_mutex_unlock(&currentConn_lock);
	return NULL;
	printf("end of thread func\n");
}

int main(int argc, char** argv) {
	int server_port;

	 if (argc == 2) { // 1 arguments
			 server_port = atoi(argv[1]);
	 } else {
			 printf("Invalid or less number of arguments provided\n");
			 printf("./server <server Port>\n");
			 exit(0);
	 }

	//initializing arrays
 	int x;
 	for(x =0; x < MAX_STATUS_TABLE_LINES; x++){
		updateStatus[x][0]=0;
		updateStatus[x][1]=0;
		updateStatus[x][2]=0;
	}

	for(int y=0; y<26;y++){
		azList[y] =0;
	}


	pthread_t threads[MAX_CONCURRENT_CLIENTS];
	int count = 0;
	pthread_mutex_init(&currentConn_lock, NULL);

	// Create a TCP socket.
	int sock = socket(AF_INET , SOCK_STREAM , 0);

	// Bind it to a local address.
	struct sockaddr_in servAddress;
	servAddress.sin_family = AF_INET;
	servAddress.sin_port = htons(server_port);
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	int check_bind = bind(sock, (struct sockaddr *) &servAddress, sizeof(servAddress));

	if(check_bind != 0){
		printf("Unsucces binding\n");
		exit(0);
	}

	// We must now listen on this port.
	int check_listen = listen(sock, MAX_CONCURRENT_CLIENTS);

	if(check_listen != 0){
		printf("Unsucces listening\n");
		exit(0);
	}

	printf("server is listening\n");

	// A server typically runs infinitely, with some boolean flag to terminate.
	while (1) {
		// Now accept the incoming connections.
		struct sockaddr_in clientAddress;

		socklen_t size = sizeof(struct sockaddr_in);
		int clientfd = accept(sock, (struct sockaddr*) &clientAddress, &size);

    struct threadArg *arg = (struct threadArg *) malloc(sizeof(struct threadArg));

		arg->clientfd = clientfd;
		arg->clientip = inet_ntoa(clientAddress.sin_addr);
		arg->clientport = clientAddress.sin_port;
		printf("open connection from %s:%d\n", arg->clientip ,	arg->clientport  );
		//TODO: Handle the accepted connection by passing off functionality to a thread
		//      Up to MAX_CONCURRENT_CLIENTS threads can be running simultaneously, so you will
		//      have to decide how to ensure that this condition holds.
		pthread_mutex_lock(&currentConn_lock);

		if(currentConn == MAX_CONCURRENT_CLIENTS) {
			printf("Server is too busy\n");
			close(clientfd);
			free(arg);
			continue;
		}else{
			pthread_create(&threads[currentConn], NULL, (void*)threadFunction, (void*)arg);
			currentConn++;
			count++;
			if(count==MAX_CONCURRENT_CLIENTS){
				count=0;
			}
		}
		pthread_mutex_unlock(&currentConn_lock);
	}
	// Close the socket.
	close(sock);

}
