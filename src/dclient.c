#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "defs.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{

	if (argc < 2)
	{
		printf("Usage:\n");
		printf("Index document: ./dclient -a [title] [authors] [year] [path]\n");
		printf("Check key: ./dclient -c [key]\n");
		printf("Delete key: ./dclient -d [key]\n");
		printf("Search keyword of given key: ./dclient -l [key] [keyword]\n");
		printf("Search keyword of all keys: ./dclient -s [keyword]\n");
		printf("Stop server: ./dclient -f\n");
		return 1;
	}

	// Make the MSG
	MSG input;
	if (argc > 1)
	{ 
		strcpy(input.flag, argv[1]);
		if (argc == 2 && (strcmp(input.flag, "-f") != 0))
		{
			printf("Usage:\n");
			printf("Index document: ./dclient -a [title] [authors] [year] [path]\n");
			printf("Check key: ./dclient -c [key]\n");
			printf("Delete key: ./dclient -d [key]\n");
			printf("Search keyword of given key: ./dclient -l [key] [keyword]\n");
			printf("Search keyword of all keys: ./dclient -s [keyword]\n");
			printf("Stop server: ./dclient -f\n");
			return 1;
		}
		for (int i = 2; i < argc; i++)
		{
			strcpy(input.argv[i - 2], argv[i]);
		}
	}
	input.pid = getpid();

	// making fifo client to server in case the server still not initiated
	if (mkfifo(C_TO_S, 0666) == -1)
	{
		if (errno != EEXIST)
		{ // if the FIFO already exists no problem
			perror("mkfifo c_to_s failed");
			return 1;
		}
	}

	// Opening fifo Client to Server
	int fdin = open(C_TO_S, O_WRONLY);
	if (fdin == -1)
	{
		perror("open client to server failed");
	}

	write(fdin, &input, sizeof(MSG));

	// different fifo output for each client
	char outfifo[20];
	snprintf(outfifo, sizeof(outfifo), "../fifos/output%d", input.pid);
	if (mkfifo(outfifo, 0666) == -1)
	{
		if (errno != EEXIST)
		{ // if the FIFO already exists no problem
			perror("mkfifo outfifo(pid) failed");
			return 1;
		}
	}
	int fdout = open(outfifo, O_RDONLY);
	if (fdin == -1)
	{
		perror("open server to client failed");
	}
	char output[100];
	ssize_t bytesRead;
	while ((bytesRead = read(fdout, output, sizeof(output) - 1)) > 0)
	{
		output[bytesRead] = '\0'; // Null to turn output a string
		printf("%s", output);
	}
	if (bytesRead == -1)
	{
		perror("read server to client failed");
	}
	if (unlink(outfifo) == -1)
	{
		perror("unlink failed");
	}

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
