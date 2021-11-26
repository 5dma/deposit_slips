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

/**
 * Structure holding two list stores: master and temporary. This structure
 * is used to pass both list stores to various functions.
 * \sa list_builder_struct
 */
typedef struct List_Stores {
    GSList *list_store_master; /**< Master list (the one stored on disk) */
    GSList *list_store_temporary; /**< Temporary list (the one stored in memory) */
    List_Builder_Struct *list_builder_struct; /**< List builder structure associated with the tree view */
} List_Store_Struct;

/**
 * Enum for positions inside a List_Store_Struct. Used with g_ptr_array_index().
*/
enum {
    POSITION_LIST_STORE_MASTER, /**< 0 */
    POSITION_LIST_STORE_TEMPORARY,  /**< 1 */
    POSITION_LIST_BUILDER_STRUCT /**< 2 */
};


void on_app_activate(GApplication *app, gpointer data);
void build_list_store(gpointer account, gpointer list_builder_struct);

GtkWidget * make_tree_view(GtkListStore *list_store );
gpointer build_temporary_list(gpointer master_account, gpointer temporary_account);
GtkWidget * get_child_from_parent (GtkWidget * parent, const gchar * child_name);
GSList *read_account_numbers();
GtkWidget * make_accounts_buttons_hbox(GPtrArray *list_store_struct);
void free_memory (GtkWidget *window, gpointer user_data);

#endif

