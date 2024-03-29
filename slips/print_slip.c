#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

/**
 * @file print_slip.c
 * @brief Prints the actual deposit slip.
 */

/**
 * Callback fired while iterating over all checks. This function prints the first two checks in the store on the front side of the deposit slip.
 * @param model Pointer to the model containing the checks.
 * @param path Path to the current check.
 * @param iter Iterator for the current check.
 * @param data Pointer to user data.
 * @return `FALSE` while we are printing the first two checks, `TRUE` if we were passed the third check, thereby halting the iteration.
 * \sa draw_page()
 * \sa print_deposit_amounts_back()
 */
gboolean print_deposit_amounts_front(GtkTreeModel *model,
                                     GtkTreePath *path,
                                     GtkTreeIter *iter,
                                     gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;
    cairo_t *cr = data_passer->cairo_context;
    gchar *amount;
    gtk_tree_model_get(model, iter, CHECK_AMOUNT, &amount, -1);
    gchar *pathstring = gtk_tree_path_to_string(path); /* Memory freed below. */

    guint64 row_number;
    GError *gerror = NULL;

    /* First, determine the row number in the list of checks for the current check. */
    gboolean string_to_int = g_ascii_string_to_unsigned(
        pathstring,  /* path of the current row */
        10,          /* Base 10 */
        0,           /* minimum value */
        100,         /* maximum value */
        &row_number, /* returned row number */
        &gerror);    /* pointer for GError *. */

    /* Stop rendering of checks after the second one in the store. */
    if (row_number > 1) {
        return TRUE;
    }

    cairo_save(cr); /* Save passed context */

    /* The current amount needs to be printed at a particular coordinate
    in the preview. The horizontal coordinate is fixed, but the vertical coordinate
    changes depending on index of the current check in the list. The farther down
    the current check is, the farther down it is in the preview. The vertical coordinate
    is therefore a function of the `path` passed to the callback. */

    cairo_select_font_face(cr, data_passer->font_family_mono, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, data_passer->font_size_monospace);

    /* Get the formatted string corresponding to this check's amount. */
    gfloat current_amount = atof(amount);
    gchar *formatted_amount = comma_formatted_amount(current_amount); /* Memory freed below. */

    /* Move to the correct position to print the amount such that it is right-aligned. */
    cairo_text_extents_t extents;
    cairo_text_extents(cr, formatted_amount, &extents);
    Coordinates *coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "front_values");
    cairo_move_to(cr, (row_number * coordinates->x) + coordinates->y, extents.width + data_passer->right_margin_print_front);

    cairo_save(cr); /* Save context 1 */
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, formatted_amount);
    cairo_restore(cr); /* Restore context 0 */

    g_free(formatted_amount);

    /* Increment the total of all amounts in the deposit slip and update its
    value in the hash table of passed pointers. */

    float amount_float = atof((char *)amount);

    data_passer->total_deposit += amount_float;

    g_free(amount);
    g_free(pathstring);

    cairo_save(cr); /* Restore passed context */
}

/**
 * Callback fired while iterating over all checks. This function prints the third through last checks (if any) and subtotal on the back side of the deposit slip.
 * @param model Pointer to the model containing the checks.
 * @param path Path to the current check.
 * @param iter Iterator for the current check.
 * @param data Pointer to user data.
 * @return Always returns `FALSE` as we want to examine every check in the store.
 * \sa draw_page()
 * \sa print_deposit_amounts_front()
 */
gboolean print_deposit_amounts_back(GtkTreeModel *model,
                                    GtkTreePath *path,
                                    GtkTreeIter *iter,
                                    gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;
    cairo_t *cr = data_passer->cairo_context;
    gchar *amount;
    gtk_tree_model_get(model, iter, CHECK_AMOUNT, &amount, -1);
    gchar *pathstring = gtk_tree_path_to_string(path); /* Memory freed below. */

    guint64 row_number;
    GError *gerror = NULL;

    /* First, determine the row number in the list of checks for the current check. */
    gboolean string_to_int = g_ascii_string_to_unsigned(
        pathstring,  /* path of the current row */
        10,          /* Base 10 */
        0,           /* minimum value */
        100,         /* maximum value */
        &row_number, /* returned row number */
        &gerror);    /* pointer for GError *. */

    /* Ignore rendering of checks before the second one in the store. */
    if (row_number < 2) {
        return FALSE;
    }

    cairo_save(cr); /* Save passed context */

    /* The current amount needs to be printed at a particular coordinate
    in the preview. The horizontal coordinate is fixed, but the vertical coordinate
    changes depending on index of the current check in the list. The farther down
    the current check is, the farther down it is in the preview. The vertical coordinate
    is therefore a function of the `path` passed to the callback. */

    cairo_select_font_face(cr, data_passer->font_family_mono, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, data_passer->font_size_monospace);

    /* Get the formatted string corresponding to this check's amount. */
    gfloat current_amount = atof(amount);
    gchar *formatted_amount = comma_formatted_amount(current_amount); /* Memory freed below. */

    /* Move to the correct position to print the amount such that it is right-aligned. */
    cairo_text_extents_t extents;
    cairo_text_extents(cr, formatted_amount, &extents);
    Coordinates *coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "back_values");
    cairo_move_to(cr, data_passer->right_margin_print_back - extents.width, (row_number * coordinates->x) + coordinates->y);
    cairo_show_text(cr, formatted_amount);
    g_free(formatted_amount);
    g_free(amount);
    g_free(pathstring);

    cairo_restore(cr); /* Restore passed context */
    return FALSE;
}

/**
 * Callback fired when a print job prints a page. It calls functions to print the front and the back of the deposit slip. (There is a lot of commonality between this code and the one in draw_preview(). However, the commonality was not enough to combine them into a single function.)
 *
 * @param self A Gtk print operation.
 * @param context A GTK print context.
 * @param page_nr Current page number.
 * @param data Pointer to user data.
 * \sa print_deposit_amounts_front()
 * \sa print_deposit_amounts_back()
 */
void draw_page(GtkPrintOperation *self, GtkPrintContext *context, gint page_nr, gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;

    gchar *routing_number = NULL;
    gchar *account_number = NULL;
    gchar *account_name = NULL;

    GtkTreeIter iter;
    GtkTreeModel *model;

    GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data_passer->checks_accounts_treeview));

    if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter,
                           ACCOUNT_NUMBER, &account_number,
                           ACCOUNT_NAME, &account_name,
                           ROUTING_NUMBER, &routing_number,
                           -1);
    }

    gdouble width, height;
    gint layout_height;
    Coordinates *coordinates;

    cairo_t *cr;
    cr = gtk_print_context_get_cairo_context(context);
    width = gtk_print_context_get_width(context);
    height = gtk_print_context_get_height(context);

    cairo_select_font_face(cr, data_passer->font_family_sans, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, data_passer->font_size_sans_serif);
    cairo_translate(cr, 208, 0);
    cairo_save(cr); /* Save context 0 */

    /* Write Name Label */
    if (data_passer->print_name_account_labels) {
        coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "name_label");
        cairo_move_to(cr, coordinates->x, coordinates->y);
        cairo_save(cr); /* Save context 1 */
        cairo_rotate(cr, -G_PI_2);
        cairo_set_font_size(cr, data_passer->font_size_sans_serif * data_passer->font_size_static_label_scaling);
        cairo_show_text(cr, "Name");
        cairo_restore(cr); /* Restore context 0 */
    }

    /* Write Name value */
    coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "name_value");
    cairo_move_to(cr, coordinates->x, coordinates->y);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, data_passer->font_size_sans_serif);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_name);
    cairo_restore(cr); /* Restore context 0 */

    /* Write Account Label */
    if (data_passer->print_name_account_labels) {
        coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "account_label");
        cairo_move_to(cr, coordinates->x, coordinates->y);
        cairo_save(cr); /* Save context 1 */
        cairo_rotate(cr, -G_PI_2);
        cairo_set_font_size(cr, data_passer->font_size_sans_serif * data_passer->font_size_static_label_scaling);
        cairo_show_text(cr, "Account No");
        cairo_restore(cr); /* Restore context 0 */
    }
    /* Write Account Value */

    coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "account_value");
    cairo_move_to(cr, coordinates->x, coordinates->y);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, data_passer->font_size_sans_serif);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_number);
    cairo_restore(cr); /* Restore context 0 */

    /* Write date */

    coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "date_value");
    GDateTime *date_time = g_date_time_new_now_local();
    gchar *date_time_string = g_date_time_format(date_time, "%B %e, %Y");
    cairo_move_to(cr, coordinates->x, coordinates->y);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, data_passer->font_size_sans_serif);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, date_time_string);
    cairo_restore(cr); /* Restore context 0 */
    g_free(date_time_string);

    /* Write Account in MICR */
    gchar *account_with_transit = g_strconcat(account_number, "O", NULL);
    coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "micr_account_value");
    cairo_move_to(cr, coordinates->x, coordinates->y);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, data_passer->font_size_sans_serif);
    cairo_select_font_face(cr, data_passer->font_face_micr, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, account_with_transit);
    cairo_restore(cr); /* Restore context 0 */
    g_free(account_with_transit);

    data_passer->cairo_context = cr;
    data_passer->total_deposit = 0;

    gtk_tree_model_foreach(GTK_TREE_MODEL(data_passer->checks_store), print_deposit_amounts_front, data_passer);

    /* Write total of checks deposited */
    data_passer->total_deposit += data_passer->total_back_side;

    /* Get the width of the total amount, and move to that point to print the total. */
    cairo_text_extents_t extents;
    gchar *formatted_total = comma_formatted_amount(data_passer->total_deposit);
    coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "total_value");
    cairo_text_extents(cr, formatted_total, &extents);
    cairo_move_to(cr, coordinates->x, extents.width + data_passer->right_margin_print_front);
    cairo_save(cr); /* Save context 1 */
    cairo_set_font_size(cr, data_passer->font_size_monospace);
    cairo_select_font_face(cr, data_passer->font_family_mono, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_rotate(cr, -G_PI_2);
    cairo_show_text(cr, formatted_total);
    cairo_restore(cr); /* Restore context 0 */
    g_free(formatted_total);

    cairo_restore(cr); /* Trash context 0 */

    /* If there are more than two checks, print their subtotal on the front side. */
    if (number_of_checks(data_passer) > 2) {
        gchar *formatted_total = comma_formatted_amount(data_passer->total_back_side);
        coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "back_side_subtotal_on_front");
        /* Move to the correct position to print the amount such that it is right-aligned. */
        cairo_text_extents_t extents;
        cairo_text_extents(data_passer->cairo_context, formatted_total, &extents);

        cairo_move_to(cr, coordinates->x, extents.width + data_passer->right_margin_print_front);

        cairo_save(cr);
        cairo_rotate(cr, -G_PI_2);
        cairo_show_text(data_passer->cairo_context, formatted_total);
        cairo_restore(cr); /* Restore context 0 */

        /* Print the rear side */
        gtk_print_operation_draw_page_finish(self);
        gtk_tree_model_foreach(GTK_TREE_MODEL(data_passer->checks_store), print_deposit_amounts_back, data_passer);

        /* Print subtotal on back side. */
        coordinates = (Coordinates *)g_hash_table_lookup(data_passer->layout, "back_side_subtotal");
        cairo_move_to(cr, data_passer->right_margin_print_back - extents.width, coordinates->y);
        cairo_show_text(data_passer->cairo_context, formatted_total);
        g_free(formatted_total);
    }
}

/**
 * Callback fired when user clicks the print button. This function instantiates a GTK Print Operation, applies settings, and associates a callback draw_page().
 * @param self Pointer to the clicked button.
 * @param data Pointer to user data.
 */
void print_deposit_slip(GtkButton *self, gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;
    GtkPrintOperation *operation;
    GError *error;
    gint res;

    operation = gtk_print_operation_new();

    gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
    gtk_print_operation_set_use_full_page(operation, TRUE);
    //  gtk_print_operation_set_allow_async(operation, TRUE);
    gtk_print_operation_set_n_pages(operation, 1);

    GtkPrintSettings *settings = gtk_print_settings_new();

    gtk_print_operation_set_print_settings(operation, settings);

    g_signal_connect(G_OBJECT(operation), "draw_page", G_CALLBACK(draw_page), data_passer);

    res = gtk_print_operation_run(operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW(data_passer->application_window), &error);
    switch (res) {
        case GTK_PRINT_OPERATION_RESULT_ERROR:
            g_print("Error\n");
            break;
        case GTK_PRINT_OPERATION_RESULT_APPLY:
            g_print("Apply\n");
            if (settings != NULL)
                g_object_unref(settings);
            settings = g_object_ref(gtk_print_operation_get_print_settings(operation));

            break;
        case GTK_PRINT_OPERATION_RESULT_CANCEL:
            g_print("Cancel\n");
            break;
        case GTK_PRINT_OPERATION_RESULT_IN_PROGRESS:
            g_print("In progress\n");
            break;
        default:
    }
    g_object_unref(operation);
}