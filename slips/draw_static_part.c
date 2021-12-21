#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

/**
 * @file draw_static_part.c
 * @brief Contains callbacks to draw the deposit slip's preview.
*/

/**
 * Callback fired each time a check amount is modified. The
 * callback redraws the entire preview. (Would be better if it redraws
 * only the area of the preview affected by the changed check amount.)
 * This callback also formats the amount entered into a decimal format, and stores
 * that formatted string.
 * @param self Pointer to the edited cell. This passed value can be `NULL` when calling this function from the first time--when setting up the GTK windows and before the user
 * clicks the add button to add the first check.
 * @param path Path to the edited cell.
 * @param new_text Text in the cell renderer after the editing is complete.
 * @param data Void pointer to the hash table of passed pointers.
*/
void deposit_amount_edited(GtkCellRendererText *self,
                           gchar *path,
                           gchar *new_text,
                           gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;

    GtkTreeIter iter;
    GtkTreeModel *model;

    GtkTreeView *tree_view = (GtkTreeView *)g_hash_table_lookup(pointer_passer, &KEY_CHECK_TREE_VIEW);
    model = gtk_tree_view_get_model(tree_view);

    if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
        float amount_float = atof(new_text);
        gchar *formatted_amount = g_strdup_printf("%3.2f", amount_float);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, CHECK_AMOUNT, formatted_amount, -1);

    }

    GtkDrawingArea *drawing_area = (GtkDrawingArea *)g_hash_table_lookup(pointer_passer, &KEY_DRAWING_AREA);
    gtk_widget_queue_draw(GTK_WIDGET(drawing_area));

    // g_signal_emit_by_name (G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_preview), pointer_passer);
}

/**
 * Callback fired while iterating over all checks.
 * @param model Pointer to the model containing the checks.
 * @param path Path to the current check.
 * @param iter Iterator for the current check.
 * @param data Void pointer to the hash table of passed pointers.
 * @return No value is returned.
*/
gboolean print_deposit_amounts(GtkTreeModel *model,
                               GtkTreePath *path,
                               GtkTreeIter *iter,
                               gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;
    cairo_t *cr = (cairo_t *)g_hash_table_lookup(pointer_passer, &KEY_CAIRO_CONTEXT);
    gchar *amount;
    gtk_tree_model_get(model, iter, CHECK_AMOUNT, &amount, -1);
    gchar *pathstring = gtk_tree_path_to_string(path);

    guint64 row_number;
    GError *gerror = NULL;

    /* The current amount needs to be printed at a particular coordinate
    in the preview. The horizontal coordinate is fixed, but the vertical coordinate
    changes depending on index of the current check in the list. The farther down
    the current check is, the farther down it is in the preview. The vertical coordinate
    is therefore a function of the `path` passed to the callback. */

    /* First, determine the row number in the list of checks for the current check. */
    gboolean string_to_int = g_ascii_string_to_unsigned(
        pathstring,  /* path of the current row */
        10,          /* Base 10 */
        0,           /* minimum value */
        100,         /* maximum value */
        &row_number, /* returned row number */
        &gerror);    /* pointer for GError *. */

    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
    cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 16);
    /* Move to the horizontal coordinate and the vertical coordinate corresponding to the
    current row. */
    cairo_move_to(cr, 50, (row_number * 10) + 50);
    cairo_show_text(cr, amount);

    /* Increment the total of all amounts in the deposit slip and update its
    value in the hash table of passed pointers. */
    gfloat *total_deposit = (gfloat *)g_hash_table_lookup(pointer_passer, &KEY_TOTAL_DEPOSIT);

    gfloat current_total = *total_deposit;

    float amount_float = atof((char *)amount);
    current_total += amount_float;

    *total_deposit = current_total;

    g_free(amount);
    g_free(pathstring);
}