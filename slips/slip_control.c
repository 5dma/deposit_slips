#include <gtk/gtk.h>

#include <constants.h>
#include <headers.h>

/**
 * @file slip_control.c
 * @brief Contains code for managing interactions with the controls in the deposit slip view.
 */

/**
 * Callback fired during a `gtk_tree_model_foreach`. The function
 * determines if the passed checkbox is checked. If it is, set the flag `CheckSelection.at_least_one_check_selected` to `TRUE`.
 * @param model Pointer to the model being iterated.
 * @param path Pointer to the path associated with the current iteration.
 * @param iter Pointer to the iterator associated with the current iteration.
 * @param data Pointer to a \ref CheckSelection structure.
 * @return `TRUE` upon being passed a checkbox that is checked, indicating that at least one checkbox is checked. We can then activate the Delete button. `FALSE` if the current checkbox is not checked, implying we can possibly iterate over all checkboxes.
 * \sa check_toggle_clicked()
 */
gboolean examine_all_check_checkboxes(GtkTreeModel *model,
									  GtkTreePath *path,
									  GtkTreeIter *iter,
									  gpointer data) {
	CheckSelection *check_selection = (CheckSelection *)data;
	gboolean value;
	if (gtk_tree_model_get_iter(model, iter, path)) {
		gtk_tree_model_get(model, iter, CHECK_RADIO, &value, -1);
		if (value == TRUE) {
			check_selection->at_least_one_check_selected = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * Callback fired after changing the selection in the accounts listing in the Slips tab.
 * The function changes the label under the accounts list to show the selected
 * account's name and description.
 * @param tree_view Pointer to tree view whose selection was changed.
 * @param user_data Pointer to user data (not used).
 */
void update_label(GtkTreeView *tree_view, gpointer user_data) {
	GtkWidget *tree_parent = gtk_widget_get_parent(GTK_WIDGET(tree_view));
	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(tree_view);
	GtkTreeModel *model;
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
		GtkWidget *lbl_description = get_child_from_parent(tree_parent, LABEL_ACCOUNT_DESCRIPTION);
		gchar *account_name;
		gchar *description;
		gtk_tree_model_get(model, &iter, ACCOUNT_NAME, &account_name, DESCRIPTION, &description, -1);
		gchar *full_label = g_strdup_printf("%s (%s)", account_name, description);
		gtk_label_set_label(GTK_LABEL(lbl_description), full_label);
		g_free(full_label);
		g_free(account_name);
		g_free(description);
	} else {
		return;
	}
}

/**
 * Callback fired if one of the checkboxes in the Delete column is toggled on or off. The function
 * changes the view to marked or cleared depending on the checkbox's previous state. However, if there is only one
 * check in the check store, we do not toggle it because we must maintain at least one check in the store.
 *
 * @param renderer Pointer to the checkbox's cell renderer.
 * @param path Pointer to the path associated with the current iteration.
 * @param data Pointer to associated treeview.
 */
void check_toggle_clicked(GtkCellRendererToggle *renderer,
								 gchar *path,
								 gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean value;
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(data_passer->check_tree_view));

	if (number_of_checks(data_passer) == 1) {
		return;
	}

	if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
		gtk_tree_model_get(model, &iter, CHECK_RADIO, &value, -1);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, CHECK_RADIO, !value, -1);
	}

	CheckSelection check_selection;
	check_selection.path = gtk_tree_path_new_from_string(path); /* Memory freed below. */
	check_selection.at_least_one_check_selected = FALSE;

	gtk_tree_model_foreach(model, examine_all_check_checkboxes, &check_selection);

	if (check_selection.at_least_one_check_selected == TRUE) {
		gtk_widget_set_sensitive(data_passer->btn_checks_delete, TRUE);
	} else {
		gtk_widget_set_sensitive(data_passer->btn_checks_delete, FALSE);
	}

	gtk_tree_path_free(check_selection.path);
}


/**
	Toggles between drawing the front side of the deposit slip and the back side.
	@param widget Pointer to the clicked Add button.
	@param data Pointer to the temporary list store.
*/
void toggle_slip_view(GtkWidget *widget, gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;

	if (widget == data_passer->btn_go_to_first) {
		gtk_widget_set_sensitive(data_passer->btn_go_to_first, FALSE);
		gtk_widget_set_sensitive(data_passer->btn_go_to_last, TRUE);
		data_passer->front_slip_active = TRUE;

	} else {
		gtk_widget_set_sensitive(data_passer->btn_go_to_last, FALSE);
		gtk_widget_set_sensitive(data_passer->btn_go_to_first, TRUE);
		data_passer->front_slip_active = FALSE;

	}
}

/**
	Adds a row to the checks model after user clicks the Add button.
	@param widget Pointer to the clicked Add button.
	@param data Pointer to the temporary list store.
*/
void add_check_row(GtkWidget *widget, gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;

	GtkTreeIter iter;
	gtk_list_store_append(data_passer->checks_store, &iter);
	gtk_list_store_set(data_passer->checks_store, &iter,
					   CHECK_AMOUNT, NEW_AMOUNT,
					   CHECK_RADIO, FALSE,
					   -1);

	/* If we added a 15th row, set the button's sensitivity to FALSE to prevent the user from adding another row. */
	gint local_number_of_checks = number_of_checks(data_passer);
	if (local_number_of_checks >= 15) {
		gtk_widget_set_sensitive(widget, FALSE);
	}

	if (local_number_of_checks > 2) {
	/* If have more than two rows in the store:

		a) Show the page first/last buttons.
		b) Set their sensitivities.
	*/
		gtk_widget_show(data_passer->btn_go_to_first);
		gtk_widget_show(data_passer->btn_go_to_last);

		gtk_widget_set_sensitive(data_passer->btn_go_to_first, FALSE);
		gtk_widget_set_sensitive(data_passer->btn_go_to_last, TRUE);

	}
	/* After adding a row, enable the radio buttons to delete one of the rows. */
	g_object_set(data_passer->radio_renderer, "activatable", TRUE, NULL);
}

/**
	Deletes all checked rows from the model (and reflected in the treeview) after user clicks the Delete button. Inspiration for this loop is from [198. How to iterate through a GtkListStore - Part 2?](https://www.kksou.com/php-gtk2/sample-codes/iterate-through-a-GtkListStore-Part-2.php). This function also sets the delete button's sensitivity to `FALSE`.

	@param widget Pointer to the clicked Delete button.
	@param data Pointer to the temporary list store.
*/
void delete_check_rows(GtkWidget *widget, gpointer data) {

	Data_passer *data_passer = (Data_passer *)data;

  //  GtkListStore *list_store = (GtkListStore *)data;
	GtkTreeIter iter;
	GValue gvalue = G_VALUE_INIT;
	gboolean still_in_list = TRUE;
	gboolean removed_last_row = FALSE;

	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data_passer->checks_store), &iter);

	do {
		g_value_unset(&gvalue);

		gtk_tree_model_get_value(GTK_TREE_MODEL(data_passer->checks_store), &iter, CHECK_RADIO, &gvalue);

		if (g_value_get_boolean(&gvalue) == TRUE) {
			removed_last_row = !gtk_list_store_remove(data_passer->checks_store, &iter);
		} else {
			still_in_list = gtk_tree_model_iter_next(GTK_TREE_MODEL(data_passer->checks_store), &iter);
		}
	} while (still_in_list && !removed_last_row);

	/* After deleting the checked rows, set the delete button's sensitivity (passed to this function) to FALSE. */
	gtk_widget_set_sensitive(widget, FALSE);


	if (number_of_checks(data_passer) <= 2) {
		gtk_widget_hide(data_passer->btn_go_to_first);
		gtk_widget_hide(data_passer->btn_go_to_last);
	}
}
