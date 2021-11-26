#include <gtk/gtk.h>

#include "accounts/account_store.c"
#include "accounts/accounts_buttons.c"
#include "accounts/accounts_tree.c"
#include "headers.h"
#include "slips/slip_view.c"

/**
 * @file app_activate.c
 * @brief Builds the view.
*/


/**
 * Function that starts the Gtk loop.
 * @param app Pointer to the GTK application
 * @param data Pointer to passed data, `NULL` in this case.
*/
void on_app_activate(GApplication *app, gpointer data) {
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));

    GtkWidget *notebook = gtk_notebook_new();

    GtkWidget *vbox_slip = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *vbox_accounts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *label_slip = gtk_label_new("Slip");
    GtkWidget *label_account = gtk_label_new("Accounts");

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_slip, label_slip);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_accounts, label_account);

    GtkListStore *list_store_master = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    GtkListStore *list_store_temporary = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    /* Read account numbers from disk. When is this freed? */
    GSList *list_accounts_from_disk = read_account_numbers();


    /* Place items in the temporary account list into list_builder. */
    g_slist_foreach(list_accounts_from_disk, build_list_store, list_store_master);
    g_slist_foreach(list_accounts_from_disk, build_list_store, list_store_temporary);


    GtkWidget *accounts_tab_tree = make_tree_view(list_store_temporary);
    GtkWidget *slips_tab_tree = make_slip_view(list_store_temporary);

    GPtrArray *list_store_ptr_array = g_ptr_array_new();
    g_ptr_array_add (list_store_ptr_array, list_store_master);
    g_ptr_array_add (list_store_ptr_array, list_store_temporary);

    
    g_signal_connect(window,"destroy",G_CALLBACK(free_memory), list_store_ptr_array);


    GtkWidget *accounts_buttons_hbox = make_accounts_buttons_hbox(list_store_ptr_array);

    /* The TRUE parameter ensures the treeview maintains its initial height, even after 
        deleting rows. */
    gtk_box_pack_start(GTK_BOX(vbox_accounts), accounts_tab_tree, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_accounts), accounts_buttons_hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_slip), slips_tab_tree, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), notebook);

    gtk_widget_show_all(GTK_WIDGET(window));
}
