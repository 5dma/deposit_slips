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
Frees memory allocated to a GSList of accounts. Called from free_memory().
* @param data Pointer to user data, which is to an Account entry in a GSList.
* @return A `GDestroyNotify` object.
*/
GDestroyNotify free_gslist_account(gpointer data) {
    Account *account = (Account *)data;
    g_free(account);
}

/**
* Frees memory in the master and temporary account lists. This callback fires after
* the user destroys the main application window.
* @param window The parent node.
* @param data Pointer to user data
*/
void free_memory(GtkWidget *window, gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;

    gtk_list_store_clear(data_passer->list_store_master);
    gtk_list_store_clear(data_passer->list_store_temporary);
    g_free(data_passer);
}


/**
 * Returns the number of checks in the checks list store (the number of checks being deposited).
 * @param data_passer User data
*/
gint number_of_checks (Data_passer *data_passer) {
    return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(data_passer->checks_store), NULL);
}
  