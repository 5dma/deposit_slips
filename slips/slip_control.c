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
        gtk_tree_model_get(model, &iter, ACCOUNT_NAME, &account_name, DESCRIPTION, &description, -1);
        gchar *full_label = g_strdup_printf("%s (%s)", account_name, description);
        g_print("%s\n", full_label);
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
                                 gpointer data) {
    GtkTreeView *treeview = (GtkTreeView *)data;
    GtkTreeIter iter;
    GtkTreeModel *model;
    gboolean value;
    model = gtk_tree_view_get_model(treeview);
    g_debug("Here\n");
    if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
        gtk_tree_model_get(model, &iter, CHECK_CHECKBOX, &value, -1);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, CHECK_CHECKBOX, !value, -1);
    }

    gboolean at_least_one_checkbox_set = FALSE;

    gtk_tree_model_foreach(model, examine_all_check_checkboxes, &at_least_one_checkbox_set);

    GtkWidget *slip_tab = gtk_widget_get_parent(GTK_WIDGET(treeview));
    GtkWidget *check_button_delete = get_child_from_parent(slip_tab, BUTTON_CHECK_DELETE);

    if (at_least_one_checkbox_set == TRUE) {
        gtk_widget_set_sensitive(check_button_delete, TRUE);
    } else {
        gtk_widget_set_sensitive(check_button_delete, FALSE);
    }
}

/**
    Adds a row to the checks model after user clicks the Add button.
    @param widget Pointer to the clicked Add button.
    @param data Void pointer to the temporary list store.
*/
static void add_check_row(GtkWidget *widget, gpointer data) {
    g_print("Clicked ADD\n");
    GtkListStore *list_store = (GtkListStore *)data;

    GtkTreeIter iter;
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set(list_store, &iter,
                       CHECK_AMOUNT, NEW_AMOUNT,
                       CHECK_CHECKBOX, FALSE,
                       -1);
}

/**
    Deletes a checked row from the model (and reflected in the treeview) after user clicks the Delete button. Inspiration for
this loop is from https://www.kksou.com/php-gtk2/sample-codes/iterate-through-a-GtkListStore-Part-2.php.
    @param widget Pointer to the clicked Delete button.
    @param data Void pointer to the temporary list store.
*/
static void delete_check_row(GtkWidget *widget, gpointer data) {
    GtkListStore *list_store = (GtkListStore *)data;
    GtkTreeIter iter;
    GValue *gvalue;
    gboolean still_in_list = TRUE;
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data), &iter);

    do {
        gtk_tree_model_get_value(GTK_TREE_MODEL(data), &iter, CHECK_CHECKBOX, gvalue);
        if (g_value_get_boolean(gvalue) == TRUE) {
            gtk_list_store_remove(list_store, &iter);
        } else {
            still_in_list = gtk_tree_model_iter_next(GTK_TREE_MODEL(data), &iter);
        }
        g_value_unset(gvalue);

    } while (still_in_list);
}

/**
 * Callback fired after a cell in the amount column is edited. The function
 * replaces the amount in the model with the one passed to the callback.
 * @param renderer Pointer to the number's cell renderer.
 * @param path Pointer to the model's path where the editing took place.
 * @param new_account_number Pointer to the new account number.
 * @param treeview Pointer to the tree view.
*/
static void amount_edited(GtkCellRendererText *renderer,
                          gchar *path,
                          gchar *new_amount,
                          gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;

    GtkTreeView *treeview = GTK_TREE_VIEW(g_hash_table_lookup(pointer_passer, &KEY_CHECK_TREE_VIEW));
    GtkWidget *drawing_area = GTK_WIDGET(g_hash_table_lookup(pointer_passer, &KEY_DRAWING_AREA));

    GtkTreeIter iter;
    GtkTreeModel *model;
    if (g_ascii_strcasecmp(new_amount, "") != 0) {
        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, CHECK_AMOUNT, new_amount, -1);
        }
        g_print("I finished adding a number");

        g_hash_table_replace (pointer_passer, &KEY_DRAWING_ACTION, &DRAWING_OPERATION_CHECKS);

        gtk_widget_queue_draw(drawing_area);
  
    }
}