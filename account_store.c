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
                Account *account_entry = g_malloc(sizeof(Account));
                strcpy(account_entry->number, single_account[0]);
                strcpy(account_entry->description, single_account[1]);

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

    /* Add "New" entry to end of store. */
    Account *account_entry = g_malloc(sizeof(Account));
    strcpy(account_entry->number, "New");
    strcpy(account_entry->description, "");

    local_struct_list = g_slist_append(local_struct_list, account_entry);
    return local_struct_list;
}

/* Adds a passed Account structure to the list store of accounts */
void build_list_store(gpointer account, gpointer list_builder_struct) {
    GtkListStore *list_store = ((List_Builder_Struct *)list_builder_struct)->list_store;
    GtkTreeIter iter = ((List_Builder_Struct *)list_builder_struct)->iter;
    Account *local_account = (Account *)account;

    gtk_list_store_append(list_store, &iter);

    GError *error = NULL;

    GdkPixbuf * delete_icon =  gtk_icon_theme_load_icon (gtk_icon_theme_get_default(),"editdelete", GTK_ICON_SIZE_BUTTON, GTK_ICON_LOOKUP_USE_BUILTIN, NULL);

    if (error == NULL) {
        g_print("I have yes icon\n");
    } else {
        g_print("I have no icon\n");
    }

    gtk_list_store_set(list_store, &iter,
                       ACCOUNT_NUMBER, local_account->number,
                       DESCRIPTION, local_account->description,
                       DELETE_ICON, delete_icon,
                       -1);
}