#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"
#include "checks_store.c"
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

GtkWidget *make_account_view(GtkListStore *list_store) {
    GtkTreeIter iter;
    GtkWidget *tree_view;

    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    g_object_set(tree_view, "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, NULL);

    GtkCellRenderer *rendererAccount;
    GtkTreeViewColumn *columnAccount;

    rendererAccount = gtk_cell_renderer_text_new();
    columnAccount = gtk_tree_view_column_new_with_attributes("Account No.",
                                                             rendererAccount,
                                                             "text", ACCOUNT_NUMBER,
                                                             NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), columnAccount);

    GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_set_mode(tree_selection, GTK_SELECTION_SINGLE);

    g_signal_connect(G_OBJECT(tree_view), "cursor-changed", G_CALLBACK(update_label), NULL);
    //g_object_unref(list_store); /* destroy model automatically with view */

    return tree_view;
}

GtkWidget *make_slip_view(GtkListStore *list_store) {
    GtkWidget *lblAccount = gtk_label_new("Accounts");
    GtkWidget *lblChecks = gtk_label_new("Checks");
    GtkWidget *lblAccountDescription = gtk_label_new("Description");

    GtkWidget *btnChecksAdd = gtk_button_new_from_icon_name("gtk-add", GTK_ICON_SIZE_BUTTON);
    GtkWidget *btnChecksDelete = gtk_button_new_from_icon_name("gtk-delete", GTK_ICON_SIZE_BUTTON);
    ;
    GtkWidget *btnSlipPrint = gtk_button_new_from_icon_name("gtk-print", GTK_ICON_SIZE_BUTTON);

    gtk_widget_set_name(btnChecksAdd, BUTTON_CHECK_ADD);
    gtk_widget_set_name(btnChecksDelete, BUTTON_CHECK_DELETE);
    gtk_widget_set_name(btnSlipPrint, BUTTON_SLIP_PRINT);
    gtk_widget_set_name(lblAccountDescription, LABEL_ACCOUNT_DESCRIPTION);

    GtkWidget *drawing_area = gtk_drawing_area_new();

    GtkWidget *treeAccounts = make_account_view(list_store);

    GtkWidget *tree_checks;

    GtkListStore *checks_store = make_checks_store();
    tree_checks = gtk_tree_view_new_with_model(GTK_TREE_MODEL(checks_store));

    GtkWidget *gridSlip = gtk_grid_new();
    /* First column of grid */
    gtk_grid_attach(GTK_GRID(gridSlip), lblAccount, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridSlip), treeAccounts, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(gridSlip), lblAccountDescription, 0, 2, 1, 1);

    /* Second column of grid */
    gtk_grid_attach(GTK_GRID(gridSlip), lblChecks, 1, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(gridSlip), tree_checks, 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(gridSlip), btnChecksDelete, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(gridSlip), btnChecksAdd, 2, 2, 1, 1);

    /* Third column of grid */
    gtk_grid_attach(GTK_GRID(gridSlip), drawing_area, 3, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(gridSlip), btnSlipPrint, 3, 2, 1, 1);

    gtk_grid_set_column_spacing(GTK_GRID(gridSlip), 20);
    gtk_grid_set_row_spacing(GTK_GRID(gridSlip), 20);

    return gridSlip;
}

GtkWidget *make_checks_view(GtkListStore *list_store) {
    GtkTreeIter iter;
    GtkWidget *tree;

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    g_object_set(tree, "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, NULL);

    GtkCellRenderer *rendererChecks;
    GtkTreeViewColumn *columnChecks;

    rendererChecks = gtk_cell_renderer_text_new();
    columnChecks = gtk_tree_view_column_new_with_attributes("Amount",
                                                            rendererChecks,
                                                            "text", ACCOUNT_NUMBER,
                                                            NULL);
    //g_signal_connect(G_OBJECT(rendererAccount), "edited", G_CALLBACK(number_edited), (gpointer)tree);

    GtkCellRenderer *rendererToggle;
    GtkTreeViewColumn *columnToggle;
    rendererToggle = gtk_cell_renderer_toggle_new();

    columnToggle = gtk_tree_view_column_new_with_attributes("Delete",
                                                            rendererToggle,
                                                            "active", CHECKBOX,
                                                            NULL);

    /* g_object_set sets a renderer's properties. */
    g_object_set(rendererToggle, "activatable", TRUE, "active", FALSE, NULL);
    g_signal_connect(G_OBJECT(rendererToggle), "toggled", G_CALLBACK(check_toggle_clicked), (gpointer)tree);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnChecks);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnToggle);

    g_object_unref(list_store); /* destroy model automatically with view */

    return tree;
}
