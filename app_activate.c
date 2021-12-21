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
 * Function that starts the GTK loop.
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

    GtkListStore *list_store_master = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    GtkListStore *list_store_temporary = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    /* Read account numbers from disk. When is this freed? */
    GSList *list_accounts_from_disk = read_account_numbers();

    /* Place items in the temporary account list into list_builder. */
    g_slist_foreach(list_accounts_from_disk, build_list_store, list_store_master);
    g_slist_foreach(list_accounts_from_disk, build_list_store, list_store_temporary);

    /* A hash table for passing pointers to callbacks */
    GHashTable *pointer_passer = g_hash_table_new(g_int_hash, g_int_equal);

    g_hash_table_insert(pointer_passer, &KEY_LIST_STORE_MASTER, list_store_master);
    g_hash_table_insert(pointer_passer, &KEY_LIST_STORE_TEMPORARY, list_store_temporary);

    GtkWidget *accounts_tab_tree = make_tree_view(list_store_temporary);
    GtkWidget *slips_tab_tree = make_slip_view(pointer_passer);

    g_signal_connect(window, "destroy", G_CALLBACK(free_memory), pointer_passer);

    GtkWidget *accounts_buttons_hbox = make_accounts_buttons_hbox(pointer_passer);

    /* The TRUE parameter ensures the treeview maintains its initial height, even after 
        deleting rows. */
    gtk_box_pack_start(GTK_BOX(vbox_accounts), accounts_tab_tree, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_accounts), accounts_buttons_hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_slip), slips_tab_tree, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), notebook);

    gfloat total_deposit = 0.0;
    g_hash_table_insert(pointer_passer, &KEY_TOTAL_DEPOSIT, &total_deposit);

    static gboolean at_least_one_check = TRUE;
    g_hash_table_insert(pointer_passer, &KEY_AT_LEAST_ONE_CHECK, &at_least_one_check);
 
    gtk_widget_show_all(GTK_WIDGET(window));
    g_hash_table_insert(pointer_passer, &KEY_APPLICATION_WINDOW, window);

    /* Select first row in accounts list of slips tab. */
    GtkTreePath *path = gtk_tree_path_new_from_string("0");
    GtkTreeView *account_tree_view_deposit_slip = GTK_TREE_VIEW(g_hash_table_lookup(pointer_passer, &KEY_CHECKS_ACCOUNTS_TREEVIEW));
    gtk_tree_view_set_cursor(account_tree_view_deposit_slip, path, NULL, FALSE);

    GtkDrawingArea *drawing_area = (GtkDrawingArea *)g_hash_table_lookup(pointer_passer, &KEY_DRAWING_AREA);
    gtk_widget_queue_draw(GTK_WIDGET(drawing_area));
}
