#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"
#include "draw_functions.c"
#include "slip_control.c"

/**
 * @file slip_view.c
 * @brief Creates the view for creating the deposit slip.
*/

/**
 * Creates the view of account numbers in the deposit list tab.
 * @param pointer_passer Hash table of pointers. This function uses the pointer to the master list of account numbers.
 * @return A tree view of account numbers.
*/
GtkWidget *make_account_view(GHashTable *pointer_passer) {
    
    GtkTreeIter iter;
    GtkWidget *tree_view;

    GtkListStore *list_store_master = GTK_LIST_STORE(g_hash_table_lookup(pointer_passer, &KEY_LIST_STORE_MASTER));

    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store_master));

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

    return tree_view;
}

/**
 * Creates the view of checks appearing in a deposit slip.
 * @param pointer_passer Hash table of pointers. This function uses the pointer to the `ListStore` of checks.
 * @return A tree view with two columns: one for amounts, another a checkbox to delete an amount.
*/
GtkWidget *make_checks_view(GHashTable *pointer_passer) {
    
    GtkTreeIter iter;
    GtkWidget *tree;

    GtkListStore *checks_store = GTK_LIST_STORE(g_hash_table_lookup(pointer_passer, &KEY_CHECKS_STORE));

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(checks_store));

    g_hash_table_insert(pointer_passer, &KEY_CHECK_TREE_VIEW, tree);
    g_object_set(tree, "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, NULL);

    GtkCellRenderer *rendererChecks;
    GtkTreeViewColumn *columnChecks;

    rendererChecks = gtk_cell_renderer_text_new();
    columnChecks = gtk_tree_view_column_new_with_attributes("Amount",
                                                            rendererChecks,
                                                            "text", CHECK_AMOUNT,
                                                            NULL);
    g_object_set(rendererChecks, "editable", TRUE, "editable-set", TRUE, NULL);

    /* Every time a cell in the Checks column is editing, redraw the preview. */
    g_signal_connect(G_OBJECT(rendererChecks), "edited", G_CALLBACK(draw_background), pointer_passer);

    GtkCellRenderer *rendererToggle;
    GtkTreeViewColumn *columnToggle;
    rendererToggle = gtk_cell_renderer_toggle_new();

    columnToggle = gtk_tree_view_column_new_with_attributes("Delete",
                                                            rendererToggle,
                                                            "active", CHECK_CHECKBOX,
                                                            NULL);

    /* g_object_set sets a renderer's properties. */
    g_object_set(rendererToggle, "activatable", TRUE, "active", FALSE, NULL);

    /* Every time a delete checkbox is toggled, set certain buttons to be sensitive. */
    g_signal_connect(G_OBJECT(rendererToggle), "toggled", G_CALLBACK(check_toggle_clicked), tree);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnChecks);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnToggle);

    g_object_unref(checks_store); /* destroy model automatically with view */

    return tree;
}

/**
 * Creates a view for the deposit slip. The view contains a list of accounts,
 * a list of checks to deposit (amount only), and a preview of the deposit slip.
 * @param pointer_passer Pointer to the hash table of passed pointers.
 * @return A widget containing the three widgets described above.
 */
GtkWidget *make_slip_view(GHashTable *pointer_passer) {

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

    gtk_widget_set_sensitive(btnChecksAdd, TRUE);
    gtk_widget_set_sensitive(btnChecksDelete, FALSE);

    GtkWidget *drawing_area = gtk_drawing_area_new();

    g_hash_table_insert(pointer_passer, &KEY_DRAWING_AREA, drawing_area);

    GtkWidget *treeAccounts = make_account_view(pointer_passer);

    GtkListStore *checks_store = gtk_list_store_new(SLIP_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN);

    g_hash_table_insert(pointer_passer, &KEY_CHECKS_STORE, checks_store);

    GtkWidget *tree_checks = make_checks_view(pointer_passer);
    /* When clicking the add button, add a row to the view */
    g_signal_connect(btnChecksAdd, "clicked", G_CALLBACK(add_check_row), checks_store);
    /* When clicking the delete button, remove rows whose checkbox is marked. */
    g_signal_connect(btnChecksDelete, "clicked", G_CALLBACK(delete_check_row), checks_store);

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

    gtk_widget_set_size_request(drawing_area, 500, 100);

    /* When the draw signal is fired on the drawing area (which can happen billions of time)
    from GTKs internal messaging), go redraw the deposit slip preview. */
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_preview), pointer_passer);



    gtk_grid_attach(GTK_GRID(gridSlip), btnSlipPrint, 3, 2, 1, 1);

    gtk_grid_set_column_spacing(GTK_GRID(gridSlip), 20);
    gtk_grid_set_row_spacing(GTK_GRID(gridSlip), 20);

    //gtk_grid_set_column_homogeneous (GTK_GRID(gridSlip), TRUE);

    return gridSlip;
}
