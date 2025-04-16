#include "services.h"
#include "defs.h"

int main(int argc, char *argv[])
{
	printf("SERVER LAUNCHED\n");
	GTree *indexTree = g_tree_new_full(compare_str, NULL, g_free, g_free);
	
	printf("Searching for Meta Information on Saves\n");
	buildMetaInfo(indexTree);

	// make fifo e o loop com o dummy
	mkfifo(C_TO_S, 0666);
	int fdin = open(C_TO_S, O_RDONLY);
	int dummy_fd = open(C_TO_S, O_WRONLY);

	MSG in;
	while (read(fdin, &in, sizeof(MSG)))
	{
		// fazer a concorrencia com N filhos
		if (strcmp(in.flag, "-a") == 0)
		{
			printf("-a executing\n");
			// meter isto no services para ser mais clean?
			Index *t;
			t = malloc(sizeof(Index));
			strcpy(t->title, in.argv[0]);
			strcpy(t->authors, in.argv[1]);
			t->year = atoi(in.argv[2]);
			strcpy(t->path, in.argv[3]);
			indexDocument(indexTree, t);
			printf("-a finished\n");
		}
		else if (strcmp(in.flag, "-c") == 0)
		{
			// done
			printf("-c executing\n");
			char title_ind[200]; // i gave this name because its a title index
			strcpy(title_ind, in.argv[0]); // Copy of the Key index to title_ind
			checkKey(indexTree, title_ind); //check if meta information about a key is on the tree
			printf("-c finished\n");
		}
		else if (strcmp(in.flag, "-d") == 0)
		{
			// done
			printf("-d executing\n");
			char title_ind[200]; // same as above
			strcpy(title_ind, in.argv[0]);
			deleteKey(indexTree, title_ind); //delete the meta information of key index from the tree if exists
			printf("-d finished\n");
		}
		else if (strcmp(in.flag, "-l") == 0)
		{
			// done
			printf("-l executing\n");
			char title_ind[200]; // same as above
			char word[200];
			strcpy(title_ind, in.argv[0]);
			strcpy(word, in.argv[1]);
			searchKeywordByKey(indexTree, title_ind, word);
			printf("-l finished\n");
		}
		else if (strcmp(in.flag, "-s") == 0)
		{
			// done
			printf("-s executing\n");
			char word[200];
			strcpy(word, in.argv[0]);
			searchKeyword(indexTree, word);
			printf("-s finished\n");
		}
		else if (strcmp(in.flag, "-f") == 0)
		{
			//doing
			printf("-f executing\n");
			
			saveMetaInfo(indexTree); //save the meta Information on a binary file for next time use
			g_tree_destroy(indexTree); //free the tree
			close(dummy_fd); //kills the dummy 
			 
		}

		if (indexTree != NULL) {
			printf("PRINTING TREE:\n");
			g_tree_foreach(indexTree, print_index, NULL);
		} else {
			printf("Tree is NULL (empty or not initialized).\n");
		}
		// separador por iteracao
		printf("\n--------------------\n");
	}

	close(fdin);
	
	unlink(C_TO_S);

	return 0;
}
