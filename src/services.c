#include "services.h"

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
	printf("Indexed Successfully\n");
	return 0;
}

int checkKey(GTree *tree, char index[])
{
	// Doing
	gpointer exist;
	exist = g_tree_lookup(tree, g_strdup(index)); // search on the tree to see if the key index exists (returns Node if exists|NULL if don't exists)

	if (exist == NULL)
	{
		printf("Meta Information about the requested index was not found\n");
	}
	else
	{
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
		printf("The Index was not found, so It cannot be deleted"); // if the function doesn't found the key index it doesn't remove anything
	}
	else
	{
		printf("Index entry %s deleted\n", index); // if the function found the key index deletes the node from the Tree
	}

	return 0;
}

int searchKeywordByKey(GTree *tree, char index[], char word[])
{
	gpointer exist;
	exist = g_tree_lookup(tree, g_strdup(index));

	if (exist == NULL)
	{
		printf("Meta Information about the requested index was not found\n");
		return -1;
	}

	Index *idx = (Index *)exist;
	char *path = idx->path;

	int grep_wc[2];
	int wc_parent[2];

	if (pipe(grep_wc) < 0)
		perror("pipe failed");
	pid_t pid1 = fork();

	if (pid1 < 0)
		perror("Fork failed");
	if (pid1 == 0)
	{
		close(grep_wc[0]);
		dup2(grep_wc[1], 1);
		close(grep_wc[1]);

		execlp("grep", "grep", word, path, NULL);
		_exit(1);
	}

	if (pipe(wc_parent) < 0)
		perror("pipe failed");
	pid_t pid2 = fork();

	if (pid2 < 0)
		perror("Fork failed");
	if (pid2 == 0)
	{
		close(grep_wc[1]);
		dup2(grep_wc[0], 0);
		close(grep_wc[0]);

		close(wc_parent[0]);
		dup2(wc_parent[1], 1);
		close(wc_parent[1]);

		execlp("wc", "wc", "-l", NULL);
		_exit(1);
	}

	close(grep_wc[0]);
	close(grep_wc[1]);

	close(wc_parent[1]);

	char buffer[64];
	ssize_t bytes_read = read(wc_parent[0], buffer, sizeof(buffer) - 1);

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
	Index *idx = (Index *)value;
	DATA_W *info = (DATA_W *)data;

	char *path = idx->path;
	char *word = info->word;

	int fd = open(path, O_RDONLY);
	off_t size = lseek(fd, 0, SEEK_END);
	off_t chunk = size / NUM_PROC;

	pid_t pids[NUM_PROC];
	for (int i = 0; i < NUM_PROC; i++)
	{
		pids[i] = fork();
		if (pids[i] < 0)
			perror("Fork failed");
		if (pids[i] == 0)
		{
			// CHILD
			int start = i * chunk;
			int end = (i == NUM_PROC - 1 ? size : start + chunk + sizeof(word));
			int line_size = end - start;
			char *line = malloc(line_size);

			lseek(fd, start, SEEK_SET);

			ssize_t n, bytes_read = 0;

			while (bytes_read < line_size && (n = read(fd, line + bytes_read, line_size - bytes_read)) > 0)
			{
				bytes_read += n;
			}

			if (strstr(line, word))
				_exit(1);
			_exit(-1);
		}
	}

	// PARENT
	int found = 0;
	for (int i = 0; i < NUM_PROC; i++)
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

	if (found)
	{
		printf("Found at: \n");
		print_index(key, value, data);
	}

	return 0;
}

int searchKeyword(GTree *tree, char word[])
{
	DATA_W info;
	info.word = word;
	g_tree_foreach(tree, findWord, &info);

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
	printf("Node has been saved on the file\n");
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
		printf("Save file without information\n");
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

			printf("indexed one Index\n");
			indexDocument(tree, temp);
		}
	}

	close(fd);
	int fd_clean = open(SAVE_FILE, O_WRONLY | O_TRUNC, 0640); // O_TRUNC cleans the file
	if (fd_clean != -1)
	{
		// file is empty
		printf("after building the tree the save file is cleaned\n");
		close(fd_clean);
	}

	printf("PRINTING TREE:\n"); // print the tree to know how many nodes it has and the metainformation
	g_tree_foreach(tree, print_index, NULL);

	return 0;
}
