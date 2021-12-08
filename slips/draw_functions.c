#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

/**
 * @file draw_functions.c
 * @brief Contains a callback to draw the deposit slip's preview.
*/


/**
 *  Callback fired when the `draw` signal is fired to redraw the preview area of the deposit slip.
 * This callback does two things:
 * Adds the created `cairo_t` pointer to the has table of pointer passers so that the function can redraw the preview.
 * Draws the preview for the first time.
 * @param widget Pointer to the preview area.
 * @param cr Pointer to the Cairo context.
 * @param data Pointer to the hash table of pointers.
*/
void draw_preview(GtkWidget *widget, cairo_t *cr, gpointer data) {
    g_print("OMG A\n");
    GHashTable *pointer_passer = (GHashTable *)data;
    g_hash_table_insert(pointer_passer, &KEY_CAIRO_CONTEXT, cr);
    draw_background(NULL, NULL, NULL, data);
}