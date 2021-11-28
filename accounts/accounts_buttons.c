#include <gtk/gtk.h>
#include <string.h>

#include "../constants.h"
#include "../headers.h"
/**
 * @file accounts_buttons.c
 * @brief Builds the view and controller for the four buttons in the Accounts tab.
*/

/**
    Adds a row to the tree model after user clicks the Add button.
    @param widget Pointer to the clicked Add button.
    @param data Void pointer to the temporary list store.
*/
static void add_row(GtkWidget *widget, gpointer data) {
    GtkListStore *list_store = (GtkListStore *)data;

    GtkTreeIter iter;
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set(list_store, &iter,
                       ACCOUNT_NUMBER, NEW_NUMBER,
                       ACCOUNT_NAME, NEW_NAME,
                       DESCRIPTION, NEW_DESCRIPTION,
                       CHECKBOX, FALSE,
                       -1);
    GtkWidget *accounts_buttons_hbox = gtk_widget_get_parent(widget);
    GtkWidget *account_button_revert = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_REVERT);
    gtk_widget_set_sensitive(account_button_revert, TRUE);
    GtkWidget *account_button_save = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_SAVE);
    gtk_widget_set_sensitive(account_button_save, TRUE);
}

/**
    Deletes a checked row from the model (and reflected in the treeview) after user clicks the Delete button. Inspiration for
this loop is from https://www.kksou.com/php-gtk2/sample-codes/iterate-through-a-GtkListStore-Part-2.php.
    @param widget Pointer to the clicked Delete button.
    @param data Void pointer to the temporary list store.
*/
static void delete_row(GtkWidget *widget, gpointer data) {
    GtkListStore *list_store = (GtkListStore *)data;
    GtkTreeIter iter;
    GValue *gvalue;
    gboolean still_in_list = TRUE;
    gboolean removed_last_row = FALSE;
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data), &iter);

    do {
        gtk_tree_model_get_value(GTK_TREE_MODEL(data), &iter, CHECKBOX, gvalue);
        if (g_value_get_boolean(gvalue) == TRUE) {
            removed_last_row = !gtk_list_store_remove(list_store, &iter);
        }
        g_value_unset(gvalue);
        if (!removed_last_row) {
            still_in_list = gtk_tree_model_iter_next(GTK_TREE_MODEL(data), &iter);
        }

    } while (still_in_list && !removed_last_row);

    GtkWidget *accounts_buttons_hbox = gtk_widget_get_parent(widget);
    GtkWidget *account_button_revert = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_REVERT);
    gtk_widget_set_sensitive(account_button_revert, TRUE);
    GtkWidget *account_button_save = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_SAVE);
    gtk_widget_set_sensitive(account_button_save, TRUE);
}

/**
    Reverts the account listing in the Accounts tab to the last saved listing after user clicks the Revert button.

    The listing in the Accounts tab reflects what is in the temporary store. The master store retains
    the listing that was read from disk or last written to disk. This function clears the temporary
    store, and then copies the contents of the master store into the temporary store.
    @param widget Pointer to the clicked button.
    @param data Void pointer to the structure holding both stores, master and temporary.
*/
static void revert_listing(GtkWidget *widget, gpointer data) {
    GPtrArray *list_store_ptr_array = (GPtrArray *)data;

    GtkListStore *list_store_master = g_ptr_array_index(list_store_ptr_array, POSITION_LIST_STORE_MASTER);

    GtkListStore *list_store_temporary = g_ptr_array_index(list_store_ptr_array, POSITION_LIST_STORE_TEMPORARY);
    gtk_list_store_clear(list_store_temporary);

    GtkTreeIter iter_master;
    GtkTreeIter iter_temporary;

    gboolean found_first_iter_master = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store_master), &iter_master);

    if (found_first_iter_master) {
        /* gchar pointers for reading the entries from the master store */
        gchar *account_number = NULL;
        gchar *account_name = NULL;
        gchar *account_description = NULL;

        while (gtk_list_store_iter_is_valid(list_store_master, &iter_master)) {
            gtk_tree_model_get(GTK_TREE_MODEL(list_store_master), &iter_master,
                               ACCOUNT_NUMBER, &account_number,
                               ACCOUNT_NAME, &account_name,
                               DESCRIPTION, &account_description,
                               -1);

            /* Add the entry from the master store to the temporary store */
            gtk_list_store_append(list_store_temporary, &iter_temporary);

            gtk_list_store_set(list_store_temporary, &iter_temporary,
                               ACCOUNT_NUMBER, account_number,
                               ACCOUNT_NAME, account_name,
                               DESCRIPTION, account_description,
                               -1);

            gtk_tree_model_iter_next(GTK_TREE_MODEL(list_store_master), &iter_master);
        }
        g_free(account_number);
        g_free(account_name);
        g_free(account_description);
    } else {
        g_print("Could not find first iter for reverting the temporary list\n");
    }
}

/**
    Constructs the view for the four buttons in the Accounts tab.
    @param list_store GtkListStore passed in from the main view. This parameter
    is passed to the callbacks for adding, deleting, reverting, and saving changes.
    @return An HBox containing the four buttons and associated callbacks.
*/
GtkWidget *make_accounts_buttons_hbox(GPtrArray *list_store_ptr_array) {
    GtkWidget *local_hbox;

    GtkWidget *account_button_add = gtk_button_new_from_icon_name("gtk-add", GTK_ICON_SIZE_BUTTON);
    GtkWidget *account_button_delete = gtk_button_new_from_icon_name("gtk-delete", GTK_ICON_SIZE_BUTTON);
    GtkWidget *account_button_revert = gtk_button_new_from_icon_name("gtk-refresh", GTK_ICON_SIZE_BUTTON);
    GtkWidget *account_button_save = gtk_button_new_from_icon_name("gtk-save", GTK_ICON_SIZE_BUTTON);

    gtk_widget_set_name(account_button_add, BUTTON_NAME_ADD);
    gtk_widget_set_name(account_button_delete, BUTTON_NAME_DELETE);
    gtk_widget_set_name(account_button_revert, BUTTON_NAME_REVERT);
    gtk_widget_set_name(account_button_save, BUTTON_NAME_SAVE);

    local_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_name(local_hbox, HBOX_ACCOUNT_BUTTONS);
    gtk_box_pack_start(GTK_BOX(local_hbox), account_button_add, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(local_hbox), account_button_delete, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(local_hbox), account_button_revert, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(local_hbox), account_button_save, TRUE, TRUE, 0);

    gtk_widget_set_tooltip_text(account_button_add, "Add account");
    gtk_widget_set_tooltip_text(account_button_delete, "Delete selected accounts");
    gtk_widget_set_tooltip_text(account_button_revert, "Revert");
    gtk_widget_set_tooltip_text(account_button_save, "Save changes");

    gtk_widget_set_sensitive(account_button_add, TRUE);
    gtk_widget_set_sensitive(account_button_delete, FALSE);
    gtk_widget_set_sensitive(account_button_revert, FALSE);
    gtk_widget_set_sensitive(account_button_save, FALSE);

    GtkListStore *list_store_temporary = g_ptr_array_index(list_store_ptr_array, POSITION_LIST_STORE_TEMPORARY);

    g_signal_connect(account_button_add, "clicked", G_CALLBACK(add_row), list_store_temporary);
    g_signal_connect(account_button_delete, "clicked", G_CALLBACK(delete_row), list_store_temporary);
    g_signal_connect(account_button_revert, "clicked", G_CALLBACK(revert_listing), list_store_ptr_array);

    return local_hbox;
}
