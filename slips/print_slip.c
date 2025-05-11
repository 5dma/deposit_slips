#include <constants.h>
#include <gtk/gtk.h>
#include <headers.h>

/**
 * @file print_slip.c
 * @brief Prints the actual deposit slip.
 */

/**
 * Callback fired while iterating over all checks. This function prints the first two checks in the store on the front side of the deposit slip.
 * @param model Pointer to the model containing the checks.
 * @param path Path to the current check.
 * @param iter Iterator for the current check.
 * @param data Pointer to user data.
 * @return `FALSE` while we are printing the first two checks, `TRUE` if we were passed the third check, thereby halting the iteration.
 * \sa draw_page()
 * \sa print_deposit_amounts_back()
 */
gboolean print_deposit_amounts_front(GtkTreeModel *model,
									 GtkTreePath *path,
									 GtkTreeIter *iter,
									 gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;
	cairo_t *cr = data_passer->cairo_context;
	gchar *amount;
	gtk_tree_model_get(model, iter, CHECK_AMOUNT, &amount, -1);
	gchar *pathstring = gtk_tree_path_to_string(path); /* Memory freed below. */

	guint64 row_number = 0;
	GError *gerror = NULL;

	g_ascii_string_to_unsigned(
		pathstring, /* path of the current row */
		10, /* Base 10 */
		0, /* minimum value */
		100, /* maximum value */
		&row_number, /* returned row number */
		&gerror); /* pointer for GError *. */

	/* Stop rendering of checks after the second one in the store. */
	if (row_number > 1) {
		return TRUE;
	}

	/* The current amount needs to be printed at a particular coordinate
	in the preview. The horizontal coordinate is fixed, but the vertical coordinate
	changes depending on index of the current check in the list. The farther down
	the current check is, the farther down it is in the preview. The vertical coordinate
	is therefore a function of the `path` passed to the callback. */

	cairo_select_font_face(cr, data_passer->font_family_mono, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, data_passer->font_size_monospace);

	/* Get the formatted string corresponding to this check's amount. */
	gfloat current_amount = atof(amount);
	gchar *formatted_amount = comma_formatted_amount(current_amount); /* Memory freed below. */

	/* Move to the correct position to print the amount such that it is right-aligned. */
	cairo_text_extents_t extents;
	cairo_text_extents(cr, formatted_amount, &extents);

	Front *front = data_passer->front;
	cairo_move_to(cr, front->amount_x - extents.width, front->first_amount_y + (row_number * front->amount_pitch));
	cairo_show_text(cr, formatted_amount);

	g_free(formatted_amount);

	/* Increment the total of all amounts in the deposit slip and update its
	value in the hash table of passed pointers. */

	float amount_float = atof((char *)amount);

	data_passer->total_deposit += amount_float;

	g_free(amount);
	g_free(pathstring);

	return FALSE; /* Continue rendering the next row. */
}

/**
 * Callback fired while iterating over all checks. This function prints the third through last checks (if any) and subtotal on the back side of the deposit slip.
 * @param model Pointer to the model containing the checks.
 * @param path Path to the current check.
 * @param iter Iterator for the current check.
 * @param data Pointer to user data.
 * @return Always returns `FALSE` as we want to examine every check in the store.
 * \sa draw_page()
 * \sa print_deposit_amounts_front()
 */
gboolean print_deposit_amounts_back(GtkTreeModel *model,
									GtkTreePath *path,
									GtkTreeIter *iter,
									gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;
	cairo_t *cr = data_passer->cairo_context;
	gchar *amount;
	gtk_tree_model_get(model, iter, CHECK_AMOUNT, &amount, -1);
	gchar *pathstring = gtk_tree_path_to_string(path); /* Memory freed below. */

	guint64 row_number = 0;
	GError *gerror = NULL;

	g_ascii_string_to_unsigned(
		pathstring, /* path of the current row */
		10, /* Base 10 */
		0, /* minimum value */
		100, /* maximum value */
		&row_number, /* returned row number */
		&gerror);

	/* Ignore rendering of checks before the second one in the store. */
	if (row_number < 2) {
		return FALSE;
	}

	if (gerror != NULL) {
		g_print("Failed converstion: %d: %s\n", gerror->code, gerror->message);
	}

	cairo_save(cr); /* Save passed context */

	cairo_rotate(cr, G_PI_2); /* Rotate 90 degrees clockwise relative to previous context */
	cairo_translate(cr, -90, -432); /* Cancel original translation */
	cairo_translate(cr, 215, 0); /* Apply translation for the back side.*/

	/* The current amount needs to be printed at a particular coordinate
	in the preview. The horizontal coordinate is fixed, but the vertical coordinate
	changes depending on index of the current check in the list. The farther down
	the current check is, the farther down it is in the preview. The vertical coordinate
	is therefore a function of the `path` passed to the callback. */

	cairo_select_font_face(cr, data_passer->font_family_mono, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, data_passer->font_size_monospace);

	/* Get the formatted string corresponding to this check's amount. */
	gfloat current_amount = atof(amount);
	gchar *formatted_amount = comma_formatted_amount(current_amount); /* Memory freed below. */

	/* Move to the correct position to print the amount such that it is right-aligned. */
	cairo_text_extents_t extents;
	cairo_text_extents(cr, formatted_amount, &extents);

	Back *back = data_passer->back;
	cairo_move_to(cr, back->amount_x - extents.width, back->first_amount_y + (row_number * back->amount_pitch));
	cairo_show_text(cr, formatted_amount);
	g_free(formatted_amount);
	g_free(amount);
	g_free(pathstring);

	cairo_restore(cr); /* Restore passed context */
	return FALSE;
}

/**
 * Callback fired when a print job prints a page. It calls functions to print the front and the back of the deposit slip. (There is a lot of commonality between this code and the one in draw_preview(). However, the commonality was not enough to combine them into a single function.)
 *
 * @param self A Gtk print operation.
 * @param context A GTK print context.
 * @param page_nr Current page number.
 * @param data Pointer to user data.
 * \sa print_deposit_amounts_front()
 * \sa print_deposit_amounts_back()
 */
void draw_page(GtkPrintOperation *self, GtkPrintContext *context, gint page_nr, gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;

	gchar *routing_number = NULL;
	gchar *account_number = NULL;
	gchar *account_name = NULL;

	GtkTreeIter iter;
	GtkTreeModel *model;

	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data_passer->checks_accounts_treeview));

	if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
						   ACCOUNT_NUMBER, &account_number,
						   ACCOUNT_NAME, &account_name,
						   ROUTING_NUMBER, &routing_number,
						   -1);
	}

	cairo_text_extents_t extents;
	cairo_t *cr;
	cr = gtk_print_context_get_cairo_context(context);

	cairo_select_font_face(cr, data_passer->font_family_sans, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, data_passer->font_size_sans_serif);
	cairo_set_source_rgb(cr, 0, 0, 0);

	/*
		Translate the surface so that the deposit slip appears in the top middle of the printed page.
		Width of paper is 8.5 * 72 = 612
		Width of deposit slip is 6 * 72 = 432
		Difference is 180
		Therefore there is a margin of 90 on each side of the deposit slip
	 */
	cairo_translate(cr, 90, 0);

	Front *front = data_passer->front;

	/*
		Write rectangle arround the deposit slip.
	*/
	cairo_rectangle(cr, 0, 0, 432, 180);
	cairo_set_line_width(cr, 0.5);
	cairo_stroke(cr);

	/*
		Write Deposit label. This label is centered on the deposit slip, so
		we need to move to the center and then offset by 1/2 the length of the
		text "DEPOSIT."

		The center of the deposit slip is 6 * 72 / 2 = 216.
	*/
	cairo_set_font_size(cr, front->deposit_label_font_size);
	cairo_text_extents(cr, "DEPOSIT", &extents);
	cairo_move_to(cr, 216 - (extents.width / 2), front->deposit_label_y);
	cairo_show_text(cr, "DEPOSIT");

	/* Write "Checks and other Items" label. This label is rotated 90 degrees. */
	cairo_set_font_size(cr, front->checks_other_items_font_size);
	cairo_push_group(cr); /* New group for rotating */
	cairo_move_to(cr, 0, 0);
	cairo_rotate(cr, -1.570796);
	cairo_move_to(cr, -126, 7);
	cairo_show_text(cr, "CHECKS AND OTHER ITEMS ARE RECEIVED FOR DEPOSIT SUBJECT");
	cairo_move_to(cr, -117, 11);
	cairo_show_text(cr, "TO THE PROVISIONS OF THE UNIFORM COMMERCIAL CODE");
	cairo_move_to(cr, -108, 15);
	cairo_show_text(cr, "OR ANY APPLICABLE COLLECTION AGREEMENT");
	cairo_move_to(cr, 50, 50);
	cairo_pop_group_to_source(cr);
	cairo_paint(cr);

	/* Write Date label and line */
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_move_to(cr, front->date_name_address_label_x, front->date_label_y);
	cairo_set_font_size(cr, front->date_name_address_font_size);
	cairo_show_text(cr, "DATE");
	cairo_move_to(cr, front->date_name_line_x, front->date_label_y);
	cairo_line_to(cr, front->date_name_line_x + front->date_line_length, front->date_label_y);
	cairo_stroke(cr);

	/* Write Name label and line */
	cairo_move_to(cr, front->date_name_address_label_x, front->name_label_y);
	cairo_show_text(cr, "NAME");
	cairo_move_to(cr, front->date_name_line_x, front->name_label_y);
	cairo_line_to(cr, front->date_name_line_x + front->name_line_length, front->name_label_y);
	cairo_stroke(cr);

	/* Write Address label and line */
	cairo_move_to(cr, front->date_name_address_label_x, front->address_label_y);
	cairo_show_text(cr, "ADDRESS");
	cairo_move_to(cr, front->address_line_x, front->address_label_y);
	cairo_line_to(cr, front->date_name_line_x + front->name_line_length, front->address_label_y);
	cairo_stroke(cr);


	/* Write Gray Account Number label*/
	cairo_push_group(cr); /* New group for rgb */
	gdouble boxes_midpoint = (front->account_number_squares_width * 5) + front->account_number_squares_x;
	cairo_set_source_rgb(cr, 0.75, 0.75, 0.75);
	cairo_set_font_size(cr, front->account_number_label_font_size);
	cairo_text_extents(cr, "ACCOUNT NUMBER", &extents);
	cairo_move_to(cr, (boxes_midpoint - (extents.width / 2)), front->account_number_label_y);
	cairo_show_text(cr, "ACCOUNT NUMBER");

	/* Write boxes for account number */
	cairo_rectangle(cr, front->account_number_squares_x,
					front->account_number_squares_y,
					front->account_number_squares_width * 10,
					front->account_number_squares_height);
	cairo_set_line_width(cr, 2.0);
	cairo_stroke(cr);

	gdouble y_position = front->account_number_squares_y + front->account_number_squares_height;
	cairo_set_line_width(cr, 1.5);
	for (gint i = 1; i <= 9; i++)
	{
		gdouble x_position = front->account_number_squares_x + (i * front->account_number_squares_width);
		cairo_move_to(cr, x_position, front->account_number_squares_y);
		cairo_line_to(cr, x_position, y_position);
		cairo_stroke(cr);
	}

	cairo_pop_group_to_source(cr);
	cairo_paint(cr); /* Pop the group with the gray stroke.*/

	/* Write MICR routing number */
	cairo_push_group(cr); /* New group for MICR */
	cairo_set_source_rgb(cr, 0,0,0);
	cairo_select_font_face(cr, data_passer->font_face_micr, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, front->micr_font_size);
	cairo_move_to(cr, front->micr_routing_number_label_x, front->micr_routing_number_label_y);
	cairo_show_text(cr, "A53001007A");

	/* Write MICR account number */
	cairo_move_to(cr, front->micr_account_number_label_x , front->micr_routing_number_label_y);
	cairo_show_text(cr, account_number);

	/* Write MICR serial number */
	cairo_move_to(cr, front->micr_serial_number_label_x, front->micr_routing_number_label_y);
	cairo_show_text(cr, "009");

	cairo_pop_group_to_source(cr);
	cairo_paint(cr); /* Pop the group with the MICR.*/

	/* Write Account Label

		cairo_move_to(cr, front->date_name_address_label_x, front->account_number_human_value_y);
		cairo_set_font_size(cr, data_passer->font_size_sans_serif * data_passer->font_size_static_label_scaling);
		cairo_show_text(cr, "Account No");*/

	/* Write Account Value
	cairo_move_to(cr, front->date_name_value_x, front->account_number_human_value_y);
	cairo_set_font_size(cr, data_passer->font_size_sans_serif);
	cairo_show_text(cr, account_number);*/

	/* Write date
	GDateTime *date_time = g_date_time_new_now_local();
	gchar *date_time_string = g_date_time_format(date_time, "%B %e, %Y");
	cairo_move_to(cr, front->date_name_value_x, front->date_value_y);
	cairo_set_font_size(cr, data_passer->font_size_sans_serif);
	cairo_show_text(cr, date_time_string);
	g_free(date_time_string);*/

	/* Write Account in MICR
	gchar *account_with_transit = g_strconcat(account_number, MICR_ON_US, NULL);
	cairo_move_to(cr, front->micr_account_number_label_x, front->micr_account_number_label_y);
	cairo_set_font_size(cr, 20); // Need to put this in the configuration structure.
	cairo_select_font_face(cr, data_passer->font_face_micr, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_show_text(cr, account_with_transit);
	g_free(account_with_transit);*/

	data_passer->cairo_context = cr;
	data_passer->total_deposit = 0;

	/* Write amount of first two checks on front side. */
	gtk_tree_model_foreach(GTK_TREE_MODEL(data_passer->checks_store), print_deposit_amounts_front, data_passer);

	/* Write total of checks deposited */
	data_passer->total_deposit += data_passer->total_back_side;

	/* Get the width of the total amount, and move to that point to print the total. */

	gchar *formatted_total = comma_formatted_amount(data_passer->total_deposit);
	cairo_text_extents(cr, formatted_total, &extents);
	cairo_move_to(cr, front->amount_x - extents.width, front->total_y);
	cairo_set_font_size(cr, data_passer->font_size_monospace);
	cairo_select_font_face(cr, data_passer->font_family_mono, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_show_text(cr, formatted_total);
	g_free(formatted_total);

	/* If there are more than two checks, print their subtotal on the front side. */
	if (number_of_checks(data_passer) > 2) {
		gchar *formatted_total = comma_formatted_amount(data_passer->total_back_side);
		/* Move to the correct position to print the amount such that it is right-aligned. */
		cairo_text_extents_t extents;
		cairo_text_extents(data_passer->cairo_context, formatted_total, &extents);

		cairo_move_to(cr, front->amount_x - extents.width, front->subtotal_y);

		cairo_show_text(data_passer->cairo_context, formatted_total);

		/* Print the rear side */
		gtk_print_operation_draw_page_finish(self);
		gtk_tree_model_foreach(GTK_TREE_MODEL(data_passer->checks_store), print_deposit_amounts_back, data_passer);

		Back *back = data_passer->back;

		/* Print subtotal on back side. */
		cairo_move_to(cr, back->amount_x - extents.width, back->total_y);
		cairo_show_text(data_passer->cairo_context, formatted_total);
		g_free(formatted_total);
	}
}

/**
 * Callback fired when user clicks the print button. This function instantiates a GTK Print Operation, applies settings, and associates a callback draw_page().
 * @param self Pointer to the clicked button.
 * @param data Pointer to user data.
 */
void print_deposit_slip(GtkButton *self, gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;
	GtkPrintOperation *operation;
	GError *error;
	gint res;

	operation = gtk_print_operation_new();

	gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
	gtk_print_operation_set_use_full_page(operation, TRUE);
	//  gtk_print_operation_set_allow_async(operation, TRUE);
	gtk_print_operation_set_n_pages(operation, 1);

	GtkPrintSettings *settings = gtk_print_settings_new();

	gtk_print_operation_set_print_settings(operation, settings);

	g_signal_connect(G_OBJECT(operation), "draw_page", G_CALLBACK(draw_page), data_passer);

	res = gtk_print_operation_run(operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW(data_passer->application_window), &error);
	switch (res) {
		case GTK_PRINT_OPERATION_RESULT_ERROR:
			g_print("Error\n");
			break;
		case GTK_PRINT_OPERATION_RESULT_APPLY:
			if (settings != NULL)
				g_object_unref(settings);
			settings = g_object_ref(gtk_print_operation_get_print_settings(operation));
			break;
		case GTK_PRINT_OPERATION_RESULT_CANCEL:
			g_print("Cancel\n");
			break;
		case GTK_PRINT_OPERATION_RESULT_IN_PROGRESS:
			g_print("In progress\n");
			break;
		default:
	}
	g_object_unref(operation);
}