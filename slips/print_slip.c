#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"


/**
 * @file print_slip.c
 * @brief Prints the actual deposit slip.
*/

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

    /* Print a rectangle around the deposit slip. Comment out when not testing. */
    cairo_rectangle(cr, 207, 0, 198, 504);
    cairo_set_line_width(cr, 10);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_stroke(cr);

    cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 16);

    cairo_save(cr); /* Save context 0 */
    cairo_translate(cr, 208, 0);

    /* Write Name */
    cairo_move_to(cr, 31, 360);
    cairo_save(cr); /* Save context 1 */
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_name);
    cairo_restore(cr); /* Restore context 1 */

    /* Write Account Number */
    cairo_move_to(cr, 54, 333);
    cairo_save(cr); /* Save context 1 */
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_number);
    cairo_restore(cr); /* Restore context 1 */

    /* Write date */
    GDateTime *date_time = g_date_time_new_now_local();
    gchar *date_time_string = g_date_time_format(date_time, "%B %e, %Y");
    cairo_move_to(cr, 75, 371);
    cairo_save(cr); /* Save context 1 */
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, date_time_string);
    cairo_restore(cr); /* Restore context 1 */
    g_free(date_time_string);


    /* Write Account in MICR */
    gchar *account_with_transit = g_strconcat(account_number, "O", NULL);
    cairo_set_font_size(cr, 10);
    cairo_move_to(cr, 180, 288);
    cairo_save(cr); /* Save context 1 */
    cairo_select_font_face(cr, "MICR", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_with_transit);
    cairo_restore(cr); /* Restore context 1 */
    g_free(account_with_transit);

    cairo_restore(cr); /* Restore context 0 */
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