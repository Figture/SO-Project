#include "services.h"
#include "defs.h"

int main(int argc, char *argv[])
{
	printf("SERVER LAUNCHED\n");
	GTree *indexTree = g_tree_new_full(compare_str, NULL, g_free, g_free);
	
	printf("Searching for Meta Information on Saves\n");
	buildMetaInfo(indexTree);

	// make fifo e o loop com o dummy
	if (mkfifo(C_TO_S, 0666) == -1) {
		if (errno != EEXIST) { // it's okay if the FIFO already exists
			perror("mkfifo client to server failed");
			return 1;
		}
	}
	int fdin = open(C_TO_S, O_RDONLY);
	if (fdin == -1) {
		perror("open client to server failed");
	}
	int dummy_fd = open(C_TO_S, O_WRONLY);
	if (dummy_fd == -1) {
		perror("open dummy failed");
	}


	MSG in;
	ssize_t bytesRead;
	while ((bytesRead = read(fdin, &in, sizeof(MSG))))
	{
		// falta por isto com forks e coletar

		// output buffer for now
		char output[100];
		char outfifo[20];
		if (strcmp(in.flag, "-a") == 0)
		{
			printf("-a executing\n");
			Index *t;
			t = malloc(sizeof(Index));
			// turning the Msg into Index
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
			char numProc[100];
			strcpy(word, in.argv[0]);
			strcpy(numProc, in.argv[1]);
			searchKeyword(indexTree, word, atoi(numProc));
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

		// alternativa com dup2
		snprintf(outfifo, sizeof(outfifo), "../fifos/output%d", in.pid);
		int fdout = open(outfifo, O_WRONLY);
		if (fdout == -1) {
			perror("open server to client fifo failed");
		}

		// Storing stdout fd
		int original_stdout = dup(1);
		if (original_stdout == -1) {
			perror("dup failed");
			close(fdout);
			return 1;
		}
		// Redirect stdout to the FIFO
		if (dup2(fdout, 1) == -1) {
			perror("dup2 failed");
		}

		// Now this goes directly into the FIFO
		printf("este é o template basico: %d\n", getpid());
		fflush(stdout);  // make sure it flushes immediately

		// Restoring the stdout fd
		if (dup2(original_stdout, STDOUT_FILENO) == -1) {
			perror("dup2 back to original stdout failed");
			close(fdout);
			return 1;
		}
		if (close(fdout) == -1) {
			perror("close failed");
		}
		if (close(original_stdout) == -1) {
			perror("close failed");
		}

		// falta trocar para o stdout origianl
	}
	if (bytesRead == -1) {
		perror("read server to client failed");
	}

	close(fdin);
	
	if (unlink(C_TO_S) == -1) {
		perror("unlink failed");
	}

	return 0;
}
