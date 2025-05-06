#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include <glib/gstdio.h>

#include <constants.h>
#include <headers.h>

/**
 * @file utilities.c
 * @brief Provides various GTK utilities.
 */

/**
	Returns a specified child node from a provided parent node.
	@param parent The parent node.
	@param child_name Name of the child node.
	@return A widget corresponding to the passed name of the child node, or `NULL`
	if the child node is not found.
*/
GtkWidget *get_child_from_parent(GtkWidget *parent, const gchar *child_name) {
	GList *children_list = gtk_container_get_children(GTK_CONTAINER(parent));

	do {
		if (g_strcmp0(gtk_widget_get_name(children_list->data), child_name) == 0) {
			return children_list->data;
		}
	} while ((children_list = g_list_next(children_list)) != NULL);
	return NULL;
}


/**
	Adds an integer key/value pair to a passed JSON builder.
	@param builder The parent node.
	@param key Key to be added.
	@param value Integer to be added.
*/
void set_int_value (JsonBuilder *builder, gchar *key, gint value) {
	json_builder_set_member_name (builder, key);
	json_builder_add_int_value (builder, value);
}

/**
	Adds a string key/value pair to a passed JSON builder.
	@param builder The parent node.
	@param key Key to be added.
	@param value String to be added.
*/
void set_string_value (JsonBuilder *builder, gchar *key, gchar *value) {
	json_builder_set_member_name (builder, key);
	json_builder_add_string_value (builder, value);
}

/**
	Adds a double key/value pair to a passed JSON builder.
	@param builder The parent node.
	@param key Key to be added.
	@param value Double to be added.
*/
void set_double_value (JsonBuilder *builder, gchar *key, gdouble value) {
	json_builder_set_member_name (builder, key);
	json_builder_add_double_value (builder, value);
}

/**
	Adds a boolean key/value pair to a passed JSON builder.
	@param builder The parent node.
	@param key Key to be added.
	@param value Boolean to be added.
*/
void set_boolean_value (JsonBuilder *builder, gchar *key, gboolean value) {
	json_builder_set_member_name (builder, key);
	json_builder_add_boolean_value (builder, value);
}


/**
Frees memory allocated to a GSList of accounts. Called from write_config_free_memory().
* @param data Pointer to user data, which is to an Account entry in a GSList.
*/
void free_gslist_account(gpointer data) {
	Account *account = (Account *)data;
	g_free(account);
}

/**
* Writes the configuration file, and frees memory in the master and temporary account lists and other string data in a `Data_passer`. This callback fires after the user destroys the main application window.
* @param window The parent node.
* @param data Pointer to user data
*/
void write_config_free_memory(GtkWidget *window, gpointer data) {
	Data_passer *data_passer = (Data_passer *)data;

	GtkTreeIter iter_master;
	GtkTreeIter iter_temporary;

	/* Copy the settings file to a backup file. */
	//g_rename (CONFIG_FILE, CONFIG_FILE_BACKUP);


	JsonBuilder *builder = json_builder_new (); /* root */
	json_builder_begin_object (builder);
	
	gboolean found_first_iter_temporary = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data_passer->list_store_temporary), &iter_temporary);

	if (found_first_iter_temporary) {
		/* gchar pointers for reading the entries from the master store. These variables are freed below. */
		gchar *account_number = NULL;
		gchar *account_name = NULL;
		gchar *account_description = NULL;
		gchar *routing_number = NULL;

		/* A JSON array of accounts. Memory freed below. */
		json_builder_set_member_name (builder, "accounts");
		json_builder_begin_array (builder);

		gtk_list_store_clear(data_passer->list_store_master);

		while (gtk_list_store_iter_is_valid(data_passer->list_store_temporary, &iter_temporary)) {
			gtk_tree_model_get(GTK_TREE_MODEL(data_passer->list_store_temporary), &iter_temporary,
							   ACCOUNT_NUMBER, &account_number,
							   ACCOUNT_NAME, &account_name,
							   DESCRIPTION, &account_description,
							   ROUTING_NUMBER, &routing_number,
							   -1);

			/* Add the entry from the temporary store to the master store */
			gtk_list_store_append(data_passer->list_store_master, &iter_master);

			gtk_list_store_set(data_passer->list_store_master, &iter_master,
							   ACCOUNT_NUMBER, account_number,
							   ACCOUNT_NAME, account_name,
							   DESCRIPTION, account_description,
							   ROUTING_NUMBER, routing_number,
							   -1);


			/* Create a JSON object for the current account. */
			json_builder_begin_object(builder);

			json_builder_set_member_name (builder, "account");
			json_builder_add_string_value(builder, account_number);

			json_builder_set_member_name (builder, "name");
			json_builder_add_string_value(builder, account_name);

			json_builder_set_member_name (builder, "description");
			json_builder_add_string_value(builder, account_description);

			json_builder_set_member_name (builder, "routing");
			json_builder_add_string_value(builder, routing_number);

 			json_builder_end_object(builder); /* End account stanza */
			
			gtk_tree_model_iter_next(GTK_TREE_MODEL(data_passer->list_store_temporary), &iter_temporary);
		}
		g_free(account_number);
		g_free(account_name);
		g_free(account_description);
		g_free(routing_number);
		
		json_builder_end_array(builder); /* accounts */
		/* Create the top-level JSON object. */

		json_builder_set_member_name (builder, "configuration");
		json_builder_begin_object (builder);


		set_int_value(builder, "font_size_sans_serif", data_passer->font_size_sans_serif);
		set_int_value(builder, "font_size_monospace", data_passer->font_size_monospace);
		set_string_value(builder, "font_family_sans", data_passer->font_family_sans);
		set_string_value(builder, "font_face_micr", data_passer->font_face_micr);
		set_string_value(builder, "font_family_mono", data_passer->font_family_mono);
		set_double_value(builder, "font_size_static_label_scaling", data_passer->font_size_static_label_scaling);

		json_builder_set_member_name (builder, "front");
		json_builder_begin_object (builder);
		set_int_value(builder, "date_name_address_label_x", data_passer->front->date_name_address_label_x);
		set_int_value(builder, "date_name_value_x", data_passer->front->date_name_value_x);
		set_int_value(builder, "name_value_y", data_passer->front->name_value_y);
		set_int_value(builder, "date_value_y", data_passer->front->date_value_y);
		set_int_value(builder, "micr_account_number_label_y", data_passer->front->micr_account_number_label_y);
		set_int_value(builder, "micr_account_number_label_x", data_passer->front->micr_account_number_label_x);
		set_int_value(builder, "first_amount_y", data_passer->front->first_amount_y);
		set_int_value(builder, "amount_pitch", data_passer->front->amount_pitch);
		set_int_value(builder, "subtotal_y", data_passer->front->subtotal_y);
		set_int_value(builder, "total_y", data_passer->front->total_y);
		set_int_value(builder, "amount_x", data_passer->front->amount_x);
		json_builder_end_object(builder); /* front */

		json_builder_set_member_name (builder, "back");
		json_builder_begin_object (builder);
		set_int_value(builder, "amount_x", data_passer->back->amount_x);
		set_int_value(builder, "first_amount_y", data_passer->back->first_amount_y);
		set_int_value(builder, "amount_pitch", data_passer->back->amount_pitch);
		set_int_value(builder, "total_y", data_passer->back->total_y);
		json_builder_end_object(builder); /* back */

		json_builder_end_object(builder); /* configuration */

		json_builder_end_object (builder); /* root */

		JsonNode *root = json_builder_get_root (builder);
		JsonGenerator *generator = json_generator_new ();
		GError *error = NULL;
		json_generator_set_root (generator, root);
		//json_generator_to_file (generator, CONFIG_FILE, &error);
		//if (error) {
		//	g_print("Unable to save the new configuration file.\n%s\nYou should rename the backup %s to %s\n",  error->message, CONFIG_FILE_BACKUP, CONFIG_FILE);
		//	g_error_free(error);
		//}

		g_object_unref(generator);
		g_object_unref(builder);

		/* Free memory allocated to the master and temporary list stores. */
		gtk_list_store_clear(data_passer->list_store_master);
		gtk_list_store_clear(data_passer->list_store_temporary);
		g_object_unref(data_passer->css_provider);
		g_free(data_passer);


	} else {
		g_print("Could not find first iter for saving the temporary list\n");
	}
}

/**
 * Returns the number of checks in the checks list store (the number of checks being deposited).
 * @param data_passer User data
 */
gint number_of_checks(Data_passer *data_passer) {
	return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(data_passer->checks_store), NULL);
}
