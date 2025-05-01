#include "services.h"

void print_debug(const char *msg) {
	//this function is to print on stderr and make it red color
    write(2, "\033[96m", 5);
    write(2, msg, strlen(msg));
    write(2, "\033[0m", 4);
}

gint compare_str(gconstpointer a, gconstpointer b, gpointer user_data)
{
	return g_strcmp0((const char *)a, (const char *)b);
}

gint print_index(gpointer key, gpointer value, gpointer data)
{
	if (value == NULL)
	{
		printf("Value is NULL for key:\n");
		return 0;
	}

	Index *idx = (Index *)value;

	printf("Title: %s\nAuthor: %s\nYear: %d\nPath: %s\n", idx->title, idx->authors, idx->year, idx->path);

	return 0;
}

gint print_index_debug(gpointer key, gpointer value, gpointer data)
{
	if (value == NULL)
	{
		print_debug("Value is NULL for key:\n");
		return 0;
	}

	Index *idx = (Index *)value;

	// talvez ajustar o tamanho do buffer, mas a soma de todos os elementos n da 1024
	char buffer[1024];
	snprintf(buffer, 1024, "Title: %s\nAuthor: %s\nYear: %d\nPath: %s\n", idx->title, idx->authors, idx->year, idx->path);

	print_debug(buffer);
	print_debug("---Node-Seperator---\n");

	return 0;
}
gint print_indexV2(gpointer value)
{ // Fiz esta versão porque nao percebi a do Pedro Mo ||João Oliveira
	if (value == NULL)
	{
		printf("Value is NULL\n");
		return 0;
	}
	Index *idx = (Index *)value;
	printf("Title: %s\nAuthor: %s\nYear: %d\nPath: %s\n", idx->title, idx->authors, idx->year, idx->path);
	return 0;
}

int indexDocument(GTree *tree, Index *in)
{
	g_tree_insert(tree, g_strdup(in->title), in);
	printf("Indexed \"%s\" Successfully\n", in->title);
	print_debug("Indexed Successfully\n");
	return 0;
}

int checkKey(GTree *tree, char index[])
{
	// Doing
	gpointer exist;
	exist = g_tree_lookup(tree, g_strdup(index)); // search on the tree to see if the key index exists (returns Node if exists|NULL if don't exists)

	if (exist == NULL)
	{
		print_debug("Meta Information about the requested index was not found\n");
		printf("Meta Information about the requested index was not found\n");
	}
	else
	{
		
		print_debug("Meta Information about the requested was found\n");
		printf("Meta Information requested:\n");
		print_indexV2(exist);
	}
	return 0;
}

int deleteKey(GTree *tree, char index[])
{
	// doing
	gboolean deleted;
	deleted = g_tree_remove(tree, g_strdup(index)); // search on the tree to see if the key index exists and removes the Node if exists return (True if removed|False if don't exists)
	if (deleted != TRUE)
	{
		print_debug("The Index was not found, so It cannot be deleted\n"); // if the function doesn't found the key index it doesn't remove anything
		printf("The Index was not found, so It cannot be deleted\n");
	}
	else
	{
		print_debug("Index entry deleted\n"); // if the function found the key index deletes the node from the Tree
		printf("Index entry %s deleted\n", index);
	}

	return 0;
}

int searchKeywordByKey(GTree *tree, char index[], char word[])
{
	gpointer exist;
	exist = g_tree_lookup(tree, g_strdup(index)); // search if index exists

	if (exist == NULL)
	{
		printf("Meta Information about the requested index was not found\n");
		return -1;
	}

	Index *idx = (Index *)exist; // get index content
	char *path = idx->path; // get path to document

	int grep_wc[2]; // file descriptors used for pipe between child that executes grep and child that executes wc
	int wc_parent[2]; // file descriptors used for pipe between child that executes wc and parent

	if (pipe(grep_wc) < 0) // pipe between child that executes grep and child that executes wc
		perror("pipe failed");
	pid_t pid1 = fork(); 

	if (pid1 < 0)
		perror("Fork failed");
	if (pid1 == 0)
	{
		// redirect stdout to writer descriptor of pipe
		close(grep_wc[0]);
		dup2(grep_wc[1], 1);
		close(grep_wc[1]);

		execlp("grep", "grep", word, path, NULL); // output of grep will be written on pipe
		_exit(1);
	}

	if (pipe(wc_parent) < 0) // pipe between child that executes wc and parent
		perror("pipe failed");
	pid_t pid2 = fork();

	if (pid2 < 0)
		perror("Fork failed");
	if (pid2 == 0)
	{
		// redirect stdin to reader descriptor of pipe
		close(grep_wc[1]);
		dup2(grep_wc[0], 0);
		close(grep_wc[0]);

		// redirect output of wc to writer descriptor of pipe
		close(wc_parent[0]);
		dup2(wc_parent[1], 1);
		close(wc_parent[1]);

		execlp("wc", "wc", "-l", NULL); // output of wc will be written on pipe
		_exit(1);
	}

	close(grep_wc[0]);
	close(grep_wc[1]);

	close(wc_parent[1]);

	char buffer[64];
	ssize_t bytes_read = read(wc_parent[0], buffer, sizeof(buffer) - 1); // get content from child process

	if (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		int count = atoi(buffer);
		printf("Total: %d\n", count);
	}
	else
	{
		perror("Read error");
	}

	return 0;
}

gint findWord(gpointer key, gpointer value, gpointer data)
{
	Index *idx = (Index *)value; // get index content
	DATA_W *info = (DATA_W *)data; // get data content

	char *path = idx->path; // get path to the document
	char *word = info->word; // get word to be searched
	int numProc = info->numProc; // get number of processes

	int fd = open(path, O_RDONLY); // file descriptor of document
	off_t size = lseek(fd, 0, SEEK_END); // size of document
	off_t chunk = size / numProc; // size of each part of the document that will be divided

	pid_t pids[numProc];
	for (int i = 0; i < numProc; i++)
	{
		pids[i] = fork();
		if (pids[i] < 0)
			perror("Fork failed");
		if (pids[i] == 0)
		{
			// CHILD
			int start = i * chunk; // start point of reading
			int end = (i == numProc - 1 ? size : start + chunk + sizeof(word)); // ending point of reading
			int line_size = end - start; // size to be read
			char *line = malloc(line_size); // buffer to save the read content

			lseek(fd, start, SEEK_SET);

			ssize_t n, bytes_read = 0;

			while (bytes_read < line_size && (n = read(fd, line + bytes_read, line_size - bytes_read)) > 0) // reads a chunk of the document
			{
				bytes_read += n;
			}

			if (strstr(line, word)) // checks if the word was found
				_exit(1);
			_exit(-1);
		}
	}

	// PARENT
	int found = 0;
	for (int i = 0; i < numProc; i++) // for each child process, check if word was found
	{
		int status;
		wait(&status);
		if (WIFEXITED(status))
		{
			int rel_val = WEXITSTATUS(status);
			if (rel_val < 255)
				found = 1;
		}
	}

	if (found) // if it was found, add it to the list
	{
		info->indexList = g_list_append(info->indexList, value);
	}

	return 0;
}

int searchKeyword(GTree *tree, char word[], int numProc)
{
	DATA_W info; // struct to save the word to be searched, number of processes and list of indexs
	info.word = word; 
	info.numProc = numProc;
	info.indexList = NULL;

	g_tree_foreach(tree, findWord, &info); // search all indexs that contain word

	printf("[");
	for (GList *l = info.indexList; l != NULL; l = l->next){
		Index *idx = (Index *)l->data;
		if(l->next == NULL) printf("%s", (char *)idx->title);
		else printf("%s, ", (char *)idx->title);
    }
	printf("]\n");
	return 0;
}

gint saveMetaInfoNode(gpointer key, gpointer value, gpointer data)
{
	Index *node = (Index *)value;

	int fd = open(SAVE_FILE, O_CREAT | O_WRONLY | O_APPEND, 0640); // open file where the Index struct is gonna be saved
	if (fd == -1)
	{
		perror("Error opening the file\n");
		return -1;
	}
	ssize_t writed_bytes = write(fd, node, sizeof(Index)); // write on binnary file the meta information about a index
	if (writed_bytes != sizeof(Index))
	{
		perror("Error writing the meta information on the saving file\n");
		close(fd);
		return -1;
	}
	print_debug("Node has been saved on the file\n");
	close(fd);
	return 0;
}

int saveMetaInfo(GTree *tree)
{
	g_tree_foreach(tree, saveMetaInfoNode, NULL); // for each node its called saveMetaInfoNode to save the Index on the binary file
	return 0;
}

int buildMetaInfo(GTree *tree)
{
	int fd = open(SAVE_FILE, O_CREAT | O_RDONLY, 0640); // file descriptor to the save file
	if (fd == -1)
	{
		perror("Error opening the file\n");
		return -1;
	}
	off_t size = lseek(fd, 0, SEEK_END); // to see if the save file has information
	if (size == 0)
	{
		print_debug("Save file without information\n");
		return 0;
	}
	lseek(fd, 0, SEEK_SET); // return to begin of file if the save file has information
	off_t nIn = size / sizeof(Index);
	for (off_t i = 0; i < nIn; i++)
	{

		ssize_t bytes_lidos;
		Index *temp;
		temp = malloc(sizeof(Index));
		if ((bytes_lidos = read(fd, temp, sizeof(Index))) > 0) // for each index is called the indexDocument to construct the tree
		{

			print_debug("indexed one Index\n");
			indexDocument(tree, temp);
		}
	}

	close(fd);
	int fd_clean = open(SAVE_FILE, O_WRONLY | O_TRUNC, 0640); // O_TRUNC cleans the file
	if (fd_clean != -1)
	{
		// file is empty
		print_debug("after building the tree the save file is cleaned\n");
		close(fd_clean);
	}

	print_debug("PRINTING TREE:\n"); // print the tree to know how many nodes it has and the metainformation
	g_tree_foreach(tree, print_index_debug, NULL);

	return 0;
}
