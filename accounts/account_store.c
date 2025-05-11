#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include <string.h>

#include <headers.h>
#include <constants.h>

/**
 * @file account_store.c
 * @brief Sets up the store for accounts.
 */

/**
 * Reads an integer value for the passed key and the passed reader.
 * @param reader A reader pointing to an object in the JSON file.
 * @param key Key within the object.
 * @return Integer value associated with the key.
  */
gint retrieve_json_int(JsonReader *reader, gchar *key) {
	json_reader_read_member(reader, key);
	gint value = json_reader_get_int_value(reader);
	json_reader_end_member(reader);
	return value;
}

/**
 * Reads a double float value for the passed key and the passed reader.
 * @param reader A reader pointing to an object in the JSON file.
 * @param key Key within the object.
 * @return Integer value associated with the key.
  */
 gdouble retrieve_json_double(JsonReader *reader, gchar *key) {
	json_reader_read_member(reader, key);
	gdouble value = json_reader_get_double_value(reader);
	json_reader_end_member(reader);
	return value;
}

/**
 * Copies a retrieved JSON string into the passed destination buffer.
 * @param reader A reader pointing to an object in the JSON file.
 * @param key Key within the object.
 * @param destination Character array into which the string value is copied. This character array must have previously been allocated.
  */
void retrieve_json_string(JsonReader *reader, gchar *key, gchar *destination) {
	json_reader_read_member(reader, key);
	strcpy(destination,json_reader_get_string_value(reader));
	json_reader_end_member(reader);
}

/**
 * Reads configuration from a JSON file whose path is defined in CONFIG_FILE.
 * @param data_passer Pointer to user data.
 */
void read_configuration_data(Data_passer *data_passer) {
	GSList *local_struct_list = NULL;
	JsonParser *parser;
	GError *error = NULL;

	/* Memory is freed at end of this function */

	gboolean input_file_exists = g_file_test(CONFIG_FILE, G_FILE_TEST_EXISTS);
	if (!input_file_exists) {
		g_print("The input file %s does not exist. Exiting\n", CONFIG_FILE);
		exit(-1);
	}

	parser = json_parser_new();
	error = NULL;
	json_parser_load_from_file(parser, CONFIG_FILE, &error);

	if (error) {
		g_print("Unable to parse `%s': %s\n", CONFIG_FILE, error->message);
		g_error_free(error);
		g_object_unref(parser);
		return;
	}


	JsonReader *reader = json_reader_new(json_parser_get_root(parser));

	/* Read the account information. */

	json_reader_read_member(reader, "accounts");
	guint len = json_reader_count_elements(reader);

	/* Would be better to put this in a function json_array_foreach_element */
	for (int i = 0; i < len; i++) {
		json_reader_read_element (reader, i); /* Start reading account element in the array. */

		json_reader_read_member (reader,"account");
		const gchar *account = json_reader_get_string_value(reader);
		json_reader_end_member (reader);
		
		json_reader_read_member (reader,"name");
		const gchar *name = json_reader_get_string_value(reader);
		json_reader_end_member (reader);

		json_reader_read_member (reader,"description");
		const gchar *description = json_reader_get_string_value(reader);
		json_reader_end_member (reader);
		
		json_reader_read_member (reader,"routing");
		const gchar *routing = json_reader_get_string_value(reader);
		json_reader_end_member (reader);

		json_reader_end_element(reader); /* End reading account element. */

		Account *account_entry = (Account *)g_malloc(sizeof(Account));
		g_strlcpy(account_entry->number, account, -1);
		g_strlcpy(account_entry->name, name, -1);
		g_strlcpy(account_entry->description, description, -1);
		g_strlcpy(account_entry->routing, routing, -1);

		local_struct_list = g_slist_append(local_struct_list, account_entry);
	}

	json_reader_end_member(reader); /* End reading the accounts stanza*/

	/* Place items in the temporary account list into list_builder. */
	g_slist_foreach(local_struct_list, build_list_store, data_passer->list_store_master);
	g_slist_foreach(local_struct_list, build_list_store, data_passer->list_store_temporary);
	g_slist_free_full(local_struct_list, (GDestroyNotify)free_gslist_account);

	/* Read the configuration parameters. */
	json_reader_read_member(reader, "configuration");

	data_passer->font_size_sans_serif = retrieve_json_int(reader, "font_size_sans_serif");
	data_passer->font_size_monospace = retrieve_json_int(reader, "font_size_monospace");
	data_passer->font_size_static_label_scaling = retrieve_json_int(reader, "font_size_static_label_scaling");
	retrieve_json_string( reader, "font_family_sans", data_passer->font_family_sans);
	retrieve_json_string( reader, "font_face_micr", data_passer->font_face_micr);
	retrieve_json_string( reader, "font_family_mono", data_passer->font_family_mono);

	json_reader_read_member(reader, "front");
	data_passer->front->checks_other_items_label_x = retrieve_json_int(reader,"checks_other_items_label_x");
	data_passer->front->checks_other_items_label_y = retrieve_json_int(reader,"checks_other_items_label_y");
	data_passer->front->checks_other_items_font_size = retrieve_json_double(reader, "checks_other_items_font_size");

	data_passer->front->deposit_label_y = retrieve_json_int(reader,"deposit_label_y");
	data_passer->front->deposit_label_font_size = retrieve_json_int(reader,"deposit_label_font_size");

	data_passer->front->date_name_address_label_x = retrieve_json_int(reader,"date_name_address_label_x");
	data_passer->front->date_label_y = retrieve_json_int(reader,"date_label_y");
	data_passer->front->name_label_y = retrieve_json_int(reader,"name_label_y");
	data_passer->front->date_name_value_x = retrieve_json_int(reader,"date_name_value_x");
	data_passer->front->date_value_y = retrieve_json_int(reader,"date_value_y");
	data_passer->front->name_value_y = retrieve_json_int(reader,"name_value_y");
	
	data_passer->front->date_name_line_x = retrieve_json_int(reader,"date_name_line_x");
	data_passer->front->date_name_line_y = retrieve_json_int(reader,"date_name_line_y");
	data_passer->front->date_line_length = retrieve_json_int(reader,"date_line_length");
	data_passer->front->name_line_length = retrieve_json_int(reader,"name_line_length");

	data_passer->front->address_label_y = retrieve_json_int(reader,"address_label_y");

	data_passer->front->address_line_x = retrieve_json_int(reader,"address_line_x");
	data_passer->front->date_name_address_font_size = retrieve_json_int(reader,"date_name_address_font_size");

	data_passer->front->account_number_label_y = retrieve_json_int(reader,"account_number_label_y");
	data_passer->front->account_number_label_font_size = retrieve_json_double(reader,"account_number_label_font_size");

	data_passer->front->account_number_human_value_x = retrieve_json_int(reader,"account_number_human_value_x");
	data_passer->front->account_number_human_value_y = retrieve_json_int(reader,"account_number_human_value_y");
	data_passer->front->account_number_squares_x = retrieve_json_int(reader,"account_number_squares_x");
	data_passer->front->account_number_squares_y = retrieve_json_int(reader,"account_number_squares_y");
	data_passer->front->account_number_squares_width = retrieve_json_int(reader,"account_number_squares_width");
	data_passer->front->account_number_squares_height = retrieve_json_int(reader,"account_number_squares_height");
	
	data_passer->front->micr_routing_number_label_x = retrieve_json_int(reader,"micr_routing_number_label_x");
	data_passer->front->micr_routing_number_label_y = retrieve_json_int(reader,"micr_routing_number_label_y");
	data_passer->front->micr_account_number_label_x = retrieve_json_int(reader,"micr_account_number_label_x");
	data_passer->front->micr_serial_number_label_x = retrieve_json_int(reader,"micr_serial_number_label_x");
	data_passer->front->micr_font_size = retrieve_json_int(reader, "micr_font_size");

	data_passer->front->cash_label_x = retrieve_json_int(reader,"cash_label_x");
	data_passer->front->checks_label_x = retrieve_json_int(reader,"checks_label_x");
	data_passer->front->checks_label_y = retrieve_json_int(reader,"checks_label_y");
	data_passer->front->subtotal_label_y = 0;
	data_passer->front->less_cash_label_y = 0;
	data_passer->front->net_deposit_label_x = retrieve_json_int(reader,"net_deposit_label_x");
	data_passer->front->net_deposit_label_y = 0;
	data_passer->front->cash_label_font_size = retrieve_json_int(reader, "cash_label_font_size");

	data_passer->front->dollar_label_x = retrieve_json_int(reader,"dollar_label_x");
	data_passer->front->dollar_label_y = retrieve_json_int(reader,"dollar_label_y");

	data_passer->front->checks_bracket_right_x = retrieve_json_int(reader,"checks_bracket_right_x");
	data_passer->front->checks_bracket_width = retrieve_json_int(reader,"checks_bracket_width");
	data_passer->front->checks_bracket_height = retrieve_json_int(reader,"checks_bracket_height");
	data_passer->front->checks_bracket_spacing = retrieve_json_int(reader,"checks_bracket_spacing");

	data_passer->front->amount_boxes_x = retrieve_json_int(reader,"amount_boxes_x");
	data_passer->front->amount_boxes_y = retrieve_json_int(reader,"amount_boxes_y");
	data_passer->front->amount_boxes_width = retrieve_json_int(reader,"amount_boxes_width");
	data_passer->front->amount_boxes_height = retrieve_json_int(reader,"amount_boxes_height");
	data_passer->front->amount_boxes_separator_height = retrieve_json_int(reader,"amount_boxes_separator_height");

	data_passer->front->first_amount_y = retrieve_json_int(reader,"first_amount_y");
	data_passer->front->amount_pitch = retrieve_json_int(reader,"amount_pitch");
	data_passer->front->subtotal_y = retrieve_json_int(reader,"subtotal_y");
	data_passer->front->total_y = retrieve_json_int(reader,"total_y");
	data_passer->front->amount_x = retrieve_json_int(reader,"amount_x");


/*
	data_passer->front->date_name_address_label_x = retrieve_json_int(reader,"date_name_address_label_x");
	data_passer->front->date_name_value_x = retrieve_json_int(reader,"date_name_value_x");
	data_passer->front->name_value_y = retrieve_json_int(reader,"name_value_y");
	data_passer->front->date_value_y = retrieve_json_int(reader,"date_value_y");
	data_passer->front->micr_account_number_label_y = retrieve_json_int(reader,"micr_account_number_label_y");
	data_passer->front->micr_account_number_label_x = retrieve_json_int(reader,"micr_account_number_label_x");
	data_passer->front->first_amount_y = retrieve_json_int(reader,"first_amount_y");
	data_passer->front->amount_pitch = retrieve_json_int(reader,"amount_pitch");
	data_passer->front->subtotal_y = retrieve_json_int(reader,"subtotal_y");
	data_passer->front->total_y = retrieve_json_int(reader,"total_y");
	data_passer->front->amount_x = retrieve_json_int(reader,"amount_x");*/
	json_reader_end_member(reader); /* front */

	json_reader_read_member(reader, "back");
	data_passer->back->amount_x = retrieve_json_int(reader,"amount_x");
	data_passer->back->first_amount_y = retrieve_json_int(reader,"first_amount_y");
	data_passer->back->amount_pitch = retrieve_json_int(reader,"amount_pitch");
	data_passer->back->total_y = retrieve_json_int(reader,"total_y");
	json_reader_end_member(reader); /* back */
	json_reader_end_member(reader); /* configuration */
	g_object_unref(reader);

}
/**
 * Callback fired while iterating over each member of a `GSList` of list of
 * accounts read from disk. The accounts are added to the passed `ListStore`.
 * @param account List of accounts in a `GSList`.
 * @param data The ListStore into which the accounts are copied.
 */
void build_list_store(gpointer account, gpointer data) {
	GtkListStore *list_store = GTK_LIST_STORE(data);
	GtkTreeIter iter;

	Account *local_account = (Account *)account;

	gtk_list_store_append(list_store, &iter);

	gtk_list_store_set(list_store, &iter,
					   ACCOUNT_NUMBER, local_account->number,
					   ACCOUNT_NAME, local_account->name,
					   DESCRIPTION, local_account->description,
					   ROUTING_NUMBER, local_account->routing,
					   CHECKBOX, FALSE,
					   -1);
}
