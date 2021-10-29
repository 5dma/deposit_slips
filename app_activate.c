#include <gtk/gtk.h>

#include "headers.h"
/* G_MESSAGES_DEBUG=all ./deposit_slip */
// callback function which is called when button is clicked
static void on_button_clicked(GtkButton *btn, gpointer data) {
    // change button label when it's clicked
    gtk_button_set_label(btn, "Hello World");
}

void last_row_cell_data_func(GtkTreeViewColumn *col,
                             GtkCellRenderer *renderer,
                             GtkTreeModel *model,
                             GtkTreeIter *iter,
                             gpointer user_data) {
    gchararray account_number[500];
    gtk_tree_model_get(model, iter, ACCOUNT_NUMBER, account_number, -1);
    g_print("Here is the account number after %s\n", *account_number);

    if (g_ascii_strcasecmp("New", *account_number) == 0) {
        g_object_set(renderer, "style", PANGO_STYLE_ITALIC, NULL);
    } else {
        g_object_set(renderer, "style", PANGO_STYLE_NORMAL, NULL);
    }
    g_object_set(renderer, "text", *account_number, NULL);
}

static void account_number_edited(GtkCellRendererText *renderer,
                                  gchar *path,
                                  gchar *new_account_number,
                                  GtkTreeView *treeview) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (g_ascii_strcasecmp(new_account_number, "") != 0) {
        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, ACCOUNT_NUMBER, new_account_number, -1);
        }
    }
}

static void description_edited(GtkCellRendererText *renderer,
                               gchar *path,
                               gchar *new_description,
                               GtkTreeView *treeview) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (g_ascii_strcasecmp(new_description, "") != 0) {
        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, DESCRIPTION, new_description, -1);
        }
    }
}

static void remove_row(GtkCellRendererToggle *renderer,
                       gchar *path,
                       GtkTreeView *treeview) {
    gtk_show_about_dialog (NULL, "program-name", "ExampleCode", "title", "You will love this", NULL);
    GtkTreeIter iter;
    GtkTreeModel *model;
    GIcon *value;
    model = gtk_tree_view_get_model(treeview);
    g_print("Here\n");
    if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

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

    GtkListStore *list_store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF);

    GtkTreeIter iter;

    /* Read account numbers from disk.*/
    GSList *account_numbers = read_account_numbers();

    List_Builder_Struct list_builder;
    list_builder.iter = iter;
    list_builder.list_store = list_store;

    g_slist_foreach(account_numbers, build_list_store, &list_builder);
    g_print("Go this far\n");
    GtkWidget *tree;

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    g_object_set(tree, "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, NULL);

    GtkCellRenderer *rendererAccount;
    GtkTreeViewColumn *columnAccount;

    rendererAccount = gtk_cell_renderer_text_new();
    columnAccount = gtk_tree_view_column_new_with_attributes("Account No.",
                                                             rendererAccount,
                                                             "text", ACCOUNT_NUMBER,
                                                             NULL);
    g_object_set(rendererAccount, "editable", TRUE, "editable-set", TRUE, NULL);

    g_signal_connect(G_OBJECT(rendererAccount), "edited", G_CALLBACK(account_number_edited), (gpointer)tree);

    GtkCellRenderer *rendererDescription;
    GtkTreeViewColumn *columnDescription;

    rendererDescription = gtk_cell_renderer_text_new();
    columnDescription = gtk_tree_view_column_new_with_attributes("Description",
                                                                 rendererDescription,
                                                                 "text", DESCRIPTION,
                                                                 NULL);

    g_object_set(rendererDescription, "editable", TRUE, "editable-set", TRUE, NULL);
    g_signal_connect(G_OBJECT(rendererDescription), "edited", G_CALLBACK(description_edited), (gpointer)tree);

    GtkCellRenderer *rendererDelete;
    GtkTreeViewColumn *columnDelete;
    rendererDelete = gtk_cell_renderer_pixbuf_new();

    columnDelete = gtk_tree_view_column_new_with_attributes("",
                                                            rendererDelete,
                                                            "pixbuf", DELETE_ICON,
                                                            NULL);

    /* g_object_set sets a renderer's properties. */
    g_object_set(rendererDelete, "cell-background", "#00CC00", "mode", GTK_CELL_RENDERER_MODE_EDITABLE, "sensitive", FALSE, NULL);
    g_signal_connect(G_OBJECT(rendererDelete), "editing-started", G_CALLBACK(remove_row), (gpointer)tree);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnAccount);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnDescription);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnDelete);

    gtk_tree_view_column_set_cell_data_func(columnAccount, rendererAccount, last_row_cell_data_func, NULL, NULL);

    g_object_unref(list_store); /* destroy model automatically with view */

    GtkWidget *topBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);

    gtk_box_pack_start(GTK_BOX(vbox_accounts), tree, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), notebook);

    gtk_widget_show_all(GTK_WIDGET(window));
}
