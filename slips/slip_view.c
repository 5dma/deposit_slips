#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"
#include "checks_store.c"
#include "slip_control.c"

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

GtkWidget *make_checks_view(GtkListStore *checks_store) {
    GtkTreeIter iter;
    GtkWidget *tree;

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(checks_store));
    g_object_set(tree, "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, NULL);

    GtkCellRenderer *rendererChecks;
    GtkTreeViewColumn *columnChecks;

    rendererChecks = gtk_cell_renderer_text_new();
    columnChecks = gtk_tree_view_column_new_with_attributes("Amount",
                                                            rendererChecks,
                                                            "text", CHECK_AMOUNT,
                                                            NULL);
    g_object_set(rendererChecks, "editable", TRUE, "editable-set", TRUE, NULL);


    GtkCellRenderer *rendererToggle;
    GtkTreeViewColumn *columnToggle;
    rendererToggle = gtk_cell_renderer_toggle_new();

    columnToggle = gtk_tree_view_column_new_with_attributes("Delete",
                                                            rendererToggle,
                                                            "active", CHECK_CHECKBOX,
                                                            NULL);

    /* g_object_set sets a renderer's properties. */
    g_object_set(rendererToggle, "activatable", TRUE, "active", FALSE, NULL);
    g_signal_connect(G_OBJECT(rendererToggle), "toggled", G_CALLBACK(check_toggle_clicked), tree);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnChecks);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnToggle);

    g_object_unref(checks_store); /* destroy model automatically with view */

    return tree;
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


    gtk_widget_set_sensitive(btnChecksAdd, TRUE);
    gtk_widget_set_sensitive(btnChecksDelete, FALSE);

    GtkWidget *drawing_area = gtk_drawing_area_new();

    GtkWidget *treeAccounts = make_account_view(list_store);

    GtkListStore *checks_store = make_checks_store();
    GtkWidget *tree_checks = make_checks_view(checks_store);
    g_signal_connect(btnChecksAdd, "clicked", G_CALLBACK(add_check_row), checks_store);

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
