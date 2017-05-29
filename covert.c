/*
// covert.c
// implements a covert channel between two processes
// channel relies on changing the permission bits of testfile
// Mano Wared 913398346
// Patrick Ong 997520004
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

char *filename = "./testfile";

void sendbit(char c){
	mode_t mode0 = 0100664;
	mode_t mode1 = 0100666;

	if(c == '0'){
		chmod(filename,mode0);
		printf("Bit sent\n");
	}
	else if(c == '1'){
		chmod(filename,mode1);
		printf("Bit sent\n");
	}
	else{
		printf("Error: Bit is neither 0 nor 1\n");
	}
	return;
}

int decodebit(){
	struct stat* getstat = (struct stat*)malloc(sizeof(stat));
	stat(filename, getstat);
	if(getstat->st_mode % 10 == 6){
		printf("Received 1 ... \n");
		return 1;
	}
	else if(getstat->st_mode % 10 == 4){
		printf("Received 0 ... \n");	
		return 0;
	}
	else{
		printf("Error: unknown message value\n");
		return -1;
	}
}

int main(){

	char *buffer;
	int bufsize = 5;
	buffer = (char*)malloc((bufsize+1) * sizeof(char));

	printf("Enter %d-bit string: ", bufsize);
	fgets(buffer, bufsize+1, stdin);
	//sleep(1);

	struct stat* statbuf = (struct stat*)malloc(sizeof(stat));
	stat(filename, statbuf);
	mode_t orig_mode = statbuf->st_mode;

	if (fork() != 0){ // parent
		int i = 0;
		for(i; i < bufsize; i++){
			sendbit(buffer[i]);
			if(i != bufsize -1)
				sleep(2);
		}
		int status;
		wait(&status);
	}
	else{ // child
		int j = 0;
		int message[bufsize];
		printf("Child sleeping to ensure parent goes first... \n");
		sleep(1);
		for(j; j < bufsize; j++){
			message[j] = decodebit();
			if(j != bufsize -1)
				sleep(2);
		}
		printf("Message reads: ");
		for(j = 0; j < bufsize; j++){
			printf("%d",message[j]);
		}
		printf("\n");
	}

	chmod(filename, orig_mode); // restore original permissions

	return 0;
}