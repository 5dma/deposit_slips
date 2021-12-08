#include <gtk/gtk.h>
#ifndef __HEADER
#define __HEADER

/**
 * @file headers.h
 * @brief C headers.
*/

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
 * Enum for declaring a list store of checks (only one column in this store).
*/
enum {
    CHECK_AMOUNT, /**< 0 */
    CHECK_CHECKBOX,  /**< 1 */
    SLIP_COLUMNS /**< 2 */
};

/**
 * Structure of an account record.
 */
typedef struct Accounts {
    /** Account number */
    gchar number[100]; 
    gchar name[100]; /**< Name of account holder */
    gchar description[500];  /**< Informal description of account */
} Account;

void on_app_activate(GApplication *app, gpointer data);
void build_list_store(gpointer account, gpointer list_builder_struct);

GtkWidget * make_tree_view(GtkListStore *list_store );
gpointer build_temporary_list(gpointer master_account, gpointer temporary_account);
GtkWidget * get_child_from_parent (GtkWidget * parent, const gchar * child_name);
GSList *read_account_numbers();
GtkWidget *make_accounts_buttons_hbox(GHashTable *pointer_passer);
void free_memory (GtkWidget *window, gpointer user_data);

void draw_single_amount(GHashTable *pointer_passer, gchar *new_amount);

gboolean print_deposit_amounts(GtkTreeModel *model,
                               GtkTreePath *path,
                               GtkTreeIter *iter,
                               gpointer data);

void draw_background( GtkCellRendererText* self, gchar* path, gchar* new_text, gpointer data);

#endif

