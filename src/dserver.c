#include <services.h>
#include "gtree.h"

int main(int argc, char *argv[]){
	// TO DO
	GTree *tree = g_tree_new_full(compare_str, NULL, g_free, g_free);

	// make fifo e o loop com o dummy


	return 0;
}
