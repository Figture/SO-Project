#include <gtree.h>



gint compare_str(gconstpointer a, gconstpointer b, gpointer user_data) {
    return g_strcmp0((const char *)a, (const char *)b);
}


gint print_index(gpointer key, gpointer value, gpointer data) {
    Index *idx = (Index *)value;
    printf("Title: %s\nAuthor: %s\n", idx->title, idx->authors);
}
