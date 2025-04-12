#include "services.h"

gint compare_str(gconstpointer a, gconstpointer b, gpointer user_data)
{
	return g_strcmp0((const char *)a, (const char *)b);
}

gint print_index(gpointer key, gpointer value, gpointer data)
{
	Index *idx = (Index *)value;
	printf("Title: %s\nAuthor: %s\n", idx->title, idx->authors);
	return 0;
}
gint print_indexV2(gpointer value)
{ // Fiz esta versão porque nao percebi a do Pedro Mo ||João Oliveira
	Index *idx = (Index *)value;
	printf("Title: %s\nAuthor: %s\nYear: %d\nPath: %s\n", idx->title, idx->authors, idx->year, idx->path);
	return 0;
}

int indexDocument(GTree *tree, Index *in)
{
	// TO DO
	g_tree_insert(tree, g_strdup(in->title), in);
	printf("Indexed Successfully\n");
	return 0;
}

int checkKey(GTree *tree, char index[])
{
	// Doing
	gpointer exist;
	exist = g_tree_lookup(tree, g_strdup(index));

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
	deleted = g_tree_remove(tree, g_strdup(index));
	if (deleted != TRUE)
	{
		printf("The Index was not found, so It cannot be deleted"); // Confirmar ingles trolha
	}
	else
	{
		printf("Index entry %s deleted\n", index);
	}

	return 0;
}

int searchKeywordByKey(GTree *tree, char index[], char word[])
{
	gpointer exist;
	exist = g_tree_lookup(tree, g_strdup(index));

	if(exist == NULL){
		printf("Meta Information about the requested index was not found\n"); 
		return -1;
	}

	Index *idx = (Index *)exist;
	char *path = idx->path;

	int grep_wc[2];
	int wc_parent[2];

	if(pipe(grep_wc)<0) perror("pipe failed");
	pid_t pid1 = fork();

	if(pid1 < 0) perror("Fork failed");
	if(pid1 == 0){
		close(grep_wc[0]);
		dup2(grep_wc[1],1);
		close(grep_wc[1]);
		
		execlp("grep","grep",word,path,NULL);
		_exit(1);
	}

	if(pipe(wc_parent)<0) perror("pipe failed");
	pid_t pid2 = fork();

	if(pid2 < 0) perror("Fork failed");
	if(pid2 == 0){
		close(grep_wc[1]);
		dup2(grep_wc[0],0);
		close(grep_wc[0]);

		close(wc_parent[0]);
		dup2(wc_parent[1],1);
		close(wc_parent[1]);
		
		execlp("wc","wc","-l",NULL);
		_exit(1);
	}

	close(grep_wc[0]);
	close(grep_wc[1]);

	close(wc_parent[1]);

	char buffer[64];
    ssize_t bytes_read = read(wc_parent[0], buffer, sizeof(buffer)-1);
	
    if(bytes_read > 0){
        buffer[bytes_read] = '\0';
        int count = atoi(buffer);
        printf("Total: %d\n", count);
    }else{
        perror("Read error");
    }

	return 0;
}

gint findWord(gpointer key, gpointer value, gpointer data){
	Index *idx = (Index *)value;
	DATA_W *info = (DATA_W *)data;

	char *path = idx->path;
	char *word = info->word;
 
	int fd = open(path, O_RDONLY);
	off_t size = lseek(fd,0,SEEK_END);
	off_t chunk = size / NUM_PROC;

	pid_t pids[NUM_PROC];
	for(int i=0;i<NUM_PROC;i++){
		pids[i] = fork();
		if(pids[i] < 0) perror("Fork failed");
		if(pids[i] == 0){
			// CHILD
			int start = i*chunk;
			int end = (i == NUM_PROC-1 ? size : start+chunk+sizeof(word));
			int line_size = end-start;
			char *line = malloc(line_size);

			lseek(fd,start,SEEK_SET);

			ssize_t n, bytes_read = 0;

			while(bytes_read < line_size && (n = read(fd,line+bytes_read,line_size-bytes_read)) > 0){
				bytes_read += n;
			}

			if(strstr(line,word)) _exit(1);
			_exit(-1);
		}
	}

	// PARENT
	int found=0;
	for(int i=0;i<NUM_PROC;i++){
		int status; wait(&status);
		if(WIFEXITED(status)){
			int rel_val = WEXITSTATUS(status);
			if(rel_val < 255) found=1;
		}
	}

	if(found){
		printf("Found at: \n");
		print_index(key,value,data);
	}

	return 0;
}

int searchKeyword(GTree *tree, char word[])
{
	DATA_W info; info.word = word;
	g_tree_foreach(tree, findWord, &info);
	
	return 0;
}
