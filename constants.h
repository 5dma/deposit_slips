/**
 * @file constants.h
 * @brief Constants used in multiple files.
*/
#ifndef __CONSTANTS
#define __CONSTANTS

 /** Dummy account number string for the new account row. */
static const char NEW_NUMBER[] = "(Number)";
/** Dummy description string for the new account row. */
static const char NEW_DESCRIPTION[] = "(Description)"; 
 /** Dummy name string for the new account row. */
static const char NEW_NAME[] = "(Name)";
 /** Dummy routing string for the new account row. */
static const char NEW_ROUTING[] = "(Routing No)";
 /** Soft gray text color for rendering the new account row. */
static const struct _GdkRGBA NEW_ACCOUNT_FOREGROUND = {0.35,0.35,0.35,1}; 
/** Black text color for rendering the new account row. */
static const struct _GdkRGBA EXISTING_ACCOUNT_FOREGROUND = {0,0,0,1}; 


/** Name for the Revert button. */
static const char BUTTON_NAME_REVERT[] = "btnAccountsRevert"; 
/** Name for the Add button. */
static const char BUTTON_NAME_ADD[] = "btnAccountsAdd"; 
/** Name for the Delete button. */
static const char BUTTON_NAME_DELETE[] = "btnAccountsDelete";
 /** Name for the Save button. */
 static const char BUTTON_NAME_SAVE[] = "btnAccountsSave"; 
 /** Name for the hbox containing the above buttons. */
static const char HBOX_ACCOUNT_BUTTONS[] = "hboxAccountButtons";  

/** Name for the Add Check button. */
static const char BUTTON_CHECK_ADD[] = "btnChecksAdd";
/** Name for the Delete Checks button. */
static const char BUTTON_CHECK_DELETE[] = "btnChecksDelete";  
/** Name for the Print Slip button. */
static const char BUTTON_SLIP_PRINT[] = "btnSlipPrint"; 
 /** Name for the account description label. */ 
static const char LABEL_ACCOUNT_DESCRIPTION[] = "lblAccountDescription"; 
/** Dummy amount string for the new check row. */
static const char NEW_AMOUNT[] = "0.00"; 

/** Right margin of on-screen deposit slip. */
static const double RIGHT_MARGIN_SCREEN = 490;
/** Right margin of printed deposit slip, front side. */
static const double RIGHT_MARGIN_PRINT_FRONT = 5;
/** Right margin of printed deposit slip, rear side. */
static const double RIGHT_MARGIN_PRINT_BACK = 180;
/**
 * Structure for passing data between functions.
*/ 
typedef struct {
  /** Master list store of checking accounts. */
    GtkListStore *list_store_master; 
/** Temporary list store of checking accounts. */
	GtkListStore *list_store_temporary; 
/** Pointer to cairo context for previewing and printing checks. */
	cairo_t *cairo_context; 
/** Pointer to checks tree view. */
	GtkWidget *check_tree_view; 
/** Indicates currently drawing the front of the slip. */
	gboolean front_slip_active;
/** Pointer to drawing area for slip. */
	GtkWidget *drawing_area; 
/** List store of the checks.  */
	GtkListStore *checks_store; 
/** Sum of all checks deposited.  */
	gfloat total_deposit; 
/** Sum of all checks listed on back of slip.  */
	gfloat total_back_side; 
/** Tree view of accounts in the deposit slip tab.  */
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
} Data_passer;


/** Font size for dynamic info (name, account number, date) on printed slip. */
static gint FONT_SIZE_PRINT_DYNAMIC = 11;
/** Font size for amounts on printed slip. */
static gint FONT_SIZE_AMOUNT = 12;

#endif