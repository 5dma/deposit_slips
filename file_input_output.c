#include <gtk/gtk.h>
#include "headers.h"

GSList *read_account_numbers() {
    GSList *local_struct_list = NULL;
    GError *error = NULL;
    gchar *input_file = g_build_filename(g_get_home_dir(), ".deposit_slip/deposit_slips.csv", NULL);
    gboolean input_file_exists = g_file_test(input_file, G_FILE_TEST_EXISTS);
    if (input_file_exists) {
        gchar *content;
        g_print("Found\n");
        if (g_file_get_contents(input_file, &content, NULL, &error)) {
            g_print("%s\n", content);
            gchar **lines[1000];
            gchar **account_records = g_strsplit(content, "\n", 1000);
            int i = 0;
            while (account_records[i] != NULL && (strlen(account_records[i]) > 0)) {
                gchar **single_account = g_strsplit(account_records[i], "\t", 1000);
                Account * account_entry = g_malloc(sizeof(Account));
                account_entry->number =  atoi(single_account[0]);
                strcpy(account_entry->description, single_account[1]);

                g_print ("The account_entry-> number: %d %s\n",account_entry->number, account_entry->description);
                local_struct_list = g_slist_append ( local_struct_list, account_entry);
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

