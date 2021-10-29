#include <gtk/gtk.h>
void on_app_activate(GApplication *app, gpointer data);
void build_list_store(gpointer account, gpointer list_builder_struct);
GtkWidget * make_tree_view(GtkListStore *list_store );


enum {
    ACCOUNT_NUMBER,
    DESCRIPTION,
    CHECKBOX,
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
