#include <services.h>
#include "gtree.h"

int indexDocument(GTree *tree, Index *in){
	// TO DO
	g_tree_insert(tree, g_strdup(in->title), in);

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
