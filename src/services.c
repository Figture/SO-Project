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
	printf("Title: %s\nAuthor: %s\nYear: %d\nPath: %s", idx->title, idx->authors, idx->year, idx->path);
	return 0;
}

int indexDocument(GTree *tree, Index *in)
{
	// TO DO
	g_tree_insert(tree, g_strdup(in->title), in);
	printf("Indexed Successfully\n");
	return 0;
}

int checkKey(GTree *tree, char *index[])
{
	// Doing
	gpointer exist;
	exist = g_tree_lookup_node(tree, g_strdup(index));

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

int deleteKey(GTree *tree, char *index[])
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

int searchKeywordByKey()
{
	// TO DO
	return 0;
}

int searchKeyword()
{
	// TO DO
	return 0;
}
