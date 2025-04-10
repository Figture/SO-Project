#include "services.h"
#include "defs.h"

int main(int argc, char *argv[]){
	printf("SERVER LAUNCHED\n");
	GTree *indexTree = g_tree_new_full(compare_str, NULL, g_free, g_free);

	// make fifo e o loop com o dummy
	mkfifo(C_TO_S, 0666);
	int fdin = open(C_TO_S, O_RDONLY);
	int dummy_fd = open(C_TO_S, O_WRONLY);
	
	MSG in;
	while (read(fdin, &in, sizeof(MSG))){
		// fazer a concorrencia com N filhos
		if(strcmp(in.flag,"-a")==0){
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

		}else if(strcmp(in.flag,"-c")==0){
			// TO DO
		}else if(strcmp(in.flag,"-d")==0){
			// TO DO
		}else if(strcmp(in.flag,"-l")==0){
			// TO DO
		}else if(strcmp(in.flag,"-s")==0){
			// TO DO
		}

		printf("PRINTING TREE:\n");
		g_tree_foreach(indexTree, print_index, NULL);
		
		//separador por iteracao
		printf("\n--------------------\n");
	}

	close(fdin);
	close(dummy_fd);
	unlink(C_TO_S);
	

	return 0;
}
