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
	gdouble checks_other_items_label_x;  /**< x position of the rotated "Checks and other itmes.." label. */
	gdouble checks_other_items_label_y;  /**< y position of the rotated "Checks and other itmes.." label. */
	gdouble checks_other_items_font_size;  /**< Font size for the rotated "Checks and other itmes.." label.*/

	gdouble deposit_label_y;  /**< y position of the **Deposit** label (x-position determined by centering on the deposit slip).*/
	gdouble deposit_label_font_size;  /**< Font size for the **Deposit** label.*/

	gdouble date_name_address_label_x;  /**< x position of the **Date**, **Name**, and **Address** labels. */
	gdouble date_label_y; /**< y position of the **Date** label. */
	gdouble name_label_y; /**< y position of the **Name** label. */
	gdouble date_name_value_x; /**< x position of the **Name** and **Date** values. */
	gdouble date_value_y; /**< y position of the **Date** value. */
	gdouble name_value_y; /**< y position of the **Name** value. */
	
	gdouble date_name_line_x; /**<x position of the line for the date and name field. */
	gdouble date_name_line_y; /**<y position of the line for the date and name field. */
	gdouble date_line_length; /**< Length of the line for the date field. */
	gdouble name_line_length; /**< Length of the line for the name field. */
	gdouble date_name_address_label_font_size;  /**< Font size for the **Date**, **Name**, and **Address** labels.*/
	gdouble date_name_address_value_font_size;  /**< Font size for the **Date**, **Name**, and **Address** values.*/

	gdouble address_label_y; /**< y position of the **Address** label. */

	gdouble address_line_x; /**<x position of the line for the address field. */

	gdouble account_number_label_y; /**< y position of the **Account Number** label (x-position determined by centering on the series of squares holding the human-readable account number). */
	gdouble account_number_label_font_size; /**< Font size for the **Account** label.*/

	gdouble account_number_human_value_x; /**< x position of the human-readable account number. */
	gdouble account_number_human_value_y; /**< y position of the human-readable account number. */
	gdouble account_number_human_font_size; /**< Font size of the human-readable account number. */
	gdouble account_number_squares_x; /**< x position of the series of squares holding the human-readable account number. */
	gdouble account_number_squares_y; /**< y position of the series of squares holding the human-readable account number. */
	gdouble account_number_squares_width; /**< Width of each square holding the human-readable account number. */
	gdouble account_number_squares_height; /**< Height of each square holding the human-readable account number. */
	
	gdouble micr_routing_number_label_x; /**< x position of MICR routing number. */
	gdouble micr_routing_number_label_y; /**< y position of MICR routing number. */
	gdouble micr_account_number_label_x; /**< x position of MICR account number. */
	gdouble micr_serial_number_label_x; /**< x position of MICR serial number */
	gdouble micr_font_size; /**< Font size for MICR characters. */

	gdouble cash_label_x; /**< x position of **Cash** label (y position determined from `amount_boxes_y`). */
	gdouble checks_label_x; /**< x position of **Checks** label. */
	gdouble checks_label_y; /**< y position of **Checks** label. */
	gdouble checks_label_spacing; /**< Line spacing between each letter in the **Checks** label. */

	gdouble subtotal_label_y; /**< y position of **Subtotal** label (x-position determined by right-aligning on the **Cash** label). */
	gdouble less_cash_label_y; /**< y position of **Less Cash Received** label (x-position determined by right-aligning on the **Cash** label). */
	gdouble net_deposit_label_x; /**< x position of **Net Deposit** label. */
	gdouble net_deposit_label_y; /**< y position of **Net Deposit** label. */
	gdouble cash_label_font_size; /**< Font size for **Cash**, **Checks**, **Subtotal**, *Less Cash**, and **Net Deposit** labels. */
	gdouble dollar_label_x; /**< x position of **$** label. */
	gdouble dollar_label_y; /**< y position of **$** label. */

	gdouble checks_bracket_right_x; /**< x position of first checks bracket. */
	gdouble checks_bracket_width; /**< Width of checks bracket. */
	gdouble checks_bracket_height; /**< Height of checks bracket. */
	gdouble checks_bracket_spacing; /**< Spacing between the two checks brackets. */

	gdouble amount_boxes_x; /**< x position of the gray squares for amounts. */
	gdouble amount_boxes_y; /**< y position of the gray squares for amounts. */
	gdouble amount_boxes_width; /**< Width of each amount box */
	gdouble amount_boxes_height; /**< Height of each amount box. */
	gdouble amount_boxes_separator_height; /**< Height of each separator line in amount boxes. */

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
/** Structure containing printing coordinates on the front side of the deposit slip. See Front.*/
	Front *front;
/** Structure containing printing coordinates on the back side of the deposit slip. See Back.*/
	Back *back;
/** Object containing the CSS definitions.*/
	GtkCssProvider* css_provider;
 /** Soft gray text color for rendering the new account row. */	
	GdkRGBA new_account_foreground;
/** Black text color for rendering the new account row. */	
	GdkRGBA existing_account_foreground;
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

GtkWidget *make_tree_view(GtkListStore *list_store, Data_passer *data_passer);

GtkWidget *get_child_from_parent (GtkWidget * parent, const gchar * child_name);
void read_configuration_data(Data_passer *data_passer);
GtkWidget *make_accounts_buttons_hbox(Data_passer *data_passer);
GtkWidget *make_configuration_view(Data_passer *data_passer);
void free_memory (GtkWidget *window, gpointer user_data);

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

void free_gslist_account(gpointer data);

GtkWidget *make_slip_view(Data_passer *data_passer);
void update_label(GtkTreeView *tree_view, gpointer user_data);
void deposit_amount_edited(GtkCellRendererText *self, gchar *path, gchar *new_text, gpointer data);
void check_toggle_clicked(GtkCellRendererToggle *renderer, gchar *path, gpointer data);
void toggle_slip_view(GtkWidget *widget, gpointer data);
void add_check_row(GtkWidget *widget, gpointer data);
void delete_check_rows(GtkWidget *widget, gpointer data);
void draw_preview(GtkWidget *widget, cairo_t *cr, gpointer data);
gint number_of_checks (Data_passer *data_passer);

