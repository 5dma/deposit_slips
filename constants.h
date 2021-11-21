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

#endif