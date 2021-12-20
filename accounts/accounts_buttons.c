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
                       ROUTING_NUMBER,NEW_ROUTING,
                       CHECKBOX, FALSE,
                       -1);
    GtkWidget *accounts_buttons_hbox = gtk_widget_get_parent(widget);
    GtkWidget *account_button_revert = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_REVERT);
    gtk_widget_set_sensitive(account_button_revert, TRUE);
    GtkWidget *account_button_save = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_SAVE);
    gtk_widget_set_sensitive(account_button_save, TRUE);
}

/**
Deletes a checked row from the model (and reflected in the treeview) after user clicks the Delete button. Inspiration for this loop is from [198. How to iterate through a GtkListStore - Part 2?](https://www.kksou.com/php-gtk2/sample-codes/iterate-through-a-GtkListStore-Part-2.php).
@param widget Pointer to the clicked Delete button.
@param data Void pointer to the temporary list store.
*/
static void delete_row(GtkWidget *widget, gpointer data) {
    GtkListStore *list_store = (GtkListStore *)data;
    GtkTreeIter iter;
    GValue gvalue = G_VALUE_INIT;
    gboolean still_in_list = TRUE;
    gboolean removed_last_row = FALSE;
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data), &iter);

    do {
        gtk_tree_model_get_value(GTK_TREE_MODEL(data), &iter, CHECKBOX, &gvalue);
        if (g_value_get_boolean(&gvalue) == TRUE) {
            removed_last_row = !gtk_list_store_remove(list_store, &iter);
        }
        g_value_unset(&gvalue);
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
    @param data Void pointer to hash table pointer passer. The hash table holds pointers to both stores, master and temporary.
*/
static void revert_listing(GtkWidget *widget, gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;

    GtkListStore *list_store_master = GTK_LIST_STORE( g_hash_table_lookup(pointer_passer, &KEY_LIST_STORE_MASTER));
    GtkListStore *list_store_temporary = GTK_LIST_STORE( g_hash_table_lookup(pointer_passer, &KEY_LIST_STORE_TEMPORARY));

    gtk_list_store_clear(list_store_temporary);

    GtkTreeIter iter_master;
    GtkTreeIter iter_temporary;

    gboolean found_first_iter_master = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store_master), &iter_master);

    if (found_first_iter_master) {
        /* gchar pointers for reading the entries from the master store */
        gchar *account_number = NULL;
        gchar *account_name = NULL;
        gchar *account_description = NULL;
        gchar *routing_number = NULL;

        while (gtk_list_store_iter_is_valid(list_store_master, &iter_master)) {
            gtk_tree_model_get(GTK_TREE_MODEL(list_store_master), &iter_master,
                               ACCOUNT_NUMBER, &account_number,
                               ACCOUNT_NAME, &account_name,
                               DESCRIPTION, &account_description,
                               ROUTING_NUMBER,&routing_number,
                               -1);

            /* Add the entry from the master store to the temporary store */
            gtk_list_store_append(list_store_temporary, &iter_temporary);

            gtk_list_store_set(list_store_temporary, &iter_temporary,
                               ACCOUNT_NUMBER, account_number,
                               ACCOUNT_NAME, account_name,
                               DESCRIPTION, account_description,
                               ROUTING_NUMBER, routing_number,
                               -1);

            gtk_tree_model_iter_next(GTK_TREE_MODEL(list_store_master), &iter_master);
        }
        g_free(account_number);
        g_free(account_name);
        g_free(account_description);
        g_free(routing_number);
        GtkWidget *accounts_buttons_hbox = gtk_widget_get_parent(widget);
        GtkWidget *account_button_revert = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_REVERT);
        gtk_widget_set_sensitive(account_button_revert, FALSE);
        GtkWidget *account_button_save = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_SAVE);
        gtk_widget_set_sensitive(account_button_save, FALSE);
        GtkWidget *account_button_delete = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_DELETE);
        gtk_widget_set_sensitive(account_button_delete, FALSE);
    } else {
        g_print("Could not find first iter for reverting the temporary list\n");
    }
}

/**
    Saves the current listing of accounts (which is the listing in the temporary store) 
    after user clicks the Save button. Also sets the master store to the listing in the
    temporary store.
    @param widget Pointer to the clicked Delete button.
    @param data Void pointer the hash table containing the pointer to the temporary list store.
*/
static void save_listing(GtkWidget *widget, gpointer data) {

    GHashTable *pointer_passer = (GHashTable *)data;

    GtkListStore *list_store_master = GTK_LIST_STORE( g_hash_table_lookup(pointer_passer, &KEY_LIST_STORE_MASTER));
    GtkListStore *list_store_temporary = GTK_LIST_STORE( g_hash_table_lookup(pointer_passer, &KEY_LIST_STORE_TEMPORARY));

    GtkTreeIter iter_master;
    GtkTreeIter iter_temporary;

    gboolean found_first_iter_temporary = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store_temporary), &iter_temporary);

    if (found_first_iter_temporary) {
        /* gchar pointers for reading the entries from the master store */
        gchar *account_number = NULL;
        gchar *account_name = NULL;
        gchar *account_description = NULL;
        gchar *routing_number = NULL;

        GString *string_to_save = g_string_new (NULL);

        gtk_list_store_clear(list_store_master);

        while (gtk_list_store_iter_is_valid(list_store_temporary, &iter_temporary)) {
            gtk_tree_model_get(GTK_TREE_MODEL(list_store_temporary), &iter_temporary,
                               ACCOUNT_NUMBER, &account_number,
                               ACCOUNT_NAME, &account_name,
                               DESCRIPTION, &account_description,
                               ROUTING_NUMBER, &routing_number,
                               -1);

            /* Add the entry from the master store to the temporary store */
            gtk_list_store_append(list_store_master, &iter_master);

            gtk_list_store_set(list_store_master, &iter_master,
                               ACCOUNT_NUMBER, account_number,
                               ACCOUNT_NAME, account_name,
                               DESCRIPTION, account_description,
                               ROUTING_NUMBER, routing_number,
                               -1);

            g_string_append_printf (string_to_save,"%s\t%s\t%s\n", account_number,account_name, account_description);

            gtk_tree_model_iter_next(GTK_TREE_MODEL(list_store_temporary), &iter_temporary);
        }


        /* Remove last line beak character. */
        glong length_string_to_save = g_utf8_strlen (string_to_save->str,-1) - 1;
        string_to_save = g_string_truncate (string_to_save, length_string_to_save);
        save_account_numbers(string_to_save);
        
        g_free(account_number);
        g_free(account_name);
        g_free(account_description);
        g_free(routing_number);

        GtkWidget *accounts_buttons_hbox = gtk_widget_get_parent(widget);
        GtkWidget *account_button_revert = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_REVERT);
        gtk_widget_set_sensitive(account_button_revert, FALSE);
        GtkWidget *account_button_save = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_SAVE);
        gtk_widget_set_sensitive(account_button_save, FALSE);
        GtkWidget *account_button_delete = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_DELETE);
        gtk_widget_set_sensitive(account_button_delete, FALSE);
        
        /* The following statement gives an invalid pointer error. Don't we need to free string_to_save? */
        //g_free(string_to_save);
    } else {
        g_print("Could not find first iter for saving the temporary list\n");
    }
}

/**
    Constructs the view for the four buttons in the Accounts tab.
    @param pointer_passer Pointer to hash table. This parameter
    is passed to the callbacks for adding, deleting, reverting, and saving changes.
    @return An HBox containing the four buttons and associated callbacks.
*/
GtkWidget *make_accounts_buttons_hbox(GHashTable *pointer_passer) {
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


    GtkListStore *list_store_temporary = GTK_LIST_STORE( g_hash_table_lookup(pointer_passer, &KEY_LIST_STORE_TEMPORARY));


    g_signal_connect(account_button_add, "clicked", G_CALLBACK(add_row), list_store_temporary);
    g_signal_connect(account_button_delete, "clicked", G_CALLBACK(delete_row), list_store_temporary);
    g_signal_connect(account_button_revert, "clicked", G_CALLBACK(revert_listing), pointer_passer);
    g_signal_connect(account_button_save, "clicked", G_CALLBACK(save_listing), pointer_passer);

    return local_hbox;
}
