#include "services.h"
#include "defs.h"

int main(int argc, char *argv[])
{
	printf("SERVER LAUNCHED\n");
	GTree *indexTree = g_tree_new_full(compare_str, NULL, g_free, g_free);

	// make fifo e o loop com o dummy
	mkfifo(C_TO_S, 0666);
	int fdin = open(C_TO_S, O_RDONLY);
	int dummy_fd = open(C_TO_S, O_WRONLY);

	MSG in;
	while (read(fdin, &in, sizeof(MSG)))
	{
		pid_t pid = fork();
		// output buffer for now
		char output[100];
		char outfifo[20];
		if (pid == 0) {
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
				// doing
				printf("-c executing\n");
				char title_ind[200]; // i give this name because its a title index
				strcpy(title_ind, in.argv[0]);
				checkKey(indexTree, title_ind);
				printf("-c finished\n");
			}
			else if (strcmp(in.flag, "-d") == 0)
			{
				// doing
				printf("-d executing\n");
				char title_ind[200]; // same as above
				strcpy(title_ind, in.argv[0]);
				deleteKey(indexTree, title_ind);
				printf("-d finished\n");
			}
			else if (strcmp(in.flag, "-l") == 0)
			{
				// TO DO
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
				// TO DO
				printf("-s executing\n");
				char word[200]; 
				strcpy(word, in.argv[0]);
				searchKeyword(indexTree, word);
				printf("-s finished\n");
			}

			printf("PRINTING TREE:\n");
			g_tree_foreach(indexTree, print_index, NULL);

			// separador por iteracao
			printf("\n--------------------\n");

			//template do output talvez melhorar por causa do output ser muito grande e fazer por dup
			snprintf(outfifo, sizeof(outfifo), "fifos/output%d", in.pid);
			int fdout = open(outfifo, O_WRONLY);
			snprintf(output, sizeof(output), "este é o template basico: %d", pid);
			write(fdin, &output, sizeof(output));

		}
	}

	close(fdin);
	close(dummy_fd);
	unlink(C_TO_S);

	return 0;
}
