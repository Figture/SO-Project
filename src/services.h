#ifndef SERVICES_H_
#define SERVICES_H_


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


typedef struct index{
	char title[200];
	char authors[200];
	char path[64];
	int year;
} Index;

int indexDocument();

int checkKey();

int deleteKey();

int searchKeywordByKey();

int searchKeyword();

#endif
