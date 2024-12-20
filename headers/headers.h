#include <gtk/gtk.h>
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
 * Enum for declaring a list store of checks.
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


/**
 * Structure holding printing coordinates on the front side of the deposit slip. Corresponds to the `front` stanza in the configuration file.
*/ 
typedef struct Front {
 	gdouble name_account_label_x;  /**< x position of the **Name** and **Account** labels. */
	gdouble name_account_date_value_x; /**< x position of the **Name**, **Account**, and **Date** values. */
	gdouble name_y; /**< y position of the **Name** value. */
	gdouble account_y; /**< y position of the **Account** value. */
	gdouble date_y; /**< y position of the **Date** value. */
	gdouble micr_y; /**< y position of the account's MICR rendition. */
	gdouble micr_x; /**< x position of the account's MICR rendition. */
	gdouble first_amount_y;  /**< y position of the first amount. */
	gdouble amount_pitch;  /**< Line spacing between amounts. */
	gdouble subtotal_y;  /**< y position of the subtotal carried from the back side of the deposit slip. */
	gdouble total_y;  /**< y position of the total. */
	gdouble amount_x;  /**< x position of all amounts. */
} Front;

/**
 * Structure holding printing coordinates on the back side of the deposit slip. Corresponds to the `back` stanza in the configuration file.
*/ 
typedef struct Back {
 	gdouble amount_x; /**< x position of all amounts. */
	gdouble first_amount_y;  /**< y position of the first amount. */
	gdouble amount_pitch;  /**< Line spacing between amounts. */
	gdouble total_y;  /**< y position of the total. */
} Back;


/**
 * Structure for passing data between functions.
*/ 
typedef struct Data_passer {
/** Pointer to master list store of checking accounts. */
	GtkListStore *list_store_master; 
/** Pointer to temporary list store of checking accounts. */
	GtkListStore *list_store_temporary; 
/** Pointer to cairo context for previewing and printing checks. */
	cairo_t *cairo_context; 
/** Pointer to checks tree view. */
	GtkWidget *check_tree_view; 
/** Indicates currently drawing the front of the slip. */
	gboolean front_slip_active;
/** Pointer to drawing area for slip. */
	GtkWidget *drawing_area; 
/** Pointer to list store of the checks.  */
	GtkListStore *checks_store; 
/** Sum of all checks deposited.  */
	gfloat total_deposit; 
/** Sum of all checks listed on back of slip.  */
	gfloat total_back_side; 
/** Pointer to tree view of accounts in the deposit slip tab.  */
	GtkWidget *checks_accounts_treeview;
/** Pointer to the application window.  */
	GtkWidget *application_window;
/** Pointer to the renderer for radio buttons.  */
	GtkCellRenderer *radio_renderer;
/** Pointer to the delete button in the checks view.  */
	GtkWidget *btn_checks_delete;
/** Pointer to the go_to_first button in the checks view.  */
	GtkWidget *btn_go_to_first;
/** Pointer to the go_to_last button in the checks view.  */
	GtkWidget *btn_go_to_last;
/** Font size for printing the text items, such as name and account number.  */
	gdouble font_size_sans_serif;
/** Font size for printing the amounts.  */
	gdouble font_size_monospace;
/** Font face, sans serif.  */
	gchar font_family_sans[100];
/** Font face, MICR.  */
	gchar font_face_micr[100];
/** Font face, mono.  */
	gchar font_family_mono[100];
/** Identifier of the sans-serif font selection button. The identifier is the pointer's memory location. */
	gpointer btn_font_sans_serif;
/** Identifier of the monospace font selection button. The identifier is the pointer's memory location. */
	gpointer btn_font_monospace;
/** Scaling for font size, applied to the static Name and Account labels. */	
	gdouble font_size_static_label_scaling;
/** Flag indicating the Name and Account labels are printed on the deposit slip.*/
	gboolean print_name_account_labels;
/** Structure containing printing coordinates on the front side of the deposit slip. See Front.*/
	Front *front;
/** Structure containing printing coordinates on the back side of the deposit slip. See Back.*/
	Back *back;
} Data_passer;

/**
 * Current UI state of a selected check.
 */
typedef struct CheckSelections {
	GtkTreePath *path; /**< Path to selected check. */
	gboolean at_least_one_check_selected; /**< Indicates at least one check is selected */
} CheckSelection;

void on_app_activate(GApplication *app, gpointer data);
void build_list_store(gpointer account, gpointer list_builder_struct);

GtkWidget *make_tree_view(GtkListStore *list_store );

GtkWidget *get_child_from_parent (GtkWidget * parent, const gchar * child_name);
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

GtkWidget *make_slip_view(Data_passer *data_passer);
void update_label(GtkTreeView *tree_view, gpointer user_data);
void deposit_amount_edited(GtkCellRendererText *self, gchar *path, gchar *new_text, gpointer data);
void check_toggle_clicked(GtkCellRendererToggle *renderer, gchar *path, gpointer data);
void toggle_slip_view(GtkWidget *widget, gpointer data);
void add_check_row(GtkWidget *widget, gpointer data);
void delete_check_rows(GtkWidget *widget, gpointer data);
void draw_preview(GtkWidget *widget, cairo_t *cr, gpointer data);
gint number_of_checks (Data_passer *data_passer);

