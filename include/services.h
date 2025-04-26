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
	int numProc;
	GList *indexList;
} DATA_W;

#define SAVE_FILE "../saves"// Save file to save meta information presented on the tree
gint compare_str(gconstpointer a, gconstpointer b, gpointer user_data);

gint print_index(gpointer key, gpointer value, gpointer data);

int indexDocument(GTree *tree, Index *in);

int checkKey(GTree *tree, char index[]);

int deleteKey(GTree *tree, char index[]);

int searchKeywordByKey(GTree *tree, char index[], char word[]);

int searchKeyword(GTree *tree, char word[], int numProc);

int saveMetaInfo(GTree *tree);

int buildMetaInfo(GTree *tree);

gint findWord(gpointer key, gpointer value, gpointer data);

gint saveMetaInfoNode(gpointer key, gpointer value, gpointer data);