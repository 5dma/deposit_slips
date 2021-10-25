#include <gtk/gtk.h>
void on_app_activate(GApplication *app, gpointer data);

typedef struct Accounts {
    int number;
    gchar description[500];
} Account;

typedef struct list_builder_struct {
    GtkTreeIter iter;
    GtkListStore *list_store;
} List_Builder_Struct;

GSList *read_account_numbers();
