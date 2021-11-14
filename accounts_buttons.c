#include <gtk/gtk.h>
#include "headers.h"
#include "constants.h"

static void add_row(GtkWidget *widget, gpointer* data) {
    GtkListStore* list_store = (GtkListStore*)data;

    GtkTreeIter iter;
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set(list_store, &iter,
                       ACCOUNT_NUMBER, NEW_NUMBER,
                       ACCOUNT_NUMBER, NEW_NAME,
                       DESCRIPTION, NEW_DESCRIPTION,
                       CHECKBOX, FALSE,
                       -1);
}

GtkWidget* make_accounts_buttons_hbox(GtkListStore* list_store) {
    GtkWidget* local_hbox;

    GtkWidget* account_button_add = gtk_button_new_from_icon_name("gtk-add", GTK_ICON_SIZE_BUTTON);
    GtkWidget* account_button_delete = gtk_button_new_from_icon_name("gtk-delete", GTK_ICON_SIZE_BUTTON);
    GtkWidget* account_button_revert = gtk_button_new_from_icon_name("gtk-refresh", GTK_ICON_SIZE_BUTTON);
    GtkWidget* account_button_save = gtk_button_new_from_icon_name("gtk-save", GTK_ICON_SIZE_BUTTON);

    local_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(local_hbox), account_button_add, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(local_hbox), account_button_delete, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(local_hbox), account_button_revert, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(local_hbox), account_button_save, TRUE, TRUE, 0);

    gtk_widget_set_tooltip_text(account_button_add, "Add account");
    gtk_widget_set_tooltip_text(account_button_delete, "Delete selected accounts");
    gtk_widget_set_tooltip_text(account_button_revert, "Revert");
    gtk_widget_set_tooltip_text(account_button_save, "Save changes");

    gtk_widget_set_sensitive(account_button_add, TRUE);
    gtk_widget_set_sensitive(account_button_delete, TRUE);
    gtk_widget_set_sensitive(account_button_revert, TRUE);
    gtk_widget_set_sensitive(account_button_save, TRUE);

    g_signal_connect(account_button_add, "clicked", G_CALLBACK(add_row), list_store);

    return local_hbox;
}
