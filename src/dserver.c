#include "services.h"

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
	int maxNodes = atoi(argv[2]);

	print_debug("SERVER LAUNCHED\n");
	GTree *indexTree = g_tree_new_full(compare_str, NULL, g_free, g_free);
	// FIFO Implementation
	GQueue *insertionOrder = g_queue_new();

	int fdsave = open(SAVE_FILE, O_CREAT | O_RDWR, 0666); // file descriptor to the save file
	print_debug("Searching for Meta Information on Saves\n");
	int numNodes = buildMetaInfo(indexTree, fdsave, maxNodes, insertionOrder);
	// lseek(fdsave, 0, SEEK_END);

	// make fifo e o loop com o dummy
	if (mkfifo(C_TO_S, 0666) == -1)
	{
		if (errno != EEXIST)
		{ // if the FIFO already exists no problem
			perror("mkfifo client to server failed");
			unlink(C_TO_S);
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
	// printf("Vou ler\n");
	while ((bytesRead = read(fdin, &in, sizeof(MSG)) > 0))
	{
		if (strcmp("r", in.flag) == 0)
		{
			// ao dar -d, isto n ta a dar update direito
			print_debug("--updating the fifo--\n");
			// whenever it needs to reload the FIFO, wait for the child who requested it
			int status;
			waitpid(in.pid, &status, 0);

			if (WIFEXITED(status))
			{
				int code = WEXITSTATUS(status);
				char soncode[255];
				snprintf(soncode, 255, "Filho %d terminou com código %d\n", in.pid, code);
				print_debug(soncode);
			}
			// if the parent didnt wait, discount it but still need to remove it... so need to fix
			// if (pidesCount != 0)
			// 	pidesCount--;
			
			Index *temp = malloc(sizeof(Index));
			// seek the found one
			off_t res = lseek(fdsave, in.offset, SEEK_SET);
			if (res == -1) {
				perror("lseek failed");
			}

			read(fdsave, temp, sizeof(Index));
			g_tree_insert(indexTree, g_strdup(temp->title), temp);
			g_queue_push_tail(insertionOrder, temp);
			// then restore the offset
			lseek(fdsave, 0, SEEK_END);

			numNodes++;
			if (numNodes > maxNodes)
			{
				Index *oldest = g_queue_pop_head(insertionOrder);
				// free(oldest); // or reuse
				g_tree_remove(indexTree, oldest->title);
				numNodes--;
			}
		}

		else
		{
			// secure the number of son's processes
			if (pidesCount >= PNUM)
			{
				int status;
				for (int l = 0; l < pidesCount; l++)
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

			print_debug("\n----New ITERATION----\n");

			char outfifo[20];

			snprintf(outfifo, sizeof(outfifo), "../fifos/output%d", in.pid);
			// if (mkfifo(outfifo, 0666) == -1)
			// {
			// 	if (errno != EEXIST)
			// 	{ // if the FIFO already exists no problem
			// 		perror("mkfifo outfifo(pid) failed");
			// 		return 1;
			// 	}
			// }
			int fdout = open(outfifo, O_WRONLY);
			if (fdout == -1)
			{
				perror("open server to client fifo failed");
			}
			if (strcmp(in.flag, "-a") == 0)
			{
				if (pidesCount != 0)
				{
					int status;
					for (int l = 0; l < pidesCount; l++)
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
				snprintf(t->path, 64, "%s/%s", documentFolder, text);
				numNodes = indexDocument(indexTree, t, fdout, fdsave, maxNodes, insertionOrder, numNodes);
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
					// sleep(5);
					print_debug("-c executing\n");
					char title_ind[200];				   // i gave this name because its a title index
					strcpy(title_ind, in.argv[0]);		   // Copy of the Key index to title_ind
					checkKey(indexTree, title_ind, fdout); // check if meta information about a key is on the tree
					print_debug("-c finished\n");
					// close(fdsave);
					_exit(1);
				}
				pides[pidesCount++] = pid1;
			}
			else if (strcmp(in.flag, "-d") == 0)
			{
				// after the other child operations we can delete
				if (pidesCount != 0)
				{
					int status;
					for (int l = 0; l < pidesCount; l++)
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
				print_debug("-d executing\n");
				char title_ind[200]; // same as above
				strcpy(title_ind, in.argv[0]);
				numNodes = deleteKey(indexTree, title_ind, fdout, fdsave, insertionOrder, numNodes); // delete the meta information of key index from the tree if exists
				print_debug("-d finished\n");
			}
			else if (strcmp(in.flag, "-l") == 0)
			{
				pid_t pid1 = fork();

				if (pid1 == 0)
				{
					print_debug("-l executing\n");
					char title_ind[200]; // same as above
					char word[200];
					strcpy(title_ind, in.argv[0]);
					strcpy(word, in.argv[1]);
					searchKeywordByKey(indexTree, title_ind, word, fdout);
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
				if(in.argc == 4) strcpy(numProc, in.argv[1]);
				else sprintf(numProc, "%d", 1); // 1 process if number not given

				searchKeyword(indexTree, word, atoi(numProc),fdout);
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
					for (int l = 0; l < pidesCount; l++)
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

				// saveMetaInfo(indexTree,fdout);   // save the meta Information on a binary file for next time use
				g_tree_destroy(indexTree); // free the tree
				close(dummy_fd);		   // kills the dummy
			}

			
			close(fdout);
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
		printf("=== Queue Contents ===\n");
		g_queue_foreach(insertionOrder, print_index_queue, NULL);
	}

	if (bytesRead == -1)
	{
		perror("read server to client failed");
	}

	close(fdin);
	close(fdsave);

	if (unlink(C_TO_S) == -1)
	{
		perror("unlink failed");
	}

	return 0;
}
