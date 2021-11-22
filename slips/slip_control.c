#include <gtk/gtk.h>
#include "../constants.h"
#include "../headers.h"

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
gboolean examine_all_check_checkboxes(GtkTreeModel *model,
                                      GtkTreePath *path,
                                      GtkTreeIter *iter,
                                      gpointer data) {
    gboolean value;
    gboolean *checkbox_checked = (gboolean *)data;
    gtk_tree_model_get(model, iter, CHECK_CHECKBOX, &value, -1);
    if (value == TRUE) {
        *checkbox_checked = TRUE;
        return TRUE;
    }
    return FALSE;
}

/**
 * Callback fired after changing the selection in the accounts listing in the Slips tab. 
 * The function changes the label under the accounts list to show the selected
 * account's name and description.
 * @param tree_view Pointer to tree view whose selection was changed.
 * @param user_data Pointer to passed user data (not used).
*/
void update_label(GtkTreeView *tree_view, gpointer user_data) {
    GtkWidget *tree_parent = gtk_widget_get_parent(GTK_WIDGET(tree_view));
    GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
        GtkWidget *lbl_description = get_child_from_parent(tree_parent, LABEL_ACCOUNT_DESCRIPTION);
        gchararray account_name;
        gchararray description;
        gtk_tree_model_get(model, &iter, ACCOUNT_NAME, &account_name,DESCRIPTION, &description, -1);
        gchar *full_label = g_strdup_printf("%s (%s)",account_name, description);
        g_print("%s\n",full_label);
        gtk_label_set_label(GTK_LABEL(lbl_description), full_label);
        g_free(full_label);
    } else {
        return;
    }
}

/**
 * Callback fired if one of the checkboxes in the Delete column is toggled on or off. The function
 * changes the view to marked or cleared depending on the checkboxes previous state.
 * @param renderer Pointer to the checkbox's cell renderer.
 * @param path Pointer to the path associated with the current iteration.
 * @param treeview Pointer to associated treeview.
*/
static void check_toggle_clicked(GtkCellRendererToggle *renderer,
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

    gtk_tree_model_foreach(model, examine_all_check_checkboxes, &at_least_one_checkbox_set);

    GtkWidget *accounts_tab = gtk_widget_get_parent(GTK_WIDGET(treeview));
    GtkWidget *accounts_hbox = get_child_from_parent(accounts_tab, HBOX_ACCOUNT_BUTTONS);
    GtkWidget *account_button_delete = get_child_from_parent(accounts_hbox, BUTTON_NAME_DELETE);

    if (at_least_one_checkbox_set == TRUE) {
        gtk_widget_set_sensitive(account_button_delete, TRUE);
    } else {
        gtk_widget_set_sensitive(account_button_delete, FALSE);
    }
}


