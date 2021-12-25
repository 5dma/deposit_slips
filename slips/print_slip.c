#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

/**
 * @file print_slip.c
 * @brief Prints the actual deposit slip.
*/


/**
 * Callback fired while iterating over all checks.
 * @param model Pointer to the model containing the checks.
 * @param path Path to the current check.
 * @param iter Iterator for the current check.
 * @param data Void pointer to the hash table of passed pointers.
 * @return No value is returned.
 * \sa draw_preview()
*/
gboolean print_deposit_amounts(GtkTreeModel *model,
                               GtkTreePath *path,
                               GtkTreeIter *iter,
                               gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;
    cairo_t *cr = (cairo_t *)g_hash_table_lookup(pointer_passer, &KEY_CAIRO_CONTEXT);
    gchar *amount;
    gtk_tree_model_get(model, iter, CHECK_AMOUNT, &amount, -1);
    gchar *pathstring = gtk_tree_path_to_string(path); /* Memory freed below. */

    guint64 row_number;
    GError *gerror = NULL;

    cairo_save(cr); /* Save passed context */

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


    cairo_select_font_face(cr, "DejaVuSansMono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, FONT_SIZE_AMOUNT);

    /* Get the formatted string corresponding to this check's amount. */
    gfloat current_amount = atof(amount);
    gchar *formatted_amount = comma_formatted_amount(&current_amount); /* Memory freed below. */

    /* Move to the correct position to print the amount such that it is right-aligned. */
    cairo_text_extents_t extents;
    cairo_text_extents(cr, formatted_amount, &extents);
    cairo_move_to(cr, (row_number * 22.5) + 40.5, extents.width + RIGHT_MARGIN_PRINT);
    
    cairo_save(cr); /* Save context 1 */
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, formatted_amount);
    cairo_restore(cr); /* Restore context 0 */

    g_free(formatted_amount);

    /* Increment the total of all amounts in the deposit slip and update its
    value in the hash table of passed pointers. */
    gfloat *total_deposit = (gfloat *)g_hash_table_lookup(pointer_passer, &KEY_TOTAL_DEPOSIT);

    gfloat current_total = *total_deposit;

    float amount_float = atof((char *)amount);
    current_total += amount_float;

    *total_deposit = current_total;

    g_free(amount);
    g_free(pathstring);

    cairo_save(cr); /* Restore passed context */

}


/**
 * Callback fired when a print job prints a page. Prints the physical deposit slip. (There is a lot of commonality between this code and the one in draw_preview(). However, the commonality was not enough to combine them into a single function.) 
*/
void draw_page(GtkPrintOperation *self, GtkPrintContext *context, gint page_nr, gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;

    gchar *routing_number = NULL;
    gchar *account_number = NULL;
    gchar *account_name = NULL;

    GtkTreeIter iter;
    GtkTreeModel *model;

    GtkWidget *accounts_tree = GTK_WIDGET(g_hash_table_lookup(pointer_passer, &KEY_CHECKS_ACCOUNTS_TREEVIEW));
    GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(accounts_tree));

    if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter,
                           ACCOUNT_NUMBER, &account_number,
                           ACCOUNT_NAME, &account_name,
                           ROUTING_NUMBER, &routing_number,
                           -1);
    }

    gdouble width, height;
    gint layout_height;

    cairo_t *cr;
    cr = gtk_print_context_get_cairo_context(context);
    width = gtk_print_context_get_width(context);
    height = gtk_print_context_get_height(context);

    cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 7);
    cairo_translate(cr, 208, 0);
    cairo_save(cr); /* Save context 0 */

    /* Write Name */

    cairo_move_to(cr, 18, 405);
    cairo_save(cr); /* Save context 1 */
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, "Name");
    cairo_restore(cr); /* Restore context 0 */

    cairo_move_to(cr, 18, 373);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, FONT_SIZE_PRINT_DYNAMIC);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_name);
    cairo_restore(cr); /* Restore context 0 */

    /* Write Account Number */

    cairo_move_to(cr, 41, 405);
    cairo_save(cr); /* Save context 1 */
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, "Account No");
    cairo_restore(cr); /* Restore context 0 */

    cairo_move_to(cr, 41, 351);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, FONT_SIZE_PRINT_DYNAMIC);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_number);
    cairo_restore(cr); /* Restore context 0 */

    /* Write date */

    GDateTime *date_time = g_date_time_new_now_local();
    gchar *date_time_string = g_date_time_format(date_time, "%B %e, %Y");
    cairo_move_to(cr, 75, 371);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, FONT_SIZE_PRINT_DYNAMIC);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, date_time_string);
    cairo_restore(cr); /* Restore context 0 */
    g_free(date_time_string);

    /* Write Account in MICR */
    gchar *account_with_transit = g_strconcat(account_number, "O", NULL);

    cairo_move_to(cr, 178, 288);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, FONT_SIZE_PRINT_DYNAMIC);
    cairo_select_font_face(cr, "MICR", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_with_transit);
    cairo_restore(cr); /* Restore context 0 */
    g_free(account_with_transit);

    g_hash_table_insert(pointer_passer, &KEY_CAIRO_CONTEXT, cr);
    
    GtkListStore *checks_store = GTK_LIST_STORE(g_hash_table_lookup(pointer_passer, &KEY_CHECKS_STORE));
    gtk_tree_model_foreach(GTK_TREE_MODEL(checks_store), print_deposit_amounts, pointer_passer);

    /* Write total of checks deposited */
    gfloat *current_total = (gfloat *)g_hash_table_lookup(pointer_passer, &KEY_TOTAL_DEPOSIT);

    /* Get the width of the total amount, and move to that point to print the total. */
    cairo_text_extents_t extents;
    gchar *formatted_total = comma_formatted_amount(current_total);
    cairo_text_extents(cr, formatted_total, &extents);
    cairo_move_to(cr, 153, extents.width + RIGHT_MARGIN_PRINT);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, FONT_SIZE_AMOUNT);
    cairo_select_font_face(cr, "DejaVuSansMono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, formatted_total);
    cairo_restore(cr); /* Restore context 0 */
    g_free(formatted_total);

    cairo_restore(cr); /* Trash context 0 */
}

/**
 * Callback fired when user clicks the print button.
 * @param self Pointer to the clikced button.
 * @param data to the passed hash table.
 * \sa draw_page()
 * 
*/
void print_deposit_slip(GtkButton *self, gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;
    GtkWidget *application_window = GTK_WIDGET(g_hash_table_lookup(pointer_passer, &KEY_APPLICATION_WINDOW));
    GtkPrintOperation *operation;
    GError *error;
    gint res;

    operation = gtk_print_operation_new();

    gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
    gtk_print_operation_set_use_full_page(operation, TRUE);
    gtk_print_operation_set_allow_async(operation, TRUE);
    gtk_print_operation_set_n_pages(operation, 1);

    g_signal_connect(G_OBJECT(operation), "draw_page", G_CALLBACK(draw_page), pointer_passer);

    res = gtk_print_operation_run(operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW(application_window), &error);
    switch (res) {
        case GTK_PRINT_OPERATION_RESULT_ERROR:
            g_print("Error\n");
            break;
        case GTK_PRINT_OPERATION_RESULT_APPLY:
            g_print("Apply\n");
            break;
        case GTK_PRINT_OPERATION_RESULT_CANCEL:
            g_print("Cancel\n");
            break;
        case GTK_PRINT_OPERATION_RESULT_IN_PROGRESS:
            g_print("In progress\n");
            break;
        default:
            g_print("Default\n");
    }
    g_object_unref(operation);
}