#include <gtk/gtk.h>
void on_app_activate(GApplication *app, gpointer data);
void build_list_store(gpointer account, gpointer list_builder_struct);
GtkWidget * make_tree_view(GtkListStore *list_store );
gpointer build_temporary_list(gpointer master_account, gpointer temporary_account);
GtkWidget * make_accounts_buttons_hbox();


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

/* Used only for initialization, passed as
a paramter to g_slist_foreach */
typedef struct list_builder_struct {
    GtkTreeIter iter;
    GtkListStore *list_store;
} List_Builder_Struct;

GSList *read_account_numbers();

typedef struct Barfs {
    gint number;
    gchar description[500];
} Barf;

