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


/** Hash key for the pointer to master list store. */
static gint KEY_LIST_STORE_MASTER = 0; 
/** Hash key for the pointer to temporary list store. */
static gint KEY_LIST_STORE_TEMPORARY = 1; 
/** Hash key for the pointer to cairo context. */
static gint KEY_CAIRO_CONTEXT = 2; 
/** Hash key for the pointer to checks tree view. */
static gint KEY_CHECK_TREE_VIEW = 3; 
/** Hash key for the pointer to the drawing area. */
static gint KEY_DRAWING_AREA = 4; 
/** Hash key for the drawing action to take. One of the  */
static gint KEY_DRAW_HANDLER = 6; 
/** Hash key for the list store of the checks  */
static gint KEY_CHECKS_STORE = 7; 
/** Hash key for the sum of all checks deposited.  */
static gint KEY_TOTAL_DEPOSIT = 8; 
/** Hash key for the tree view of accounts in the deposit slip tab.  */
static gint KEY_CHECKS_ACCOUNTS_TREEVIEW = 9;
/** Hash key for the tree view of accounts in the deposit slip tab.  */
static gint KEY_CHECK_CELL_RENDERER = 10;
/** Hash key for the tree view of accounts in the deposit slip tab.  */
static gint KEY_AT_LEAST_ONE_CHECK = 11;
/** Hash key for the pointer to the application window.  */
static gint KEY_APPLICATION_WINDOW = 12;
/** Hash key for the pointer to boolean indicating output is to paper or screen.  */
static gint KEY_OUTPUT_TO_PAPER = 13;


#endif