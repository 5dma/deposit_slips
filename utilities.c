#include <gtk/gtk.h>

#include "headers.h"

/**
 * @file utilities.c
 * @brief Provides various GTK utilities.
*/

/**
    Returns a specified child node from a provided parent node.
    @param parent The parent node.
    @param child_name Name of the child node.
    @return A widget corresponding to the passed name of the child node.
*/
GtkWidget *get_child_from_parent(GtkWidget *parent, const gchar *child_name) {
    GList *children_list = gtk_container_get_children(GTK_CONTAINER(parent));

    do {
        if (g_strcmp0(gtk_widget_get_name(children_list->data), child_name) == 0) {
            return children_list->data;
        }
    } while ((children_list = g_list_next(children_list)) != NULL);
}

/**
    Frees memory a GSList of accounts. Called from `free_memory`.
    @param data Pointer to user data, which is to an Account entry in a GSList.
*/
GDestroyNotify free_gslist_account(gpointer data) {
    Account *account = (Account *)data;
    g_free(account);
}


/**
    Frees memory in the master and temporary account lists. This callback fires after
    the user destroyes the main application windows.
    @param window The parent node.
    @param user_data Pointer to user data
    @return A widget corresponding to the passed name of the child node.
*/
void free_memory(GtkWidget *window, gpointer user_data) {
    GPtrArray *list_store_ptr_array = (GPtrArray *)user_data;
    GSList *master_account_list = g_ptr_array_index(list_store_ptr_array, POSITION_LIST_STORE_MASTER);
    g_slist_free_full(master_account_list, (GDestroyNotify)free_gslist_account);
     GSList *temporary_account_list = g_ptr_array_index(list_store_ptr_array, POSITION_LIST_STORE_TEMPORARY);
    g_slist_free_full(temporary_account_list, (GDestroyNotify)free_gslist_account);

    /* The following lines clear the list_store behind the store of the account tree. However
    it gives us a run-time error, it seems like the store no longer exists.
    List_Builder_Struct *list_builder_struct = g_ptr_array_index(list_store_ptr_array, POSITION_LIST_BUILDER_STRUCT);
    GtkListStore *list_store =  list_builder_struct->list_store;
    gtk_list_store_clear (list_store);
    */
}
