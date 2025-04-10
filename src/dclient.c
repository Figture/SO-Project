#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"

#define C_TO_S "fifos/c_to_s" // Client TO Server

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

	// ligar o fifo
	


	// e se temos varios clientes? abrir um fifo por cliente (PID)? porque tem mts argumentos
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
	}/*else if(strcmp(argv[1],"-f")==0){
		// TO DO
	}*/
}
