#include <gtk/gtk.h>
void on_app_activate(GApplication *app, gpointer data);
void build_list_store(gpointer account, gpointer list_builder_struct);


enum {
    ACCOUNT_NUMBER,
    DESCRIPTION,
    DELETE_ICON,
    N_COLUMNS
};

typedef struct Accounts {
    gchar number[100];
    gchar description[500];
} Account;

typedef struct list_builder_struct {
    GtkTreeIter iter;
    GtkListStore *list_store;
} List_Builder_Struct;

GSList *read_account_numbers();
