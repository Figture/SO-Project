#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <glib.h>

typedef struct index{
	char title[200];
	char authors[200];
	char path[64];
	int year;
} Index;

typedef struct data_word{
    char *word;
} DATA_W;

int NUM_PROC = 10;

gint compare_str(gconstpointer a, gconstpointer b, gpointer user_data);

gint print_index(gpointer key, gpointer value, gpointer data);

int indexDocument(GTree *tree, Index *in);

int checkKey();

int deleteKey();

int searchKeywordByKey();

int searchKeyword();
