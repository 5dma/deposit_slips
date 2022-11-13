#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"


void update_config (GtkWidget *widget, gpointer user_data) {

    gdouble *value = (gdouble *)user_data;

    gdouble spin_value = gtk_spin_button_get_value (GTK_SPIN_BUTTON(widget));
    *value = spin_value;

}

GtkWidget *make_configuration_view(Data_passer *data_passer) {
    GtkWidget *local_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label_layout = gtk_label_new("Layout");
    gtk_box_pack_start(GTK_BOX(local_vbox), label_layout, TRUE, TRUE, 0);

    GtkWidget *grid_layout_fields = gtk_grid_new();
    GtkWidget *x_coord = gtk_label_new("x:");
    GtkWidget *y_coord = gtk_label_new("y:");

    GtkWidget *label_right_margin_screen = gtk_label_new("Right margin screen");
    GtkWidget *spin_right_margin_screen = gtk_spin_button_new_with_range (0, 1000, 1);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(spin_right_margin_screen), data_passer->right_margin_screen);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), label_right_margin_screen, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), spin_right_margin_screen, 1, 0, 1, 1);
    g_signal_connect(GTK_WIDGET(spin_right_margin_screen),"value-changed", G_CALLBACK(update_config),&(data_passer->right_margin_screen));



    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), grid_layout_fields);
    gtk_box_pack_start(GTK_BOX(local_vbox), scrolled_window, TRUE, TRUE, 0);
    return local_vbox;
}

GtkWidget *make_font_view(Data_passer *data_passer) {
    GtkWidget *local_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label_fonts = gtk_label_new("Fonts");
    gtk_box_pack_start(GTK_BOX(local_vbox), label_fonts, TRUE, TRUE, 0);
}