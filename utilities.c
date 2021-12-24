#include <gtk/gtk.h>

#include "constants.h"
#include "headers.h"

/**
 * @file utilities.c
 * @brief Provides various GTK utilities.
*/

/**
    Returns a specified child node from a provided parent node.
    @param parent The parent node.
    @param child_name Name of the child node.
    @return A widget corresponding to the passed name of the child node, or `NULL`
    if the child node is not found.
*/
GtkWidget *get_child_from_parent(GtkWidget *parent, const gchar *child_name) {
    GList *children_list = gtk_container_get_children(GTK_CONTAINER(parent));

    do {
        if (g_strcmp0(gtk_widget_get_name(children_list->data), child_name) == 0) {
            return children_list->data;
        }
    } while ((children_list = g_list_next(children_list)) != NULL);
    return NULL;
}

/**
Frees memory a GSList of accounts. Called from free_memory().
@param data Pointer to user data, which is to an Account entry in a GSList.
*/
GDestroyNotify free_gslist_account(gpointer data) {
    Account *account = (Account *)data;
    g_free(account);
}

/**
* Frees memory in the master and temporary account lists. This callback fires after
* the user destroys the main application windows.
* @param window The parent node.
* @param user_data Pointer to user data
* @return A widget corresponding to the passed name of the child node.
*/
void free_memory(GtkWidget *window, gpointer user_data) {
    
    GHashTable *pointer_passer = (GHashTable *)user_data;

    GtkListStore *list_store_master = GTK_LIST_STORE(g_hash_table_lookup(pointer_passer, &KEY_LIST_STORE_MASTER));
    GtkListStore *list_store_temporary = GTK_LIST_STORE(g_hash_table_lookup(pointer_passer, &KEY_LIST_STORE_TEMPORARY));

    gtk_list_store_clear(list_store_master);
    gtk_list_store_clear(list_store_temporary);
}
