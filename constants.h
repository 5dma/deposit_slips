/**
 * @file constants.h
 * @brief Constants used in multiple files.
*/
#ifndef __CONSTANTS
#define __CONSTANTS

static const char NEW_NUMBER[] = "(Number)"; /**< Dummy account number string for the new account row. */
static const char NEW_DESCRIPTION[] = "(Description)"; /**< Dummy description string for the new account row. */
static const char NEW_NAME[] = "(Name)"; /**<< Dummy name string for the new account row. */
static const struct _GdkRGBA NEW_ACCOUNT_FOREGROUND = {0.35,0.35,0.35,1}; /**< Soft gray for rendering the new account row. */
static const struct _GdkRGBA EXISTING_ACCOUNT_FOREGROUND = {0,0,0,1}; /**< Black for rendering the new account row. */

static const char BUTTON_NAME_REVERT[] = "btnAccountsRevert"; /**< Name for the Revert button. */
static const char BUTTON_NAME_ADD[] = "btnAccountsAdd"; /**< Name for the Add button. */
static const char BUTTON_NAME_DELETE[] = "btnAccountsDelete";  /**< Name for the Delete button. */
static const char BUTTON_NAME_SAVE[] = "btnAccountsSave";  /**< Name for the Save button. */
static const char HBOX_ACCOUNT_BUTTONS[] = "hboxAccountButtons";  /**< Name for the hbox containing the above buttons. */

static const char BUTTON_CHECK_ADD[] = "btnChecksAdd"; /**< Name for the Add Check button. */
static const char BUTTON_CHECK_DELETE[] = "btnChecksDelete";  /**< Name for the Delete Checks button. */
static const char BUTTON_SLIP_PRINT[] = "btnSlipPrint";  /**< Name for the Print Slip button. */
static const char LABEL_ACCOUNT_DESCRIPTION[] = "lblAccountDescription";  /**< Name for the account description label. */
static const char NEW_AMOUNT[] = "0.00"; /**< Dummy amount string for the new check row. */

static gint KEY_LIST_STORE_MASTER = 0; /**< Hash key for the pointer to master list store. */
static gint KEY_LIST_STORE_TEMPORARY = 1; /**< Hash key for the pointer to temporary list store. */
static gint KEY_CAIRO_CONTEXT = 2; /**< Hash key for the pointer to cairo context. */
static gint KEY_CHECK_TREE_VIEW = 3; /**< Hash key for the pointer to checks tree view. */
static gint KEY_DRAWING_AREA = 4; /**< Hash key for the pointer to the drawing area. */
static gint KEY_DRAWING_ACTION = 5; /**< Hash key for the drawing action to take. One of the  */


static gint DRAWING_OPERATION_BACKGROUND = 0; /**< Indicates redrawing the background */
static gint DRAWING_OPERATION_CHECKS = 1; /**< Indicates drawing the check amounts  */



#endif