#include <gtk/gtk.h>

// callback function which is called when button is clicked
static void on_button_clicked(GtkButton *btn, gpointer data)
{
    // change button label when it's clicked
    gtk_button_set_label(btn, "Hello World");
}

static void barf(GApplication *app, gpointer data)
{
    g_print("OMG barf");
}

enum
{
    ACCOUNT_NUMBER,
    DESCRIPTION,
    CHECKBOX,
    N_COLUMNS
};

void last_row_cell_data_func(GtkTreeViewColumn *col,
                             GtkCellRenderer *renderer,
                             GtkTreeModel *model,
                             GtkTreeIter *iter,
                             gpointer user_data)
{

    gchararray account_number[500];
    gtk_tree_model_get(model, iter, ACCOUNT_NUMBER, account_number, -1);
    g_print("Here is the account number after %s\n", *account_number);

    if (g_ascii_strcasecmp("New", *account_number) == 0)
    {
        g_object_set(renderer, "style", PANGO_STYLE_ITALIC, NULL);
    }
    else
    {
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

// callback function which is called when application is first started
void on_app_activate(GApplication *app, gpointer data)
{

    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));

    GtkWidget *notebook = gtk_notebook_new();

    GtkWidget *vbox_slip = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *vbox_accounts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *label_slip = gtk_label_new("Slip");
    GtkWidget *label_account = gtk_label_new("Accounts");

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_slip, label_slip);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_accounts, label_account);

    GtkWidget *btn = gtk_button_new_with_label("Click Me!");

    g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), NULL);

    GtkListStore *list_store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    GtkTreeIter iter;

    gtk_list_store_append(list_store, &iter); /* Acquire an iterator */

    gtk_list_store_set(list_store, &iter,
                       ACCOUNT_NUMBER, "123456",
                       DESCRIPTION, "Personal checking",
                       CHECKBOX,
                       FALSE,
                       -1);
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set(list_store, &iter,
                       ACCOUNT_NUMBER, "New",
                       DESCRIPTION, "",
                       CHECKBOX,
                       FALSE,
                       -1);

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

    GtkCellRenderer *rendererToggle;
    GtkTreeViewColumn *columnToggle;
    rendererToggle = gtk_cell_renderer_toggle_new();

    columnToggle = gtk_tree_view_column_new_with_attributes("Include",
                                                            rendererToggle,
                                                            "activatable", CHECKBOX,
                                                            NULL);

    //  GtkCellRenderer *rendererAddAccount;
    //  rendererAddAccount = gtk_cell_renderer_text_new();
    columnToggle = gtk_tree_view_column_new_with_attributes("Include",
                                                            rendererToggle,
                                                            "activatable", CHECKBOX,
                                                            NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnAccount);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnDescription);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnToggle);

    gtk_tree_view_column_set_cell_data_func(columnAccount, rendererAccount, last_row_cell_data_func, NULL, NULL);

    g_signal_connect(rendererToggle, "toggled", G_CALLBACK(barf), NULL);

    g_object_unref(list_store); /* destroy model automatically with view */

    GtkWidget *topBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);

    gtk_box_pack_start(GTK_BOX(vbox_accounts), tree, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(topBox), tree, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), notebook);

    gtk_widget_show_all(GTK_WIDGET(window));
}
