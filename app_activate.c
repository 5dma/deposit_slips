#include <gtk/gtk.h>
#include <headers.h>
#include <constants.h>

/**
 * @file app_activate.c
 * @brief Builds the view.
 */

/**
 * Function that starts the GTK loop.
 * @param app Pointer to the GTK application
 * @param data Pointer to user data, `NULL` in this case.
 */
void on_app_activate(GApplication *app, gpointer data) {
	/* Structure for passing data to functions and callbacks */
	Data_passer *data_passer = g_new(Data_passer, 1);

	data_passer->list_store_master = NULL;
	data_passer->list_store_temporary = NULL;
	data_passer->cairo_context = NULL;
	data_passer->check_tree_view = NULL;
	data_passer->front_slip_active = TRUE;
	data_passer->drawing_area = NULL;
	data_passer->checks_store = NULL;
	data_passer->total_deposit = 0;
	data_passer->total_back_side = 0;
	data_passer->checks_accounts_treeview = NULL;
	data_passer->application_window = NULL;
	data_passer->radio_renderer = NULL;
	data_passer->btn_go_to_first = NULL;
	data_passer->btn_go_to_last = NULL;
	data_passer->front = (Front *)g_malloc(sizeof(Front));
	data_passer->back = (Back *)g_malloc(sizeof(Back));
	data_passer->new_account_foreground.red = 0.35; 
	data_passer->new_account_foreground.green = 0.35; 
	data_passer->new_account_foreground.blue = 0.35; 
	data_passer->new_account_foreground.alpha = 1; 

	data_passer->existing_account_foreground.red = 0; 
	data_passer->existing_account_foreground.green = 0; 
	data_passer->existing_account_foreground.blue = 0; 
	data_passer->existing_account_foreground.alpha = 1; 



	data_passer->css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(data_passer->css_provider, "/home/abba/programming/c_programs/deposit_slips/styles.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(data_passer->css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

	GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	data_passer->application_window = window;

	GtkWidget *notebook = gtk_notebook_new();

	GtkWidget *vbox_slip = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	GtkWidget *vbox_accounts = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

	GtkWidget *label_slip = gtk_label_new("Slip");
	GtkWidget *label_account = gtk_label_new("Accounts");

	gtk_widget_set_name (vbox_slip, "vbox_slip");
	gtk_widget_set_name (vbox_accounts, "vbox_accounts");


	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_slip, label_slip);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_accounts, label_account);


	/* This memory is free after application is destroyed in `free_memory()`. */
	data_passer->list_store_master = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	//   data_passer->list_store_master = list_store_master;
	/* This memory is free after application is destroyed in `free_memory()`. */
	data_passer->list_store_temporary = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	//   data_passer->list_store_temporary = list_store_temporary;

	/* Read configuration data from disk. */
	read_configuration_data(data_passer);

	/* Make the view for the Accounts tab. */
	GtkWidget *accounts_tab_tree = make_tree_view(data_passer->list_store_temporary, data_passer);
	/* Make the view for the Slip  tab. */
	GtkWidget *slips_tab_tree = make_slip_view(data_passer);

	/* Upon destroying the application, free memory in data structures in pointer_passer. */
	g_signal_connect(window, "destroy", G_CALLBACK(free_memory), data_passer);

	/* Make buttons under the accounts in the Accounts tab. This should be part of make_tree_view. */
	GtkWidget *accounts_buttons_hbox = make_accounts_buttons_hbox(data_passer);

	/* Place the buttons in the Accounts tab. Again, should be part of make_tree_view. The TRUE parameter ensures the treeview maintains its initial height, even after deleting rows. */
	gtk_box_pack_start(GTK_BOX(vbox_accounts), accounts_tab_tree, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_accounts), accounts_buttons_hbox, FALSE, FALSE, 0);
	
	/* Place the slip view in the Slip tab. */
	gtk_box_pack_start(GTK_BOX(vbox_slip), slips_tab_tree, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(window), notebook);

	gtk_widget_show_all(GTK_WIDGET(window));

	/* Select first row in accounts list of slips tab. */
	GtkTreePath *path = gtk_tree_path_new_from_string("0");
	GtkTreeView *account_tree_view_deposit_slip = GTK_TREE_VIEW(data_passer->checks_accounts_treeview);
	gtk_tree_view_set_cursor(account_tree_view_deposit_slip, path, NULL, FALSE);

	/* Emit row-activated signal on the account tree in the slips tab to populate the description correctly.  */
	g_signal_emit_by_name(G_OBJECT(data_passer->checks_accounts_treeview), "row-activated", NULL);

	/* Draw the preview. */
	gtk_widget_queue_draw(data_passer->drawing_area);
}
