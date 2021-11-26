#include <gtk/gtk.h>
#include <string.h>
#include "../headers.h"

/**
 * @file account_store.c
 * Sets up the store for accounts.
*/
/** 
 * Reads a CSV file in `~/.deposit_slip/deposit_slips.csv` into a `GSList`.
 * 
 * @return Returns a `GSList *` of account numbers read from disk.
*/
GSList *read_account_numbers() {
    GSList *local_struct_list = NULL;
    GError *error = NULL;

    /* Memory is freed at end of this function */
    gchar *input_file = g_build_filename(g_get_home_dir(), ".deposit_slip/deposit_slips.csv", NULL);
    gboolean input_file_exists = g_file_test(input_file, G_FILE_TEST_EXISTS);
    if (input_file_exists) {
        gchar *content;
        if (g_file_get_contents(input_file, &content, NULL, &error)) {
            g_print("%s\n", content);
            gchar **lines[1000];
            /* Split input string a newlines, memory is freed below. */
            gchar **account_records = g_strsplit(content, "\n", 1000);
            int i = 0;
            /* For each line, instantiate an account structure and add it to the GSList of structures */
            while (account_records[i] != NULL && (strlen(account_records[i]) > 0)) {
                gchar **single_account = g_strsplit(account_records[i], "\t", 1000);
                /* Memory is freed below */
                Account *account_entry = g_new(Account,1);
                strcpy(account_entry->number, single_account[0]);
                strcpy(account_entry->name, single_account[1]);
                strcpy(account_entry->description, single_account[2]);

                g_print("The account_entry-> number: %s %s\n", account_entry->number, account_entry->description);
                local_struct_list = g_slist_append(local_struct_list, account_entry);
                g_strfreev(single_account);
                i++;
            }
            g_free(content);
            g_strfreev(account_records);
        } else {
            g_print("There was an error reading the file.\n");
            g_print("%s\n", error->message);
        }
    } else {
        g_print("Input file does not exist.\n");
    }
    g_free(input_file);

    return local_struct_list;
}

/**
 *  Adds a passed Account structure to the list store of accounts
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
                       CHECKBOX, FALSE,
                       -1);
}

/**
 Adds a passed Account structure to the list store of accounts. This function
 is a callback passed from a `g_slist_copy_deep`.
 @return A malloced pointer to a copy of the passed master account record.
*/
gpointer build_temporary_list(gpointer item, gpointer user_data) {

    Account *master_account = item;

    /* Copy master account into a new temporary account */
    Account *temp_account = g_new (Account, 1);
    strcpy(temp_account->number, master_account->number);
    strcpy(temp_account->name, master_account->name);
    strcpy(temp_account->description, master_account->description);

    return temp_account;
}