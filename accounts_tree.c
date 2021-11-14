#include <gtk/gtk.h>
#include "headers.h"


void last_row_cell_data_func(GtkTreeViewColumn *col,
                             GtkCellRenderer *renderer,
                             GtkTreeModel *model,
                             GtkTreeIter *iter,
                             gpointer user_data) {
    gchararray account_number[500];
    gtk_tree_model_get(model, iter, ACCOUNT_NUMBER, account_number, -1);
//    g_print("Here is the account number after %s\n", *account_number);

    g_object_set(renderer, "style", PANGO_STYLE_NORMAL, NULL);
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
    gtk_show_about_dialog(NULL, "program-name", "ExampleCode", "title", "You will love this", NULL);
    GtkTreeIter iter;
    GtkTreeModel *model;
    GIcon *value;
    model = gtk_tree_view_get_model(treeview);
    g_print("Here\n");
    if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

static void toggle_clicked(GtkCellRendererToggle *renderer,
                           gchar *path,
                           GtkTreeView *treeview) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    gboolean value;
    model = gtk_tree_view_get_model(treeview);
    g_debug("Here\n");
    if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
        gtk_tree_model_get(model, &iter, CHECKBOX, &value, -1);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, CHECKBOX, !value, -1);
        
    }
}

GtkWidget * make_tree_view(GtkListStore *list_store ) {

    GtkTreeIter iter;
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

    GtkCellRenderer *rendererToggle;
    GtkTreeViewColumn *columnToggle;
    rendererToggle = gtk_cell_renderer_toggle_new();

    columnToggle = gtk_tree_view_column_new_with_attributes("Delete",
                                                            rendererToggle,
                                                            "active", CHECKBOX,
                                                            NULL);

    /* g_object_set sets a renderer's properties. */
    g_object_set(rendererToggle, "activatable", TRUE, "active", FALSE, NULL);
    g_signal_connect(G_OBJECT(rendererToggle), "toggled", G_CALLBACK(toggle_clicked), (gpointer)tree);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnAccount);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnDescription);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnToggle);

    gtk_tree_view_column_set_cell_data_func(columnAccount, rendererAccount, last_row_cell_data_func, NULL, NULL);

    g_object_unref(list_store); /* destroy model automatically with view */

    return tree;

}
