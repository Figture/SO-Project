#include "services.h"

gint compare_str(gconstpointer a, gconstpointer b, gpointer user_data) {
    return g_strcmp0((const char *)a, (const char *)b);
}


gint print_index(gpointer key, gpointer value, gpointer data) {
    Index *idx = (Index *)value;
    printf("Title: %s\nAuthor: %s\n", idx->title, idx->authors);
	return 0;
}


int indexDocument(GTree *tree, Index *in){
	// TO DO
	g_tree_insert(tree, g_strdup(in->title), in);
	printf("Indexed Successfully\n");
	return 0;
}

int checkKey(){
	// TO DO
	return 0;
}

int deleteKey(){
	// TO DO
	return 0;
}

int searchKeywordByKey(){
	// TO DO
	return 0;
}

int searchKeyword(){
	// TO DO
	return 0;
}
