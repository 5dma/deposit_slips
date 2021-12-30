#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include <string.h>

#include "../headers.h"

/**
 * @file account_store.c
 * @brief Sets up the store for accounts.
*/

/** 
 * Reads a CSV file in `~/.deposit_slip/deposit_slips.csv` into a `GSList`.
 * @return Returns a `GSList *` of account numbers read from disk.
*/
GSList *read_account_numbers() {
    GSList *local_struct_list = NULL;
    JsonParser *parser;
    JsonNode *root;
    GError *error = NULL;

    /* Memory is freed at end of this function */
    gchar *input_file = g_build_filename(g_get_home_dir(), ".deposit_slip/deposit_slips.json", NULL);
    gboolean input_file_exists = g_file_test(input_file, G_FILE_TEST_EXISTS);
    if (input_file_exists) {
        parser = json_parser_new();
        error = NULL;
        json_parser_load_from_file(parser, input_file, &error);

        if (error) {
            g_print("Unable to parse `%s': %s\n", input_file, error->message);
            g_error_free(error);
            g_object_unref(parser);
            return NULL;
        }

        JsonNode *root = json_parser_get_root(parser);
        JsonObject *obj = json_node_get_object(root);
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

        g_object_unref(parser);

    } else {
        g_print("Input file does not exist.\n");
    }
    g_free(input_file);

    return local_struct_list;
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
 * Callback fired while itering each member of a `GSList` of list of
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
