#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"
#include "draw_functions.c"
#include "print_slip.c"
#include "slip_control.c"

/**
 * @file slip_view.c
 * @brief Creates the view for creating the deposit slip.
 */

/**
 * Creates the view of account numbers in the deposit list tab.
 * @param data_passer Pointer to user data.
 * @return A tree view of account numbers.
 */
GtkWidget *make_account_view(Data_passer *data_passer) {
	GtkTreeIter iter;
	GtkWidget *tree_view;

	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(data_passer->list_store_master));
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

	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree_view), TRUE);
	g_signal_connect(G_OBJECT(tree_view), "row-activated", G_CALLBACK(update_label), NULL);

	return tree_view;
}

/**
 * Callback that prevents the user from entering anything in a `GtkCellEditable` other than digits and a decimal point. The actual allowed keys are [0-9], decimal point, backspace, delete, cursor right, and cursor left.
 * @param widget Widget where the edit is occurring.
 * @param event Key that was pressed.
 * @param user_data Pointer to user data. `NULL` in this case.
 * @return  `FALSE` if an allowed key was pressed, `TRUE` otherwise.
 * \sa started_cell_editing()
 */
static gboolean number_formatter(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	if (
		(event->keyval >= GDK_KEY_0) && (event->keyval <= GDK_KEY_9) ||
		(event->keyval >= GDK_KEY_KP_0) && (event->keyval <= GDK_KEY_KP_9) ||
		(event->keyval == GDK_KEY_period) ||
		(event->keyval == GDK_KEY_KP_Decimal) ||
		(event->keyval == GDK_KEY_BackSpace) ||
		(event->keyval == GDK_KEY_Left) ||
		(event->keyval == GDK_KEY_Right) ||
		(event->keyval == GDK_KEY_KP_Left) ||
		(event->keyval == GDK_KEY_KP_Right) ||
		(event->keyval == GDK_KEY_Delete)) {
		return FALSE;
	}
	return TRUE;
}

/**
 * Callback used to capture the `GtkCellEditable`. We connect that object to the `key-press-event` signal so we can ensure the user enters only digits and a decimal point.
 * @param self Cell where the editing started.
 * @param editable `GtkCellEditable` in which user performs edits.
 * @param path Path within the tree view the editing occurs.
 * @param user_data Pointer to user data. `NULL` in this case.
 * \sa number_formatter()
 */
void started_cell_editing(GtkCellRenderer *self, GtkCellEditable *editable, gchar *path, gpointer user_data) {
	g_signal_connect(GTK_WIDGET(editable), "key-press-event",
					 G_CALLBACK(number_formatter), user_data);
}

/**
 * Creates the view of checks appearing in a deposit slip.
 * @param data_passer Pointer to user data.
 * @return A tree view with two columns: one for amounts, another a checkbox to delete an amount.
 */
void make_checks_view(Data_passer *data_passer) {
	GtkTreeIter iter;
	GtkWidget *tree;

	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(data_passer->checks_store));

	data_passer->check_tree_view = tree;
	g_object_set(tree, "enable-grid-lines", GTK_TREE_VIEW_GRID_LINES_BOTH, NULL);

	GtkCellRenderer *rendererChecks;
	GtkTreeViewColumn *columnChecks;

	rendererChecks = gtk_cell_renderer_text_new();
	columnChecks = gtk_tree_view_column_new_with_attributes("Amount",
															rendererChecks,
															"text", CHECK_AMOUNT,
															NULL);
	/* Right-align text in check column. */
	gtk_cell_renderer_set_alignment(rendererChecks, 1.0, 0.5);

	g_object_set(rendererChecks, "editable", TRUE, "editable-set", TRUE, NULL);

	/* Every time a cell in the Checks column is editing, redraw the preview. */
	g_signal_connect(G_OBJECT(rendererChecks), "edited", G_CALLBACK(deposit_amount_edited), data_passer);

	/* Connect to a callback that captures the GtkEdit used to edit a cell. We use that callback to monitor the keys pressed inside the cell. */
	g_signal_connect(rendererChecks, "editing-started", G_CALLBACK(started_cell_editing), NULL);

	GtkCellRenderer *rendererToggle;
	GtkTreeViewColumn *columnToggle;
	rendererToggle = gtk_cell_renderer_toggle_new();

	columnToggle = gtk_tree_view_column_new_with_attributes("Delete",
															rendererToggle,
															"active", CHECK_RADIO,
															NULL);
	/* Set alignment of column title. */
	gtk_tree_view_column_set_alignment(columnToggle, 0.5);

	gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(rendererToggle), FALSE);

	/* g_object_set sets a renderer's properties. */
	g_object_set(rendererToggle, "activatable", FALSE, "active", FALSE, NULL);

	/* Add the renderer to the pointer passer, as it is used in various callbacks. */
	data_passer->radio_renderer = rendererToggle;

	/* Every time a delete checkbox is toggled, set certain buttons to be sensitive. */
	g_signal_connect(G_OBJECT(rendererToggle), "toggled", G_CALLBACK(check_toggle_clicked), data_passer);

	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnChecks);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnToggle);

	g_object_unref(data_passer->checks_store); /* destroy model automatically with view */
}

/**
 * Creates a view for the deposit slip. The view contains a list of accounts,
 * a list of checks to deposit (amount only), and a preview of the deposit slip.
 * @param data_passer Pointer to user data.
 * @return A widget containing the three widgets described above.
 */
GtkWidget *make_slip_view(Data_passer *data_passer) {
	GtkWidget *lblAccount = gtk_label_new(NULL);
	gtk_label_set_markup (GTK_LABEL (lblAccount), "<b>Accounts</b>");
	GtkWidget *lblChecks = gtk_label_new(NULL);
	gtk_label_set_markup (GTK_LABEL (lblChecks), "<b>Checks</b>");
	GtkWidget *lblAccountDescription = gtk_label_new("Description");

	GtkWidget *btnChecksAdd = gtk_button_new_from_icon_name("gtk-add", GTK_ICON_SIZE_BUTTON);
	GtkWidget *btnChecksDelete = gtk_button_new_from_icon_name("gtk-delete", GTK_ICON_SIZE_BUTTON);
	;
	data_passer->btn_checks_delete = btnChecksDelete;
	GtkWidget *btnSlipPrint = gtk_button_new_from_icon_name("gtk-print", GTK_ICON_SIZE_BUTTON);

	GtkWidget *btnGotoFirst = gtk_button_new_from_icon_name("gtk-goto-first-ltr", GTK_ICON_SIZE_BUTTON);
	GtkWidget *btnGotoLast = gtk_button_new_from_icon_name("gtk-goto-last-ltr", GTK_ICON_SIZE_BUTTON);
	data_passer->btn_go_to_first = btnGotoFirst;
	data_passer->btn_go_to_last = btnGotoLast;
	g_signal_connect(btnGotoFirst, "clicked", G_CALLBACK(toggle_slip_view), data_passer);
	g_signal_connect(btnGotoLast, "clicked", G_CALLBACK(toggle_slip_view), data_passer);


	gtk_widget_set_name(btnChecksAdd, BUTTON_CHECK_ADD);
	gtk_widget_set_name(btnChecksDelete, BUTTON_CHECK_DELETE);
	gtk_widget_set_name(btnSlipPrint, BUTTON_SLIP_PRINT);
	gtk_widget_set_name(lblAccountDescription, LABEL_ACCOUNT_DESCRIPTION);

	gtk_label_set_xalign(GTK_LABEL(lblAccount), 0.0);
	gtk_label_set_xalign(GTK_LABEL(lblChecks), 0.0);
	gtk_label_set_xalign(GTK_LABEL(lblAccountDescription), 0.0);

	gtk_widget_set_margin_start(lblAccount, 3);
	gtk_widget_set_margin_start(lblChecks, 4);
	gtk_widget_set_margin_start(lblAccountDescription, 5);

	gtk_widget_set_sensitive(btnChecksAdd, TRUE);
	gtk_widget_set_sensitive(btnChecksDelete, FALSE);

	/* Prevent buttons from expanding horizontally. */
	gtk_widget_set_halign(btnChecksAdd, GTK_ALIGN_CENTER);
	gtk_widget_set_halign(btnChecksDelete, GTK_ALIGN_CENTER);
	gtk_widget_set_halign(btnSlipPrint, GTK_ALIGN_CENTER);

	/* Prevent btnGotoFirst and btnGotoLast from appearing when showing the application window. */
	gtk_widget_set_no_show_all(btnGotoFirst, TRUE);
	gtk_widget_set_no_show_all(btnGotoLast, TRUE);

	gtk_widget_hide(btnGotoFirst);
	gtk_widget_hide(btnGotoLast);




	GtkWidget *drawing_area = gtk_drawing_area_new();
	data_passer->drawing_area = drawing_area;

	data_passer->checks_accounts_treeview = make_account_view(data_passer);

	GtkListStore *checks_store = gtk_list_store_new(SLIP_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN);
	GtkTreeIter trashiter;
	gtk_list_store_append(checks_store, &trashiter);
	gtk_list_store_set(checks_store, &trashiter,
					   CHECK_AMOUNT, "1.00",
					   CHECK_RADIO, FALSE,
					   -1);

	data_passer->checks_store = checks_store;

	make_checks_view(data_passer);
	/* When clicking the add button, add a row to the view */
	g_signal_connect(btnChecksAdd, "clicked", G_CALLBACK(add_check_row), data_passer);
	/* When clicking the delete button, remove rows whose checkbox is marked. */
	g_signal_connect(btnChecksDelete, "clicked", G_CALLBACK(delete_check_rows), data_passer);
	/* When clicking the preint button, print the deposit slip. */
	g_signal_connect(btnSlipPrint, "clicked", G_CALLBACK(print_deposit_slip), data_passer);

	/* Make a scrolled window for the checks being deposited. */
	GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_window), data_passer->check_tree_view);
	gtk_widget_set_size_request(scrolled_window, 125, -1);

	GtkWidget *gridSlip = gtk_grid_new();
	/* First column of grid */
	gtk_grid_attach(GTK_GRID(gridSlip), lblAccount, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(gridSlip), data_passer->checks_accounts_treeview, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(gridSlip), lblAccountDescription, 0, 2, 4, 1);

	/* Second column of grid */
	gtk_grid_attach(GTK_GRID(gridSlip), lblChecks, 1, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(gridSlip), scrolled_window, 1, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(gridSlip), btnChecksDelete, 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(gridSlip), btnChecksAdd, 2, 3, 1, 1);

	/* Third column of grid */
	gtk_grid_attach(GTK_GRID(gridSlip), drawing_area, 3, 1, 1, 1);

	gtk_widget_set_size_request(drawing_area, 500, 150);

	/* When the draw signal is fired on the drawing area (which can happen billions of times
	from GTK's internal messaging), go redraw the deposit slip preview. */
	g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_preview), data_passer);

	/* Prevent the print button from expanding to the width of the grid column. */

	GtkWidget *box_first_last_print = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_start(GTK_BOX(box_first_last_print), btnGotoFirst, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(box_first_last_print), btnGotoLast, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(box_first_last_print), btnSlipPrint, TRUE, FALSE, 5);

	gtk_grid_attach(GTK_GRID(gridSlip), box_first_last_print, 3, 3, 1, 1);

	gtk_grid_set_column_spacing(GTK_GRID(gridSlip), 20);
	gtk_grid_set_row_spacing(GTK_GRID(gridSlip), 20);

	return gridSlip;
}
