#include <gtk/gtk.h>

GtkWidget* get_child_from_parent(GtkWidget* parent, const gchar* child_name) {

    GList* children_list = gtk_container_get_children(GTK_CONTAINER(parent));

    do {
        if (g_strcmp0 ( gtk_widget_get_name(children_list->data),child_name) == 0) {
            return children_list->data;
        }
    } while ((children_list = g_list_next(children_list)) != NULL);

   
}