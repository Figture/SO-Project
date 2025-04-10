#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



int main(int argc, char *argv[]){

	if(argc<3){
		printf("Usage:\n");
        printf("Index document: ./dclient -a [title] [authors] [year] [path]\n");
        printf("Check key: ./dclient -c [key]\n");
        printf("Delete key: ./dclient -d [key]\n");
        printf("Search keyword of given key: ./dclient -l [key] [keyword]\n");
        printf("Search keyword of all keys: ./dclient -s [keyword]\n");
		// printf("Stop server: ./dclient -f\n");
        return 1;
	}

	// Make the MSG
	MSG input;
	if(argc > 1) {
		strcpy(input.flag,argv[1]);
		for (int i = 2; i < argc; i++) {
			strcpy(input.argv[i-2], argv[i]);
		}
	}
	input.pid = getpid();
	
	// ligar o fifo
	int fdin = open(C_TO_S, O_WRONLY);
	
	write(fdin, &input, sizeof(MSG));


	// fifo output por cliente
	


	/*
	if(strcmp(argv[1],"-a")==0){
		// TO DO
	}else if(strcmp(argv[1],"-c")==0){
		// TO DO
	}else if(strcmp(argv[1],"-d")==0){
		// TO DO
	}else if(strcmp(argv[1],"-l")==0){
		// TO DO
	}else if(strcmp(argv[1],"-s")==0){
		// TO DO
	}else if(strcmp(argv[1],"-f")==0){
		// TO DO
	}*/

	close(fdin);
}
