#include <gtk/gtk.h>

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
GtkWidget* get_child_from_parent(GtkWidget* parent, const gchar* child_name) {

    GList* children_list = gtk_container_get_children(GTK_CONTAINER(parent));

    do {
        if (g_strcmp0 ( gtk_widget_get_name(children_list->data),child_name) == 0) {
            return children_list->data;
        }
    } while ((children_list = g_list_next(children_list)) != NULL);

   
}