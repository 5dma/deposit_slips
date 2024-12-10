#pragma once
/**
 * @file constants.h
 * @brief Constants used in multiple files.
*/

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

