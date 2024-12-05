#include <gtk/gtk.h>
#include <constants.h>
#include <json-glib/json-glib.h>

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
	ROUTING_NUMBER, /**< 3 */
	CHECKBOX,  /**< 4 */
	N_COLUMNS  /**< 5 */
};

/**
 * Enum for declaring a list store of checks (only one column in this store).
*/
enum {
	CHECK_AMOUNT, /**< 0 */
	CHECK_RADIO,  /**< 1 */
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
	gchar routing[100];  /**< Routing number associated with bank account */
} Account;

void on_app_activate(GApplication *app, gpointer data);
void build_list_store(gpointer account, gpointer list_builder_struct);

GtkWidget * make_tree_view(GtkListStore *list_store );

GtkWidget * get_child_from_parent (GtkWidget * parent, const gchar * child_name);
void read_configuration_data(Data_passer *data_passer);
GtkWidget *make_accounts_buttons_hbox(Data_passer *data_passer);
GtkWidget *make_configuration_view(Data_passer *data_passer);
void write_config_free_memory (GtkWidget *window, gpointer user_data);

gboolean preview_deposit_amounts_front(GtkTreeModel *model,
							   GtkTreePath *path,
							   GtkTreeIter *iter,
							   gpointer data);

gboolean preview_deposit_amounts_back(GtkTreeModel *model,
							   GtkTreePath *path,
							   GtkTreeIter *iter,
							   gpointer data);

void print_deposit_slip(GtkButton *self, gpointer data);
gchar *comma_formatted_amount(gfloat amount);

GDestroyNotify free_gslist_account(gpointer data);

void read_coordinates(JsonObject *configuration_object, GHashTable *layout_hash, const gchar *hash_key);
void write_coordinates(JsonObject *configuration_object, GHashTable *layout_hash, const gchar *hash_key);
GtkWidget *make_slip_view(Data_passer *data_passer);
void update_label(GtkTreeView *tree_view, gpointer user_data);
void deposit_amount_edited(GtkCellRendererText *self, gchar *path, gchar *new_text, gpointer data);
void check_toggle_clicked(GtkCellRendererToggle *renderer, gchar *path, gpointer data);
void toggle_slip_view(GtkWidget *widget, gpointer data);
void add_check_row(GtkWidget *widget, gpointer data);
void delete_check_rows(GtkWidget *widget, gpointer data);
void draw_preview(GtkWidget *widget, cairo_t *cr, gpointer data);

/**
 * Current UI state of a selected check.
 */
typedef struct CheckSelections {
	GtkTreePath *path; /**< Path to selected check. */
	gboolean at_least_one_check_selected; /**< Indicates at least one check is selected */
} CheckSelection;


gint number_of_checks (Data_passer *data_passer);

