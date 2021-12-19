#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

/**
 * @file accounts_tree.c
 * @brief Constructs the tree view in the Accounts tab.
*/

/**
 * Callback fired to format each row in the Account Number column in the Accounts tab. The callback
 * retrieves the text associated with the passed `iter`. If the text is `(Number)`, the text is formatted in light gray and italic; otherwise the text is formatted as normal and black.
 * @param col Pointer to the account's column.
 * @param renderer Pointer to the account's cell renderer.
 * @param model Pointer to model associated with the tree view.
 * @param iter Pointer to iterator associated with the model.
 * @param user_data Void pointer to passed user data, `NULL` in this case.
*/
void account_column_formatter(GtkTreeViewColumn *col,
                              GtkCellRenderer *renderer,
                              GtkTreeModel *model,
                              GtkTreeIter *iter,
                              gpointer user_data) {
    gchararray account_number[500];
    gtk_tree_model_get(model, iter, ACCOUNT_NUMBER, account_number, -1);

    g_object_set(renderer, "text", *account_number, NULL);
    if (strcmp(*account_number, NEW_NUMBER) == 0) {
        g_object_set(renderer, "foreground-rgba", &NEW_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_ITALIC, NULL);

    } else {
        g_object_set(renderer, "foreground-rgba", &EXISTING_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_NORMAL, NULL);
    }
}

/**
 * Callback fired to format each row in the Name column in the Accounts tab. The callback
 * retrieves the text associated with the passed `iter`. If the text is `(Name)`, the text is formatted in light gray and italic; otherwise the text is formatted as normal and black.
 * @param col Pointer to the name's column.
 * @param renderer Pointer to the name's cell renderer.
 * @param model Pointer to model associated with the tree view.
 * @param iter Pointer to iterator associated with the model.
 * @param user_data Void pointer to passed user data, `NULL` in this case.
*/
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

/**
 * Callback fired to format each row in the Description column in the Accounts tab. The callback
 * retrieves the text associated with the passed `iter`. If the text is `(Description)`, the text is formatted in light gray and italic; otherwise the text is formatted as normal and black.
 * @param col Pointer to the description's column.
 * @param renderer Pointer to the description's cell renderer.
 * @param model Pointer to model associated with the tree view.
 * @param iter Pointer to iterator associated with the model.
 * @param user_data Void pointer to passed user data, `NULL` in this case.
*/
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


/**
 * Callback fired to format each row in the Description column in the Accounts tab. The callback
 * retrieves the text associated with the passed `iter`. If the text is `(Description)`, the text is formatted in light gray and italic; otherwise the text is formatted as normal and black.
 * @param col Pointer to the description's column.
 * @param renderer Pointer to the description's cell renderer.
 * @param model Pointer to model associated with the tree view.
 * @param iter Pointer to iterator associated with the model.
 * @param user_data Void pointer to passed user data, `NULL` in this case.
*/
void routing_column_formatter(GtkTreeViewColumn *col,
                                  GtkCellRenderer *renderer,
                                  GtkTreeModel *model,
                                  GtkTreeIter *iter,
                                  gpointer user_data) {
    gchararray routing[500];
    gtk_tree_model_get(model, iter, ROUTING_NUMBER, routing, -1);
    g_object_set(renderer, "text", *routing, NULL);
    if (strcmp(*routing, NEW_ROUTING) == 0) {
        g_object_set(renderer, "foreground-rgba", &NEW_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_ITALIC, NULL);
    } else {
        g_object_set(renderer, "foreground-rgba", &EXISTING_ACCOUNT_FOREGROUND, NULL);
        g_object_set(renderer, "style", PANGO_STYLE_NORMAL, NULL);
    }

}



/**
 * Callback fired after a cell in the account number column is edited. The function
 * replaces the account number in the model with the one passed to the callback.
 * @param renderer Pointer to the number's cell renderer.
 * @param path Pointer to the model's path where the editing took place.
 * @param new_account_number Pointer to the new account number.
 * @param user_data Void pointer to the account tree view.
*/
static void number_edited(GtkCellRendererText *renderer,
                          gchar *path,
                          gchar *new_account_number,
                          gpointer user_data) {
    GtkTreeView *treeview = (GtkTreeView *) user_data;
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (g_ascii_strcasecmp(new_account_number, "") != 0) {
        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, ACCOUNT_NUMBER, new_account_number, -1);
        }
    }
}

/**
 * Callback fired after a cell in the name  column is edited. The function
 * replaces the name in the model with the one passed to the callback.
 * @param renderer Pointer to the name's cell renderer.
 * @param path Pointer to the model's path where the editing took place.
 * @param new_account_name Pointer to the new account name.
 * @param user_data Void pointer to the tree view.
*/
static void name_edited(GtkCellRendererText *renderer,
                        gchar *path,
                        gchar *new_account_name,
                       gpointer user_data) {
    GtkTreeView *treeview = (GtkTreeView *) user_data;
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (g_ascii_strcasecmp(new_account_name, "") != 0) {
        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, ACCOUNT_NAME, new_account_name, -1);
        }
    }
}

/**
 * Callback fired after a cell in the description column is edited. The function
 * replaces the description in the model with the one passed to the callback.
 * @param renderer Pointer to the description's cell renderer.
 * @param path Pointer to the model's path where the editing took place.
 * @param new_description Pointer to the new account description.
 * @param user_data Void pointer to the tree view.
*/
static void description_edited(GtkCellRendererText *renderer,
                               gchar *path,
                               gchar *new_description,
                               gpointer user_data) {
        GtkTreeView *treeview = (GtkTreeView *) user_data;
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (g_ascii_strcasecmp(new_description, "") != 0) {
        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, DESCRIPTION, new_description, -1);
        }
    }
}


/**
 * Callback fired after a cell in the name  column is edited. The function
 * replaces the name in the model with the one passed to the callback.
 * @param renderer Pointer to the name's cell renderer.
 * @param path Pointer to the model's path where the editing took place.
 * @param new_routing_number Pointer to the new account name.
 * @param user_data Void pointer to the tree view.
*/
static void routing_edited(GtkCellRendererText *renderer,
                        gchar *path,
                        gchar *new_routing_number,
                       gpointer user_data) {
    GtkTreeView *treeview = (GtkTreeView *) user_data;
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (g_ascii_strcasecmp(new_routing_number, "") != 0) {
        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, ROUTING_NUMBER, new_routing_number, -1);
        }
    }
}



/**
 * Callback fired during a `gtk_tree_model_foreach`. The function
 * examines the checkbox in the model's passed iteration, and returns true or false depending
 * if the checkbox is marked.
 * @param model Pointer to the model being iterated.
 * @param path Pointer to the path associated with the current iteration.
 * @param iter Pointer to the iterator associated with the current iteration.
 * @param data Void pointer to user data.
 * @return `TRUE` if at least one of the checkboxes in the delete column is checked, `FALSE` otherwise.
*/
gboolean examine_all_checkboxes(GtkTreeModel *model,
                                GtkTreePath *path,
                                GtkTreeIter *iter,
                                gpointer data) {
    gboolean value;
    gboolean *checkbox_checked = (gboolean *)data;
    gtk_tree_model_get(model, iter, CHECKBOX, &value, -1);
    if (value == TRUE) {
        *checkbox_checked = TRUE;
        return TRUE;
    }
    return FALSE;
}

/**
 * Callback fired if one of the checkboxes in the Delete column is toggled on or off. The function
 * changes the view to marked or cleared depending on the checkboxes previous state.
 * @param renderer Pointer to the checkbox's cell renderer.
 * @param path Pointer to the path associated with the current iteration.
 * @param user_data Pointer to associated treeview.
*/
static void toggle_clicked(GtkCellRendererToggle *renderer,
                           gchar *path,
                           gpointer user_data) {
    GtkTreeView *treeview = (GtkTreeView *)user_data;
    GtkTreeIter iter;
    GtkTreeModel *model;
    gboolean value;
    model = gtk_tree_view_get_model(treeview);
    g_debug("Here\n");
    if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
        gtk_tree_model_get(model, &iter, CHECKBOX, &value, -1);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, CHECKBOX, !value, -1);
    }

    /* This boolean is true if at least one of the checkboxes is marked. If it is true,
       then several buttons on this UI become sensitive. */
    gboolean at_least_one_checkbox_set = FALSE;

    gtk_tree_model_foreach(model, examine_all_checkboxes, &at_least_one_checkbox_set);

    GtkWidget *accounts_tab = gtk_widget_get_parent(GTK_WIDGET(treeview));
    GtkWidget *accounts_hbox = get_child_from_parent(accounts_tab, HBOX_ACCOUNT_BUTTONS);
    GtkWidget *account_button_delete = get_child_from_parent(accounts_hbox, BUTTON_NAME_DELETE);

    if (at_least_one_checkbox_set == TRUE) {
        gtk_widget_set_sensitive(account_button_delete, TRUE);
    } else {
        gtk_widget_set_sensitive(account_button_delete, FALSE);
    }
}

/**
 * Function that creates the tree view for the account listing in the Accounts tab.
 * @param list_store Pointer to the `GtkListStore` associated with the tree view this function creates.
 * @return A pointer to the created tree view.
*/
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

    g_signal_connect(G_OBJECT(rendererAccount), "edited", G_CALLBACK(number_edited), tree);

    GtkCellRenderer *rendererName;
    GtkTreeViewColumn *columnName;

    rendererName = gtk_cell_renderer_text_new();
    columnName = gtk_tree_view_column_new_with_attributes("Name",
                                                          rendererName,
                                                          "text", ACCOUNT_NAME,
                                                          NULL);
    g_object_set(rendererName, "editable", TRUE, "editable-set", TRUE, NULL);

    g_signal_connect(G_OBJECT(rendererName), "edited", G_CALLBACK(name_edited), tree);

    GtkCellRenderer *rendererDescription;
    GtkTreeViewColumn *columnDescription;

    rendererDescription = gtk_cell_renderer_text_new();
    columnDescription = gtk_tree_view_column_new_with_attributes("Description",
                                                                 rendererDescription,
                                                                 "text", DESCRIPTION,
                                                                 NULL);

    g_object_set(rendererDescription, "editable", TRUE, "editable-set", TRUE, NULL);
    g_signal_connect(G_OBJECT(rendererDescription), "edited", G_CALLBACK(description_edited), tree);


    GtkCellRenderer *rendererRouting;
    GtkTreeViewColumn *columnRouting;

    rendererRouting = gtk_cell_renderer_text_new();
    columnRouting = gtk_tree_view_column_new_with_attributes("Routing No.",
                                                             rendererRouting,
                                                             "text", ROUTING_NUMBER,
                                                             NULL);
    g_object_set(rendererRouting, "editable", TRUE, "editable-set", TRUE, NULL);

    g_signal_connect(G_OBJECT(rendererRouting), "edited", G_CALLBACK(routing_edited), tree);


    GtkCellRenderer *rendererToggle;
    GtkTreeViewColumn *columnToggle;
    rendererToggle = gtk_cell_renderer_toggle_new();

    columnToggle = gtk_tree_view_column_new_with_attributes("Delete",
                                                            rendererToggle,
                                                            "active", CHECKBOX,
                                                            NULL);

    /* g_object_set sets a renderer's properties. */
    g_object_set(rendererToggle, "activatable", TRUE, "active", FALSE, NULL);
    g_signal_connect(G_OBJECT(rendererToggle), "toggled", G_CALLBACK(toggle_clicked), tree);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnAccount);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnName);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnDescription);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnRouting);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnToggle);

    gtk_tree_view_column_set_cell_data_func(columnAccount, rendererAccount, account_column_formatter, NULL, NULL);
    gtk_tree_view_column_set_cell_data_func(columnName, rendererName, name_column_formatter, NULL, NULL);
    gtk_tree_view_column_set_cell_data_func(columnDescription, rendererDescription, description_column_formatter, NULL, NULL);
    gtk_tree_view_column_set_cell_data_func(columnRouting, rendererRouting, routing_column_formatter, NULL, NULL);

    return tree;
}
