#include <gtk/gtk.h>
void on_app_activate(GApplication *app, gpointer data);
void build_list_store(gpointer account, gpointer list_builder_struct);
GtkWidget * make_tree_view(GtkListStore *list_store );
gpointer build_temporary_list(gpointer master_account, gpointer temporary_account);
GtkWidget * make_accounts_buttons_hbox(GtkListStore *list_store);
GtkWidget * get_child_from_parent (GtkWidget * parent, const gchar * child_name);

/**
 * Enum for declaring a list store of accounts.
*/
enum {
    ACCOUNT_NUMBER, /**< 0 */
    ACCOUNT_NAME,  /**< 1 */
    DESCRIPTION,  /**< 2 */
    CHECKBOX,  /**< 3 */
    N_COLUMNS  /**< 4 */
};

/**
 * Structure of an account record.
 */
typedef struct Accounts {
    gchar number[100]; /**< Account number */
    gchar name[100]; /**< Name of account holder */
    gchar description[500];  /**< Informal description of account */
} Account;

/**
 * Structure for copying the GS_List of accounts to another GS_List. The
 * instance of this struct is passed as user data to a function `build_list_store`.
*/
typedef struct list_builder_struct {
    GtkTreeIter iter; /**< Iter of the passed GS_List. */
    GtkListStore *list_store; /**< Pointer to the passed GS_List. */
} List_Builder_Struct;

GSList *read_account_numbers();


