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
 * @param self Pointer to the edited cell. This passed value can be `NULL` when calling this function from the first time--when setting up the GTK windows and before the user
 * clicks the add button to add the first check.
 * @param path Path to the edited cell.
 * @param new_text Text in the cell renderer after the editing is complete.
 * @param data Void pointer to the hash table of passed pointers.
*/
void draw_background(GtkCellRendererText *self,
                     gchar *path,
                     gchar *new_text,
                     gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;

    GtkTreeView *accounts_treeview = (GtkTreeView *)g_hash_table_lookup(pointer_passer, &KEY_CHECKS_ACCOUNTS_TREEVIEW);

    /* Get account number, name, and routing number from selected account. */
    GtkTreeSelection *tree_view_selection = gtk_tree_view_get_selection(accounts_treeview);
    GtkTreeModel *account_model;
    GtkTreeIter account_iter;
    gtk_tree_selection_get_selected(tree_view_selection, &account_model, &account_iter);
    gchar *routing_number;
    gchar *account_number;
    gchar *account_name;

    gtk_tree_model_get(account_model, &account_iter,
                       ACCOUNT_NUMBER, &account_number,
                       ACCOUNT_NAME, &account_name,
                       ROUTING_NUMBER, &routing_number,
                       -1);

    GtkWidget *drawing_area = (GtkWidget *)g_hash_table_lookup(pointer_passer, &KEY_DRAWING_AREA);

    cairo_t *cr = (cairo_t *)g_hash_table_lookup(pointer_passer, &KEY_CAIRO_CONTEXT);

    guint width = gtk_widget_get_allocated_width(drawing_area);
    guint height = 0.45 * width;

    cairo_rectangle(cr, 0.0, 0.0, width, height);
    cairo_set_line_width(cr, 5.0);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_text_extents_t te;
    cairo_text_extents(cr, "a", &te);

    /* Write Deposit Ticket */
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 6);
    cairo_move_to(cr, 104, 10);
    cairo_show_text(cr, "DEPOSIT TICKET");

    /* Write Name */
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 6);
    cairo_move_to(cr, 27, 25);
    cairo_show_text(cr, "Name");

    cairo_set_font_size(cr, 16);
    cairo_move_to(cr, 127, 25);
    cairo_show_text(cr, account_name);

    /* Write Account */
    cairo_set_font_size(cr, 6);
    cairo_move_to(cr, 27, 52);
    cairo_show_text(cr, "Account");

    cairo_set_font_size(cr, 16);
    cairo_move_to(cr, 127, 52);
    cairo_show_text(cr, account_number);

    /* Write Date */
    cairo_set_font_size(cr, 5);
    cairo_move_to(cr, 27, 83);
    cairo_show_text(cr, "Date");

    GDateTime *date_time = g_date_time_new_now_local();
    gchar *date_time_string = g_date_time_format (date_time, "%B %e, %Y");
    cairo_move_to(cr, 127, 83);
    cairo_show_text(cr, date_time_string);
    g_print("%s\n",date_time_string);
    g_free(date_time_string);



    /* Write Routing number */
    cairo_select_font_face(cr, "MICR", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 10);
    cairo_move_to(cr, 60, 103);
    cairo_show_text(cr, routing_number);

    /* Write account number */
    cairo_move_to(cr, 160, 103);
    cairo_show_text(cr, account_number);

    /* Draw individual deposit lines */

    GtkTreeView *treeview = GTK_TREE_VIEW(g_hash_table_lookup(pointer_passer, &KEY_CHECK_TREE_VIEW));
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    GtkTreeIter iter;

    if (self != NULL) {
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, CHECK_AMOUNT, new_text, -1);
        }
    }

    /* Check if any checks exist in the view. */
    gboolean checks_exist = gtk_tree_model_get_iter_first(model, &iter);

    /* If any checks exist:
       a) Go print the deposit amounts of all existing checks.
       b) Compute the total for all existing chekcs.
       c) Print the total.
    */
    if (checks_exist) {
        gtk_tree_model_foreach(model, print_deposit_amounts, pointer_passer);
        /* Write total of checks deposited */
        gfloat *current_total = (gfloat *)g_hash_table_lookup(pointer_passer, &KEY_TOTAL_DEPOSIT);
        gchar current_total_string[10];
        g_snprintf(current_total_string, 11, "%.2f", *current_total);

        cairo_set_font_size(cr, 15);
        cairo_move_to(cr, 270, 83);
        cairo_show_text(cr, current_total_string);

        gtk_widget_queue_draw(drawing_area);
    }
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
    /* Move to the horizontal coordinate and the vertical coordinatet corresponding to the
    current row. */
    cairo_move_to(cr, 50, (row_number * 10) + 50);
    g_print("The amountddd is %s and the string is %s\n", amount, pathstring);
    cairo_show_text(cr, amount);

    /* Increment the total of all amounts in the deposit slip and update its
    value in the hash table of passed pointers. */
    gfloat *current_total = (gfloat *)g_hash_table_lookup(pointer_passer, &KEY_TOTAL_DEPOSIT);

    float amount_float = atof((char *)amount);
    *current_total += amount_float;

    g_print("The total amount is %.2f\n", *current_total);

    g_hash_table_replace(pointer_passer, &KEY_TOTAL_DEPOSIT, current_total);
}