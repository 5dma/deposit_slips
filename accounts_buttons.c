#include <gtk/gtk.h>

#include "constants.h"
#include "headers.h"

static void add_row(GtkWidget* widget, gpointer* data) {
    GtkListStore* list_store = (GtkListStore*)data;

    GtkTreeIter iter;
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set(list_store, &iter,
                       ACCOUNT_NUMBER, NEW_NUMBER,
                       ACCOUNT_NAME, NEW_NAME,
                       DESCRIPTION, NEW_DESCRIPTION,
                       CHECKBOX, FALSE,
                       -1);
    GtkWidget* accounts_buttons_hbox = gtk_widget_get_parent(widget);
    GtkWidget* account_button_revert = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_REVERT);
    gtk_widget_set_sensitive(account_button_revert, TRUE);
    GtkWidget* account_button_save = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_SAVE);
    gtk_widget_set_sensitive(account_button_save, TRUE);
}

static void delete_row(GtkWidget* widget, gpointer* data) {
    GtkListStore* list_store = (GtkListStore*)data;
    GtkTreeModel * treemodel = (GtkTreeModel *)data;
    GtkTreeIter iter;
    GValue * gvalue;
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data), &iter);

    do {
        gtk_tree_model_get_value(GTK_TREE_MODEL(data), &iter, CHECKBOX, gvalue);
        if (g_value_get_boolean(gvalue) == TRUE) {
            gtk_list_store_remove(list_store, &iter);
        }
        g_value_unset(gvalue);

    } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(data), &iter));

    GtkWidget* accounts_buttons_hbox = gtk_widget_get_parent(widget);
    GtkWidget* account_button_revert = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_REVERT);
    gtk_widget_set_sensitive(account_button_revert, TRUE);
    GtkWidget* account_button_save = get_child_from_parent(accounts_buttons_hbox, BUTTON_NAME_SAVE);
    gtk_widget_set_sensitive(account_button_save, TRUE);
}

GtkWidget* make_accounts_buttons_hbox(GtkListStore* list_store) {
    GtkWidget* local_hbox;

    GtkWidget* account_button_add = gtk_button_new_from_icon_name("gtk-add", GTK_ICON_SIZE_BUTTON);
    GtkWidget* account_button_delete = gtk_button_new_from_icon_name("gtk-delete", GTK_ICON_SIZE_BUTTON);
    GtkWidget* account_button_revert = gtk_button_new_from_icon_name("gtk-refresh", GTK_ICON_SIZE_BUTTON);
    GtkWidget* account_button_save = gtk_button_new_from_icon_name("gtk-save", GTK_ICON_SIZE_BUTTON);

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

    g_signal_connect(account_button_add, "clicked", G_CALLBACK(add_row), list_store);
    g_signal_connect(account_button_delete, "clicked", G_CALLBACK(delete_row), list_store);

    return local_hbox;
}
