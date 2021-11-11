#include <gtk/gtk.h>

#include "headers.h"
/* G_MESSAGES_DEBUG=all ./deposit_slip */

// callback function which is called when application is first started
void on_app_activate(GApplication *app, gpointer data) {
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));

    GtkWidget *notebook = gtk_notebook_new();

    GtkWidget *vbox_slip = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *vbox_accounts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *label_slip = gtk_label_new("Slip");
    GtkWidget *label_account = gtk_label_new("Accounts");

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_slip, label_slip);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_accounts, label_account);

    GtkListStore *list_store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    /* Read account numbers from disk.*/
    GSList *master_account_list = read_account_numbers();
    GtkTreeIter iter;

    List_Builder_Struct list_builder;
    list_builder.iter = iter;
    list_builder.list_store = list_store;

    g_slist_foreach(master_account_list, build_list_store, &list_builder);

    //GSList *temporary_account_list = NULL;

  //  temporary_account_list = g_slist_copy_deep(master_account_list, (GCopyFunc) build_temporary_list, temporary_account_list);
    GSList *temporary_account_list = g_slist_copy_deep(master_account_list, (GCopyFunc) build_temporary_list, NULL);

    gpointer test = g_slist_nth(temporary_account_list, 1);
    Account * testptr = (Account *)test;
    g_print("Second temporary record: %s %s\n", testptr->number, testptr->description);

    GtkWidget *accounts_tab_tree = make_tree_view(list_store);

    GtkWidget *accounts_buttons_hbox = make_accounts_buttons_hbox();

    GtkWidget *topBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);

    gtk_box_pack_start(GTK_BOX(vbox_accounts), accounts_tab_tree, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_accounts), accounts_buttons_hbox, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), notebook);

    gtk_widget_show_all(GTK_WIDGET(window));
}
