#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include <string.h>

#include <headers.h>

/**
 * @file account_store.c
 * @brief Sets up the store for accounts.
 */

/**
 * Reads configuration from a JSON file in `~/.deposit_slip/deposit_slips.json`.
 */
void read_configuration_data(Data_passer *data_passer) {
	GSList *local_struct_list = NULL;
	JsonParser *parser;
	GError *error = NULL;

	/* Memory is freed at end of this function */
	gchar *input_file = g_build_filename(g_get_home_dir(), ".deposit_slip/deposit_slips.json", NULL);
	gboolean input_file_exists = g_file_test(input_file, G_FILE_TEST_EXISTS);
	if (!input_file_exists) {
		g_print("The input file %s does not exist. Exiting\n", input_file);
		exit(-1);
	}

	parser = json_parser_new();
	error = NULL;
	json_parser_load_from_file(parser, input_file, &error);

	if (error) {
		g_print("Unable to parse `%s': %s\n", input_file, error->message);
		g_error_free(error);
		g_object_unref(parser);
		return;
	}

	JsonNode *root = json_parser_get_root(parser);
	JsonObject *obj = json_node_get_object(root);

	/* Read the account information. */

	JsonArray *array = (JsonArray *)json_object_get_array_member(obj, "accounts");
	guint len = json_array_get_length(array);

	/* Would be better to put this in a function json_array_foreach_element */
	for (int i = 0; i < len; i++) {
		JsonObject *object = json_array_get_object_element(array, i);
		const gchar *account = json_object_get_string_member(object, "account");
		const gchar *name = json_object_get_string_member(object, "name");
		const gchar *description = json_object_get_string_member(object, "description");
		const gchar *routing = json_object_get_string_member(object, "routing");

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
	JsonObject *configuration_object = json_object_get_object_member(obj, "configuration");

	data_passer->font_size_sans_serif = json_object_get_int_member(configuration_object, "font_size_sans_serif");
	data_passer->font_size_monospace = json_object_get_int_member(configuration_object, "font_size_monospace");
	data_passer->font_size_static_label_scaling = json_object_get_double_member(configuration_object, "font_size_static_label_scaling");
	strcpy(data_passer->font_family_sans,json_object_get_string_member(configuration_object, "font_family_sans"));
	data_passer->font_face_micr = strdup(json_object_get_string_member(configuration_object, "font_face_micr"));
	data_passer->print_name_account_labels = json_object_get_boolean_member (configuration_object, "print_name_account_labels");
	strcpy(data_passer->font_family_mono, json_object_get_string_member(configuration_object, "font_family_mono"));

	configuration_object = json_object_get_object_member(obj, "front");
	data_passer->front->name_account_label_x = json_object_get_int_member(configuration_object,"name_account_label_x");
	data_passer->front->name_account_date_value_x = json_object_get_int_member(configuration_object,"name_account_date_value_x");
	data_passer->front->name_y = json_object_get_int_member(configuration_object,"name_y");
	data_passer->front->account_y = json_object_get_int_member(configuration_object,"account_y");
	data_passer->front->date_y = json_object_get_int_member(configuration_object,"date_y");
	data_passer->front->micr_y = json_object_get_int_member(configuration_object,"micr_y");
	data_passer->front->micr_x = json_object_get_int_member(configuration_object,"micr_x");
	data_passer->front->first_amount_y = json_object_get_int_member(configuration_object,"first_amount_y");
	data_passer->front->amount_pitch = json_object_get_int_member(configuration_object,"amount_pitch");
	data_passer->front->subtotal_y = json_object_get_int_member(configuration_object,"subtotal_y");
	data_passer->front->total_y = json_object_get_int_member(configuration_object,"total_y");
	data_passer->front->total_x = json_object_get_int_member(configuration_object,"total_x");
	json_reader_end_member(configuration_object);

	configuration_object = json_object_get_object_member(obj, "back");
	data_passer->back->amount_x = json_object_get_int_member(configuration_object,"amount_x");
	data_passer->back->first_amount_y = json_object_get_int_member(configuration_object,"first_amount_y");
	data_passer->back->amount_pitch = json_object_get_int_member(configuration_object,"amount_pitch");
	data_passer->back->total_y = json_object_get_int_member(configuration_object,"total_y");
	json_reader_end_member(configuration_object);


	g_free(input_file);
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
