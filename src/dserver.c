#include "services.h"
#include "defs.h"

int main(int argc, char *argv[])
{

	int pides[PNUM];	// save the son's pids
	int pidesCount = 0; // counter of pids
	char documentFolder[48];
	if (argc > 1)
	{

		strcpy(documentFolder, argv[1]);
	}
	else
	{
		strcpy(documentFolder, "");
	}
	print_debug("SERVER LAUNCHED\n");
	GTree *indexTree = g_tree_new_full(compare_str, NULL, g_free, g_free);

	print_debug("Searching for Meta Information on Saves\n");
	buildMetaInfo(indexTree);

	// make fifo e o loop com o dummy
	if (mkfifo(C_TO_S, 0666) == -1)
	{
		if (errno != EEXIST)
		{ // if the FIFO already exists no problem
			perror("mkfifo client to server failed");
			return 1;
		}
	}
	int fdin = open(C_TO_S, O_RDONLY);
	if (fdin == -1)
	{
		perror("open client to server failed");
	}
	int dummy_fd = open(C_TO_S, O_WRONLY);
	if (dummy_fd == -1)
	{
		perror("open dummy failed");
	}

	MSG in;
	ssize_t bytesRead;
	if (pidesCount >= PNUM)
	{
		int status;
		for (int l = 0; l < PNUM; l++)
		{
			waitpid(pides[l], &status, 0);

			if (WIFEXITED(status))
			{
				int code = WEXITSTATUS(status);
				char soncode[255];
				snprintf(soncode, 255, "Filho %d terminou com código %d\n", pides[l], code);
				print_debug(soncode);
			}
		}
		pidesCount = 0;
	}
	while ((bytesRead = read(fdin, &in, sizeof(MSG))))
	{
		// falta por isto com forks e coletar e como modificar a arvore dentro do fork, n modifica o do pai
		// por isso n faz diferenca precisamos msm de ter fork ent?

		print_debug("\n----New ITERATION----\n");

		char outfifo[20];

		snprintf(outfifo, sizeof(outfifo), "../fifos/output%d", in.pid);
		if (mkfifo(outfifo, 0666) == -1)
		{
			if (errno != EEXIST)
			{ // if the FIFO already exists no problem
				perror("mkfifo outfifo(pid) failed");
				return 1;
			}
		}
		int fdout = open(outfifo, O_WRONLY);
		if (fdout == -1)
		{
			perror("open server to client fifo failed");
		}

		// Storing stdout fd
		int original_stdout = dup(1);
		if (original_stdout == -1)
		{
			perror("dup failed");
			close(fdout);
			return 1;
		}
		// Redirect stdout to the FIFO
		if (dup2(fdout, 1) == -1)
		{
			perror("dup2 failed");
		}
		close(fdout);
		if (strcmp(in.flag, "-a") == 0)
		{
			if (pidesCount != 0)
			{
				int status;
				for (int l = 0; l < PNUM; l++)
				{
					waitpid(pides[l], &status, 0);

					if (WIFEXITED(status))
					{
						int code = WEXITSTATUS(status);
						char soncode[255];
						snprintf(soncode, 255, "Filho %d terminou com código %d\n", pides[l], code);
						print_debug(soncode);
					}
				}
				pidesCount = 0;
			}
			print_debug("-a executing\n");
			Index *t;

			t = malloc(sizeof(Index));
			// turning the Msg into Index
			strcpy(t->title, in.argv[0]);
			strcpy(t->authors, in.argv[1]);
			t->year = atoi(in.argv[2]);
			char text[16];
			strcpy(text, in.argv[3]);
			sprintf(t->path, "%s/%s", documentFolder, text);
			indexDocument(indexTree, t);
			print_debug("-a finished\n");
		}
		else if (strcmp(in.flag, "-c") == 0)
		{
			// done
			// fildes[0] le
			// fildes[1] escreve

			pid_t pid1 = fork();

			if (pid1 == 0)
			{
				sleep(5);
				print_debug("-c executing\n");
				char title_ind[200];			// i gave this name because its a title index
				strcpy(title_ind, in.argv[0]);	// Copy of the Key index to title_ind
				checkKey(indexTree, title_ind); // check if meta information about a key is on the tree
				print_debug("-c finished\n");
				_exit(1);
			}
			pides[pidesCount++] = pid1;
		}
		else if (strcmp(in.flag, "-d") == 0)
		{
			// done
			print_debug("-d executing\n");
			char title_ind[200]; // same as above
			strcpy(title_ind, in.argv[0]);
			deleteKey(indexTree, title_ind); // delete the meta information of key index from the tree if exists
			print_debug("-d finished\n");
		}
		else if (strcmp(in.flag, "-l") == 0)
		{
			// done

			pid_t pid1 = fork();

			if (pid1 == 0)
			{
				print_debug("-l executing\n");
				char title_ind[200]; // same as above
				char word[200];
				strcpy(title_ind, in.argv[0]);
				strcpy(word, in.argv[1]);
				searchKeywordByKey(indexTree, title_ind, word);
				print_debug("-l finished\n");
				_exit(1);
			}
			pides[pidesCount++] = pid1;
		}
		else if (strcmp(in.flag, "-s") == 0)
		{
			// done

			pid_t pid1 = fork();

			if (pid1 == 0)
			{
				print_debug("-s executing\n");
				char word[200];
				char numProc[100];
				strcpy(word, in.argv[0]);
				strcpy(numProc, in.argv[1]);
				searchKeyword(indexTree, word, atoi(numProc));
				print_debug("-s finished\n");
				_exit(1);
			}
			pides[pidesCount++] = pid1;
		}
		else if (strcmp(in.flag, "-f") == 0)
		{
			// doing

			if (pidesCount != 0)
			{
				int status;
				for (int l = 0; l < PNUM; l++)
				{
					waitpid(pides[l], &status, 0);

					if (WIFEXITED(status))
					{
						int code = WEXITSTATUS(status);
						char soncode[255];
						snprintf(soncode, 255, "Filho %d terminou com código %d\n", pides[l], code);
						print_debug(soncode);
					}
				}
				pidesCount = 0;
			}
			print_debug("-f executing\n");

			saveMetaInfo(indexTree);   // save the meta Information on a binary file for next time use
			g_tree_destroy(indexTree); // free the tree
			close(dummy_fd);		   // kills the dummy
		}

		if (indexTree != NULL)
		{
			print_debug("PRINTING TREE:\n");
			g_tree_foreach(indexTree, print_index_debug, NULL);
		}
		else
		{
			perror("Tree is NULL (empty or not initialized).\n");
		}

		fflush(stdout); // make sure it flushes immediately

		// Restoring the stdout fd
		if (dup2(original_stdout, STDOUT_FILENO) == -1)
		{
			perror("dup2 back to original stdout failed");
			close(original_stdout);
			return 1;
		}
		if (close(original_stdout) == -1)
		{
			perror("close failed");
		}
		
	}

	if (bytesRead == -1)
	{
		perror("read server to client failed");
	}

	close(fdin);

	if (unlink(C_TO_S) == -1)
	{
		perror("unlink failed");
	}

	return 0;
}
