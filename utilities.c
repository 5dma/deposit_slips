#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

#include "constants.h"
#include "headers.h"

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
Frees memory allocated to a GSList of accounts. Called from write_config_free_memory().
* @param data Pointer to user data, which is to an Account entry in a GSList.
* @return A `GDestroyNotify` object.
*/
GDestroyNotify free_gslist_account(gpointer data) {
    Account *account = (Account *)data;
    g_free(account);
}

/**
 * Writes a CSV string to `~/.deposit_slip/deposit_slips.csv`.
 * @param generator A JsonGenerator used to write a JSON object to a file. See
 * \sa [json_generator_to_file](https://gnome.pages.gitlab.gnome.org/json-glib/method.Generator.to_file.html)
 * \sa [JsonGenerator](https://gnome.pages.gitlab.gnome.org/json-glib/class.Generator.html)
 */
void save_account_numbers(JsonGenerator *generator) {
    GError *error = NULL;
    GtkTreeIter iter;

    /* Establish directory where accounts are saved, and create it if necessary. */
    gchar *save_directory = g_build_filename(g_get_home_dir(), ".deposit_slip/", NULL);
    if (!g_file_test(save_directory, G_FILE_TEST_EXISTS)) {
        g_mkdir_with_parents(save_directory, 755);
    }
    g_free(save_directory);

    gchar *output_file = g_build_filename(g_get_home_dir(), ".deposit_slip/deposit_slips.json", NULL);
    gboolean write_successful = json_generator_to_file(generator, output_file, &error);

    if (!write_successful) {
        g_print("Could not write the new list, so the previous master list is still will show when restarting this program.\n");
    }
    g_free(output_file);
}

/**
* Writes the configuration file, and frees memory in the master and temporary account lists and other string data in a `Data_passer`. This callback fires after the user destroys the main application window.
  \sa save_account_numbers()
* @param window The parent node.
* @param data Pointer to user data
*/
void write_config_free_memory(GtkWidget *window, gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;

    GtkTreeIter iter_master;
    GtkTreeIter iter_temporary;

    gboolean found_first_iter_temporary = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data_passer->list_store_temporary), &iter_temporary);

    if (found_first_iter_temporary) {
        /* gchar pointers for reading the entries from the master store. These variables are freed below. */
        gchar *account_number = NULL;
        gchar *account_name = NULL;
        gchar *account_description = NULL;
        gchar *routing_number = NULL;

        /* A JSON array of accounts. Memory freed below. */
        JsonArray *account_array = json_array_new();

        gtk_list_store_clear(data_passer->list_store_master);

        while (gtk_list_store_iter_is_valid(data_passer->list_store_temporary, &iter_temporary)) {
            gtk_tree_model_get(GTK_TREE_MODEL(data_passer->list_store_temporary), &iter_temporary,
                               ACCOUNT_NUMBER, &account_number,
                               ACCOUNT_NAME, &account_name,
                               DESCRIPTION, &account_description,
                               ROUTING_NUMBER, &routing_number,
                               -1);

            /* Add the entry from the master store to the temporary store */
            gtk_list_store_append(data_passer->list_store_master, &iter_master);

            gtk_list_store_set(data_passer->list_store_master, &iter_master,
                               ACCOUNT_NUMBER, account_number,
                               ACCOUNT_NAME, account_name,
                               DESCRIPTION, account_description,
                               ROUTING_NUMBER, routing_number,
                               -1);

            /* Create a JSON object for the current account. */
            JsonObject *account = json_object_new(); /* Memory freed below. */
            json_object_set_string_member(account, "account", account_number);
            json_object_set_string_member(account, "name", account_name);
            json_object_set_string_member(account, "description", account_description);
            json_object_set_string_member(account, "routing", routing_number);

            /* Add the JSON object to the JSON array of accounts. */
            json_array_add_object_element(account_array, account);
            // json_object_unref(account); /* This statement causes a seg fault. Why? Doesn't the json_array_add_object_element increase the reference count?*/

            gtk_tree_model_iter_next(GTK_TREE_MODEL(data_passer->list_store_temporary), &iter_temporary);
        }

        g_free(account_number);
        g_free(account_name);
        g_free(account_description);
        g_free(routing_number);

        /* Create the top-level JSON object. */
        JsonObject *object = json_object_new();
        json_object_set_array_member(object, "accounts", account_array);

        /* Create a node for the top-level JSON object, from which we create a JsonGenerator. */
        JsonNode *node = json_node_new(JSON_NODE_OBJECT);
        json_node_set_object(node, object);

        JsonGenerator *generator;
        generator = json_generator_new();
        json_generator_set_root(generator, node);

        /* Create the layout object. */
        JsonObject *layout_object = json_object_new();
        json_object_set_int_member(layout_object, "right_margin_screen", data_passer->right_margin_screen);
        json_object_set_int_member(layout_object, "right_margin_print_front", data_passer->right_margin_print_front);
        json_object_set_int_member(layout_object, "right_margin_print_back", data_passer->right_margin_print_back);
        json_object_set_int_member(layout_object, "font_size_sans_serif", data_passer->font_size_sans_serif);
        json_object_set_int_member(layout_object, "font_size_monospace", data_passer->font_size_monospace);
        json_object_set_string_member(layout_object, "font_family_sans", data_passer->font_family_sans);
        json_object_set_string_member(layout_object, "font_face_micr", data_passer->font_face_micr);
        json_object_set_string_member(layout_object, "font_family_mono", data_passer->font_family_mono);

        /* Add the layout object to the root object. */
        json_object_set_object_member(object, "layout", layout_object);

        write_coordinates(layout_object, data_passer->layout, "name_label");
        write_coordinates(layout_object, data_passer->layout, "name_value");
        write_coordinates(layout_object, data_passer->layout, "account_label");
        write_coordinates(layout_object, data_passer->layout, "account_value");
        write_coordinates(layout_object, data_passer->layout, "date_value");
        write_coordinates(layout_object, data_passer->layout, "micr_account_value");
        write_coordinates(layout_object, data_passer->layout, "total_value");
        write_coordinates(layout_object, data_passer->layout, "back_side_value");
        write_coordinates(layout_object, data_passer->layout, "back_side_subtotal");
        write_coordinates(layout_object, data_passer->layout, "front_values");
        write_coordinates(layout_object, data_passer->layout, "back_values");

        /* Go write the JsonGenerator to a file. */
        save_account_numbers(generator);

        /* Free memory allocated to the JSON object. */
        //   json_object_unref(layout_object);
        g_object_unref(generator);
        json_object_unref(object);
        json_array_unref(account_array);

        /* Free memory allocated to the master and temporary list stores. */
        gtk_list_store_clear(data_passer->list_store_master);
        gtk_list_store_clear(data_passer->list_store_temporary);
        g_free(data_passer->font_face_micr);
        g_free(data_passer);

    } else {
        g_print("Could not find first iter for saving the temporary list\n");
    }
}

/**
 * Reads coordinates from a `JsonObject` into a hash of coordinates.
 * @param layout_object JSON object containing the coordinates from the configuration file.
 * @param layout_hash GHashTable containing coordinates.
 * @param hash_key Key into `layout_hash`.
 */
void read_coordinates(JsonObject *layout_object, GHashTable *layout_hash, const gchar *hash_key) {
    JsonObject *x_y_object = json_object_get_object_member(layout_object, hash_key);
    Coordinates *coordinates = (Coordinates *)g_hash_table_lookup(layout_hash, hash_key);
    coordinates->x = json_object_get_int_member(x_y_object, "x");
    coordinates->y = json_object_get_int_member(x_y_object, "y");
}

/**
 * Writes coordinates into a `JsonObject` from a hash of coordinates.
 * @param layout_object JSON object containing the coordinates from the configuration file.
 * @param layout_hash GHashTable containing coordinates.
 * @param hash_key Key into `layout_hash`.
 */
void write_coordinates(JsonObject *layout_object, GHashTable *layout_hash, const gchar *hash_key) {
    JsonObject *coordinate_object = json_object_new();
    Coordinates *coordinates = (Coordinates *)g_hash_table_lookup(layout_hash, hash_key);

    json_object_set_int_member(coordinate_object, "x", coordinates->x);
    json_object_set_int_member(coordinate_object, "y", coordinates->y);
    json_object_set_object_member(layout_object, hash_key, coordinate_object);
}

/**
 * Returns the number of checks in the checks list store (the number of checks being deposited).
 * @param data_passer User data
 */
gint number_of_checks(Data_passer *data_passer) {
    return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(data_passer->checks_store), NULL);
}
