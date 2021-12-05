#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

void draw_preview(GtkWidget *widget, cairo_t *cr, gpointer data) {
    g_print("The address pointed to by data is %p\n",data);
    GHashTable *pointer_passer = (GHashTable *)data;
    g_hash_table_insert(pointer_passer, &KEY_CAIRO_CONTEXT, cr);
    draw_background(NULL, NULL, NULL, data);
}