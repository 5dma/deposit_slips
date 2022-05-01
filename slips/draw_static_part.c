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
 * \sa make_checks_view()
*/
void deposit_amount_edited(GtkCellRendererText *self,
                           gchar *path,
                           gchar *new_text,
                           gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;

    GtkTreeIter iter;
    GtkTreeModel *model;

    GtkTreeView *tree_view = (GtkTreeView *)data_passer->check_tree_view;
    model = gtk_tree_view_get_model(tree_view);

    if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
        float amount_float = atof(new_text);
        gchar *formatted_amount = g_strdup_printf("%3.2f", amount_float);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, CHECK_AMOUNT, formatted_amount, -1);
        g_free(formatted_amount);
    }

    gtk_widget_queue_draw(data_passer->drawing_area);
    
}

/**
 * Callback fired while iterating over all checks.
 * @param model Pointer to the model containing the checks.
 * @param path Path to the current check.
 * @param iter Iterator for the current check.
 * @param data Void pointer to the hash table of passed pointers.
 * @return No value is returned.
 * \sa draw_preview()
*/
gboolean preview_deposit_amounts(GtkTreeModel *model,
                               GtkTreePath *path,
                               GtkTreeIter *iter,
                               gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;

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


    cairo_select_font_face(data_passer->cairo_context, "DejaVuMono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(data_passer->cairo_context, 16);

    /* Get the formatted string corresponding to this check's amount. */
    gfloat current_amount = atof(amount);
    gchar *formatted_total = comma_formatted_amount(current_amount);

    /* Move to the correct position to print the amount such that it is right-aligned. */
    cairo_text_extents_t extents;
    cairo_text_extents(data_passer->cairo_context, formatted_total, &extents);
    cairo_move_to(data_passer->cairo_context, RIGHT_MARGIN_SCREEN - extents.width, (row_number * (extents.height + 7)) + 40);
    cairo_show_text(data_passer->cairo_context, formatted_total);
    g_free(formatted_total);

    /* Increment the total of all amounts in the deposit slip and update its
    value in the hash table of passed pointers. */

    gfloat current_total = data_passer->total_deposit;

    float amount_float = atof((char *)amount);
    current_total += amount_float;

    data_passer->total_deposit = current_total;

    g_free(amount);
    g_free(pathstring);
}