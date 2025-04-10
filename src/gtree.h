#include <glib.h>

typedef struct index{
	char title[200];
	char authors[200];
	char path[64];
	int year;
} Index;

gint compare_str(gconstpointer a, gconstpointer b, gpointer user_data);

gint print_index(gpointer key, gpointer value, gpointer data);