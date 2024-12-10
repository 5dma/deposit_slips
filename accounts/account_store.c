#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include <string.h>

#include <headers.h>
#include <constants.h>

/**
 * @file account_store.c
 * @brief Sets up the store for accounts.
 */

gint retrieve_json_int(JsonReader *reader, gchar *name) {
	json_reader_read_member(reader, name);
	gint value = json_reader_get_int_value(reader);
	json_reader_end_member(reader);
	return value;
}


void retrieve_json_string(JsonReader *reader, gchar *name, gchar *destination) {
	json_reader_read_member(reader, name);
	strcpy(destination,json_reader_get_string_value(reader));
	json_reader_end_member(reader);
}



/**
 * Reads configuration from a JSON file in `~/.deposit_slip/deposit_slips.json`.
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
		json_reader_read_element (reader, i);
		const gchar *account = json_reader_get_string_value(reader);
		const gchar *name = json_reader_get_string_value(reader);
		const gchar *description = json_reader_get_string_value(reader);
		const gchar *routing = json_reader_get_string_value(reader);

		Account *account_entry = g_new(Account, 1);
		g_stpcpy(account_entry->number, account);
		g_stpcpy(account_entry->name, name);
		g_stpcpy(account_entry->description, description);
		g_stpcpy(account_entry->routing, routing);

		local_struct_list = g_slist_append(local_struct_list, account_entry);
	}

	/* Place items in the temporary account list into list_builder. */
	g_slist_foreach(local_struct_list, build_list_store, data_passer->list_store_master);
	g_slist_foreach(local_struct_list, build_list_store, data_passer->list_store_temporary);
	g_slist_free_full(local_struct_list, (GDestroyNotify)free_gslist_account);

	/* Read the layout parameters. */
	json_reader_read_member(reader, "configuration");


	data_passer->font_size_sans_serif = retrieve_json_int(reader, "font_size_sans_serif");
	data_passer->font_size_monospace = retrieve_json_int(reader, "font_size_monospace");
	data_passer->font_size_static_label_scaling = retrieve_json_int(reader, "font_size_static_label_scaling");
	retrieve_json_string( reader, "font_family_sans", data_passer->font_family_sans);
	retrieve_json_string( reader, "font_face_micr", data_passer->font_face_micr);

	json_reader_read_member(reader, "print_name_account_labels");
	data_passer->print_name_account_labels = json_reader_get_boolean_value (reader);
	json_reader_end_member(reader);

	retrieve_json_string( reader, "font_family_mono", data_passer->font_family_mono);

	json_reader_read_member(reader, "configuration");
	data_passer->front->name_account_label_x = retrieve_json_int(reader,"name_account_label_x");
	data_passer->front->name_account_date_value_x = retrieve_json_int(reader,"name_account_date_value_x");
	data_passer->front->name_y = retrieve_json_int(reader,"name_y");
	data_passer->front->account_y = retrieve_json_int(reader,"account_y");
	data_passer->front->date_y = retrieve_json_int(reader,"date_y");
	data_passer->front->micr_y = retrieve_json_int(reader,"micr_y");
	data_passer->front->micr_x = retrieve_json_int(reader,"micr_x");
	data_passer->front->first_amount_y = retrieve_json_int(reader,"first_amount_y");
	data_passer->front->amount_pitch = retrieve_json_int(reader,"amount_pitch");
	data_passer->front->subtotal_y = retrieve_json_int(reader,"subtotal_y");
	data_passer->front->total_y = retrieve_json_int(reader,"total_y");
	data_passer->front->total_x = retrieve_json_int(reader,"total_x");
	json_reader_end_member(reader);

	json_reader_read_member(reader, "back");
	data_passer->back->amount_x = retrieve_json_int(reader,"amount_x");
	data_passer->back->first_amount_y = retrieve_json_int(reader,"first_amount_y");
	data_passer->back->amount_pitch = retrieve_json_int(reader,"amount_pitch");
	data_passer->back->total_y = retrieve_json_int(reader,"total_y");
	json_reader_end_member(reader); /* back */
	json_reader_end_member(reader); /* configuration */
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
