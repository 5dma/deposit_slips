#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "constants.h"
#include "headers.h"

void account_column_formatter(GtkTreeViewColumn *col,
                              GtkCellRenderer *renderer,
                              GtkTreeModel *model,
                              GtkTreeIter *iter,
                              gpointer user_data) {
    gchararray account_number[500];
    gtk_tree_model_get(model, iter, ACCOUNT_NUMBER, account_number, -1);
    g_print("Here is the account number after %s\n", *account_number);
    g_object_set(renderer, "text", *account_number, NULL);
    if (strcmp(*account_number, NEW_NUMBER) == 0) {
        g_object_set(renderer, "foreground-rgba", &NEW_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_ITALIC, NULL);

    } else {
        g_object_set(renderer, "foreground-rgba", &EXISTING_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_NORMAL, NULL);
    }
}

void name_column_formatter(GtkTreeViewColumn *col,
                           GtkCellRenderer *renderer,
                           GtkTreeModel *model,
                           GtkTreeIter *iter,
                           gpointer user_data) {
    gchararray name[500];
    gtk_tree_model_get(model, iter, ACCOUNT_NAME, name, -1);
    g_object_set(renderer, "text", *name, NULL);
    if (strcmp(*name, NEW_NAME) == 0) {
        g_object_set(renderer, "foreground-rgba", &NEW_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_ITALIC, NULL);

    } else {
        g_object_set(renderer, "foreground-rgba", &EXISTING_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_NORMAL, NULL);
    }
}

void description_column_formatter(GtkTreeViewColumn *col,
                                  GtkCellRenderer *renderer,
                                  GtkTreeModel *model,
                                  GtkTreeIter *iter,
                                  gpointer user_data) {
    gchararray description[500];
    gtk_tree_model_get(model, iter, DESCRIPTION, description, -1);
    g_object_set(renderer, "text", *description, NULL);
    if (strcmp(*description, NEW_DESCRIPTION) == 0) {
        g_object_set(renderer, "foreground-rgba", &NEW_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_ITALIC, NULL);
    } else {
        g_object_set(renderer, "foreground-rgba", &EXISTING_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_NORMAL, NULL);
    }
}

static void number_edited(GtkCellRendererText *renderer,
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

static void name_edited(GtkCellRendererText *renderer,
                        gchar *path,
                        gchar *new_account_name,
                        GtkTreeView *treeview) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (g_ascii_strcasecmp(new_account_name, "") != 0) {
        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, ACCOUNT_NAME, new_account_name, -1);
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

gboolean examine_all_checkboxes (GtkTreeModel *model,
                                    GtkTreePath *path,
                                    GtkTreeIter *iter,
                                    gpointer data) {
    gboolean value;
    gboolean * checkbox_checked = (gboolean *) data;
    gtk_tree_model_get(model, iter, CHECKBOX, &value, -1);
    if (value == TRUE) {
        *checkbox_checked = TRUE;
        return TRUE;
    }
  return FALSE;
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

    gboolean at_least_one_checkbox_set = FALSE;

    gtk_tree_model_foreach(model,  examine_all_checkboxes , &at_least_one_checkbox_set);

    GtkWidget * accounts_tab = gtk_widget_get_parent (GTK_WIDGET(treeview));
    GtkWidget * accounts_hbox = get_child_from_parent(accounts_tab, HBOX_ACCOUNT_BUTTONS);
    GtkWidget * account_button_delete =  get_child_from_parent(accounts_hbox, BUTTON_NAME_DELETE);

    if (at_least_one_checkbox_set == TRUE) {
            gtk_widget_set_sensitive(account_button_delete, TRUE);
    } else {
            gtk_widget_set_sensitive(account_button_delete, FALSE);

    }
}


GtkWidget *make_tree_view(GtkListStore *list_store) {
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

    g_signal_connect(G_OBJECT(rendererAccount), "edited", G_CALLBACK(number_edited), (gpointer)tree);

    GtkCellRenderer *rendererName;
    GtkTreeViewColumn *columnName;

    rendererName = gtk_cell_renderer_text_new();
    columnName = gtk_tree_view_column_new_with_attributes("Name",
                                                          rendererName,
                                                          "text", ACCOUNT_NAME,
                                                          NULL);
    g_object_set(rendererName, "editable", TRUE, "editable-set", TRUE, NULL);

    g_signal_connect(G_OBJECT(rendererName), "edited", G_CALLBACK(name_edited), (gpointer)tree);

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
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnName);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnDescription);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnToggle);

    gtk_tree_view_column_set_cell_data_func(columnAccount, rendererAccount, account_column_formatter, NULL, NULL);
    gtk_tree_view_column_set_cell_data_func(columnName, rendererName, name_column_formatter, NULL, NULL);
    gtk_tree_view_column_set_cell_data_func(columnDescription, rendererDescription, description_column_formatter, NULL, NULL);

    //gtk_list_box_set_selection_mode(GTK_LIST_BOX(tree), GTK_SELECTION_SINGLE);

    g_object_unref(list_store); /* destroy model automatically with view */

    return tree;
}
