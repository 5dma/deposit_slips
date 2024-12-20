#include <gtk/gtk.h>
#include <math.h>

#include <constants.h>
#include <headers.h>

/**
 * @file draw_functions.c
 * @brief Contains callbacks to draw the deposit slip's preview, front side or back side.
 */


/**
 * Formats a passed float number into a familiar currency value. For example, takes 51003 and formats it into 51,003.00.
 * @param number Any gfloat less than 999999.99
 * @return The formatted string.
 */
gchar *comma_formatted_amount(gfloat number) {
	//    gfloat amount = *number;
	gchar formatted_amount[100];
	if (number < 1000) {
		g_snprintf(formatted_amount, 11, "%.2f", number);
	} else {
		gdouble first_group = floor(number / 1000);
		gfloat second_group = number - (first_group * 1000);
		g_snprintf(formatted_amount, sizeof(formatted_amount), "%.0f,%06.2f", first_group, second_group);
	}
	return g_strdup(formatted_amount);
}

/**
 * Drawing function for the front of the deposit slip. This function does the following:
 * \li Adds the created `cairo_t` pointer to the has table of pointer passers so that the function can redraw the preview.
 * \li Draws the preview for the first time.
 *
 * Some of the values in this function are hard-coded to reflect use of the MICR Encoding font by Digital Graphical Labs, https://www.1001fonts.com/micr-encoding-font.html.
 * 
 * (There is a lot of commonality between this code and the one in print_deposit_amounts_front(). However, the commonality is not enough to combine them into a single function.)
 * @param widget Pointer to the preview area.
 * @param cr Pointer to the Cairo context.
 * @param data Pointer to user data.
 */
void draw_front_preview(GtkWidget *widget, cairo_t *cr, gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;

	GtkTreeIter iter;
	GtkTreeModel *model;

	GtkTreeSelection *tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data_passer->checks_accounts_treeview));

	gchar *routing_number = NULL;
	gchar *account_number = NULL;
	gchar *account_name = NULL;

	if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
						   ACCOUNT_NUMBER, &account_number,
						   ACCOUNT_NAME, &account_name,
						   ROUTING_NUMBER, &routing_number,
						   -1);
	}

	guint width = gtk_widget_get_allocated_width(widget);
	guint height = 0.45 * width;

	gint number_of_checks_local = number_of_checks(data_passer);

	/* Draw white background */
	cairo_rectangle(cr, 0.0, 0.0, width, height);
	cairo_set_line_width(cr, 1.0);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_fill(cr);

	/* Write "Deposit Ticket" */
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_select_font_face(cr, data_passer->font_family_sans, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 6);
	cairo_move_to(cr, 104, 10);
	cairo_show_text(cr, "DEPOSIT TICKET");

	/* Write Name */
	cairo_select_font_face(cr, data_passer->font_family_sans, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 6);
	cairo_move_to(cr, 15, 38);
	cairo_show_text(cr, "Name");

	cairo_set_font_size(cr, 10);
	cairo_move_to(cr, 41, 38);
	cairo_show_text(cr, account_name);

	cairo_move_to(cr, 41, 39);
	cairo_line_to(cr, 200, 39);
	cairo_stroke(cr);

	/* Write Account Number */
	cairo_select_font_face(cr, data_passer->font_family_sans, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 6);
	cairo_move_to(cr, 15, 70);
	cairo_show_text(cr, "Account No");

	cairo_set_font_size(cr, 10);
	cairo_move_to(cr, 60, 70);
	cairo_show_text(cr, account_number);

	cairo_move_to(cr, 60, 71);
	cairo_line_to(cr, 200, 71);
	cairo_stroke(cr);

	/* Write Date */
	cairo_set_font_size(cr, 6);
	cairo_move_to(cr, 15, 105);
	cairo_show_text(cr, "Date");

	GDateTime *date_time = g_date_time_new_now_local();
	gchar *date_time_string = g_date_time_format(date_time, "%B %e, %Y");
	cairo_set_font_size(cr, 10);
	cairo_move_to(cr, 40, 105);
	cairo_show_text(cr, date_time_string);
	g_free(date_time_string);

	cairo_move_to(cr, 40, 106);
	cairo_line_to(cr, 200, 106);
	cairo_stroke(cr);

	/* Write Routing number */
	gchar *routing_with_transit = g_strconcat(MICR_TRANSIT, routing_number, MICR_TRANSIT, NULL);
	cairo_select_font_face(cr, "MICR Encoding", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 10);
	cairo_move_to(cr, 15, 135);
	cairo_show_text(cr, routing_with_transit);
	g_free(routing_with_transit);

	/* Write account number MICR*/
	gchar *account_with_transit = g_strconcat(account_number, MICR_ON_US, "009", NULL);
	cairo_set_font_size(cr, 10);
	cairo_move_to(cr, 120, 135);
	cairo_show_text(cr, account_with_transit);
	g_free(account_with_transit);

	/* Write "Total from other side" */
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_select_font_face(cr, data_passer->font_family_sans, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 6);
	cairo_move_to(cr, 350, 75);
	cairo_show_text(cr, "TOTAL FROM");
	cairo_move_to(cr, 350, 80);
	cairo_show_text(cr, "OTHER SIDE");

	/* Print the deposit amounts of all existing checks.    */

	data_passer->total_deposit = 0;
	data_passer->cairo_context = cr;
	data_passer->total_back_side = 0;
	gtk_tree_model_foreach(GTK_TREE_MODEL(data_passer->checks_store), preview_deposit_amounts_front, data_passer);

	/* Write the subtotal line from the back side. */
	if (number_of_checks_local > 2) {
		cairo_select_font_face(data_passer->cairo_context, "DejaVuMono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(data_passer->cairo_context, 16);

		gchar *formatted_total = comma_formatted_amount(data_passer->total_back_side);

		/* Move to the correct position to print the amount such that it is right-aligned. */
		cairo_text_extents_t extents;
		cairo_text_extents(data_passer->cairo_context, formatted_total, &extents);
		cairo_move_to(data_passer->cairo_context, RIGHT_MARGIN_SCREEN - extents.width, 81);
		cairo_show_text(data_passer->cairo_context, formatted_total);
		g_free(formatted_total);

		/* Add amount from back side to the total deposit. */
		data_passer->total_deposit += data_passer->total_back_side;
	}

	/* Format the total string with thousands separators and draw it. There is similar code in
  draw_static_part.c:preview_deposit_amounts_front that should be put into a function. */

	/* Get the width of the total amount, and move to that point to print the total. */
	cairo_set_font_size(cr, 15);
	cairo_text_extents_t extents;
	gchar *formatted_total = comma_formatted_amount(data_passer->total_deposit);
	cairo_text_extents(cr, formatted_total, &extents);
	cairo_move_to(cr, RIGHT_MARGIN_SCREEN - extents.width, 125);
	cairo_show_text(cr, formatted_total);
	g_free(formatted_total);

	g_free(account_name);
	g_free(routing_number);
	g_free(account_number);
}

/**
 * Drawing function for the back of the deposit slip. This function draws the check amounts in the store (starting with the third check), and also draws a subtotal. 
 *
 * (There is a lot of commonality between this code and the one in print_deposit_amounts_back(). However, the commonality is not enough to combine them into a single function.)
 * @param widget Pointer to the preview area.
 * @param cr Pointer to the Cairo context.
 * @param data Pointer to user data.
 */
void draw_back_preview(GtkWidget *widget, cairo_t *cr, gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;

	guint width = gtk_widget_get_allocated_width(widget);
	guint height = 0.45 * width;

	/* Draw white background */
	cairo_rectangle(cr, 0.0, 0.0, width, height);
	cairo_set_line_width(cr, 1.0);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_fill(cr);

	cairo_save(cr); /* Save passed context */

	/* Write "CHECKS" */
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 6);
	cairo_move_to(cr, 145, 136);
	cairo_save(cr); /* Save context 1 */
	cairo_rotate(cr, -G_PI_2);
	cairo_show_text(cr, "CHECKS");
	cairo_restore(cr); /* Restore context 1 */

	/* Write "LIST SINGLY" */
	cairo_move_to(cr, 150, 136);
	cairo_save(cr); /* Save context 1 */
	cairo_set_font_size(cr, 5);
	cairo_rotate(cr, -G_PI_2);
	cairo_show_text(cr, "LIST SINGLY");
	cairo_restore(cr); /* Restore context 1 */

	/* Write "TOTAL" */
	cairo_save(cr); /* Save context 1 */
	cairo_set_font_size(cr, 5);
	cairo_move_to(cr, 469, 126);
	cairo_rotate(cr, -G_PI_2);
	cairo_show_text(cr, "TOTAL");
	cairo_restore(cr); /* Restore context 1 */

	/* Write "MUST BE ENTERED" */
	cairo_save(cr); /* Save context 1 */
	cairo_set_font_size(cr, 4);
	cairo_move_to(cr, 474, 136);
	cairo_rotate(cr, -G_PI_2);
	cairo_show_text(cr, "MUST BE ENTERED");
	cairo_restore(cr); /* Restore context 1 */

	/* Write "ON FRONT SIDE" */
	cairo_save(cr); /* Save context 1 */
	cairo_set_font_size(cr, 4);
	cairo_move_to(cr, 479, 133);
	cairo_rotate(cr, -G_PI_2);
	cairo_show_text(cr, "ON FRONT SIDE");
	cairo_restore(cr); /* Restore context 1 */

	/* Go print the deposit amounts of all checks starting with the third one.   */

	data_passer->total_back_side = 0;
	gtk_tree_model_foreach(GTK_TREE_MODEL(data_passer->checks_store), preview_deposit_amounts_back, data_passer);

	/* Format the total string with thousands separators. There is similar code in
  draw_static_part.c:preview_deposit_amounts_back that should be put into a function. */
	cairo_set_font_size(cr, 15);

	/* Get the width of the total amount, and move to that point to print the total. */
	cairo_text_extents_t extents;
	gchar *formatted_total = comma_formatted_amount(data_passer->total_back_side);
	cairo_text_extents(cr, formatted_total, &extents);
	cairo_move_to(cr, 478, extents.width + 12);

	cairo_save(data_passer->cairo_context); /* Save context 1 */
	cairo_rotate(data_passer->cairo_context, -G_PI_2);
	cairo_show_text(data_passer->cairo_context, formatted_total);
	cairo_restore(data_passer->cairo_context); /* Restore context 1 */

	g_free(formatted_total);

	cairo_restore(cr); /* Restore passed context */
}

/**
 * Callback fired when the `draw` signal is fired to redraw the preview area of the deposit slip.
 * This callback examines the flag \ref Data_passer.front_slip_active.
 * \li If the flag is `TRUE`, we are drawing the front of the deposit slip. In that case, call draw_front_preview().
 * \li If the flag is `FALSE`, we are drawing the back of the deposit slip. In that case, call draw_back_preview().
 *
 * @param widget Pointer to the preview area.
 * @param cr Pointer to the Cairo context.
 * @param data Pointer to user data.
 * \sa print_deposit_slip()
 */
void draw_preview(GtkWidget *widget, cairo_t *cr, gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;

	if (data_passer->front_slip_active == TRUE) {
		draw_front_preview(widget, cr, data);
	} else {
		draw_back_preview(widget, cr, data);
	}
}
