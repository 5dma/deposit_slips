#include <gtk/gtk.h>
#include <math.h>

#include "../constants.h"
#include "../headers.h"

/**
 * @file draw_functions.c
 * @brief Contains a callback to draw the deposit slip's preview.
*/

/**
 * Formats a passed float number into a familiar currency value. For example, takes 51003.34 and formats it into 51,003.34.
 * @param amount Any gfloat less than 999999.99
 * @return The formatted string. 
*/
gchar *comma_formatted_amount(gfloat *number) {
    gfloat amount = *number;
    gchar formatted_amount[100];
    int num;
    if (amount < 1000) {
        num = g_snprintf(formatted_amount, 11, "%.2f", *number);
    } else {
        gdouble first_group = floor(amount / 1000);
        gfloat second_group = amount - (first_group * 1000);
        num = g_snprintf(formatted_amount, sizeof(formatted_amount), "%.0f,%06.2f", first_group, second_group);
    }
    return g_strdup(formatted_amount);
}

/**
 * Callback fired when the `draw` signal is fired to redraw the preview area of the deposit slip.
 * This callback does two things:
 * \li Adds the created `cairo_t` pointer to the has table of pointer passers so that the function can redraw the preview.
 * \li Draws the preview for the first time.
 * 
 * (There is a lot of commonality between this code and the one in draw_page(). However, the commonality was not enough to combine them into a single function.) 
 * @param widget Pointer to the preview area.
 * @param cr Pointer to the Cairo context.
 * @param data Pointer to the hash table of pointers.
 * \sa print_deposit_amounts()
*/
void draw_preview(GtkWidget *widget, cairo_t *cr, gpointer data) {

    GHashTable *pointer_passer = (GHashTable *)data;

    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkListStore *checks_store = GTK_LIST_STORE(g_hash_table_lookup(pointer_passer, &KEY_CHECKS_STORE));
    GtkWidget *accounts_tree = GTK_WIDGET(g_hash_table_lookup(pointer_passer, &KEY_CHECKS_ACCOUNTS_TREEVIEW));
    GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(accounts_tree));

    gchar *routing_number = NULL;
    gchar *account_number = NULL;
    gchar *account_name = NULL;

    if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter,
                           ACCOUNT_NUMBER, &account_number,
                           ACCOUNT_NAME, &account_name,
                           ROUTING_NUMBER, &routing_number,
                           -1);
    }

    guint width = gtk_widget_get_allocated_width(widget);
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

    /* Write Date */
    cairo_set_font_size(cr, 5);
    cairo_move_to(cr, 27, 83);
    cairo_show_text(cr, "Date");

    GDateTime *date_time = g_date_time_new_now_local();
    gchar *date_time_string = g_date_time_format(date_time, "%B %e, %Y");
    cairo_move_to(cr, 127, 83);
    cairo_show_text(cr, date_time_string);
    g_free(date_time_string);

    /* Write Routing number */
    gchar *routing_with_transit = g_strconcat("T", routing_number, "T", NULL);
    cairo_select_font_face(cr, "MICR", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 10);
    cairo_move_to(cr, 60, 103);
    cairo_show_text(cr, routing_with_transit);
    g_free(routing_with_transit);

    /* Write account number */
    gchar *account_with_transit = g_strconcat(account_number, "O009", NULL);
    cairo_set_font_size(cr, 10);
    cairo_move_to(cr, 160, 103);
    cairo_show_text(cr, account_with_transit);
    g_free(account_with_transit);

    /* Check if any checks exist in the view. */

    gboolean checks_exist = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(checks_store), &iter);

    gboolean *at_least_one_check = (gboolean *)(g_hash_table_lookup(pointer_passer, &KEY_AT_LEAST_ONE_CHECK));

    /* If any checks exist:
       a) Go print the deposit amounts of all existing checks.
       b) Compute the total for all existing chekcs.
       c) Print the total.
    */

    if (*at_least_one_check == TRUE) {  // We need to get rid of this variable at_least_one_check because the UI should enforce at least one check.
        g_hash_table_insert(pointer_passer, &KEY_CAIRO_CONTEXT, cr);
        gtk_tree_model_foreach(GTK_TREE_MODEL(checks_store), print_deposit_amounts, pointer_passer);

        /* Write total of checks deposited */
        gfloat *current_total = (gfloat *)g_hash_table_lookup(pointer_passer, &KEY_TOTAL_DEPOSIT);

        /* Format the total string with thousands separators. There is similar code in
      draw_static_part.c:print_deposit_amounts that should be put into a function. */
        cairo_set_font_size(cr, 15);
        cairo_move_to(cr, 270, 83);
        cairo_show_text(cr, comma_formatted_amount(current_total));
    }

    g_free(account_name);
    g_free(routing_number);
    g_free(account_number);

    GRand *seed = g_rand_new();
    g_print("I am in draw_preview at random number %d\n", g_rand_int(seed));
}
