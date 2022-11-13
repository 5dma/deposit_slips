#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

/**
 * Callback fired when the user changes a value in a spin box. The callback stores the changed value inside `Data_passer`.
 * @param widget Pointer to the spin box whose value changed.
 * @param user_data Pointer to the value inside `Data_passer`.
 */
void update_config(GtkWidget *widget, gpointer user_data) {
    gdouble *value = (gdouble *)user_data;
    gdouble spin_value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    *value = spin_value;
}

/**
 * Adds a field and one spin button to the passed grid.
 * @param label Field's label.
 * @param value Spin button's initial value.
 * @param top Row in which the field is placed within the grid.
 * @param grid_layout_fields Grid into which the field is added.
 */
void add_one_value_configuration(const gchar *label,
                                 double *value,
                                 gint top,
                                 GtkWidget *grid_layout_fields) {
    GtkWidget *label_field_name = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(label_field_name), 0.0);
    GtkWidget *spin_button = gtk_spin_button_new_with_range(0, 1000, 1);
    gtk_entry_set_alignment(GTK_ENTRY(spin_button), 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), *value);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), label_field_name, 0, top, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), spin_button, 1, top, 1, 1);
    g_signal_connect(GTK_WIDGET(spin_button), "value-changed", G_CALLBACK(update_config), value);
}

/**
 * Adds a field and two spin buttosn to the passed grid.
 * @param label Field's label.
 * @param value Spin button's initial value.
 * @param top Row in which the field is placed within the grid.
 * @param grid_layout_fields Grid into which the field is added.
 */
void add_two_value_configuration(const gchar *label,
                                 const gchar *hash_key,
                                 GHashTable *layout_hash,
                                 gint top,
                                 GtkWidget *grid_layout_fields) {
    GtkWidget *label_field_name = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(label_field_name), 0.0);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), label_field_name, 0, top, 1, 1);

        GtkWidget *x_label = gtk_label_new("x:");
    GtkWidget *y_label = gtk_label_new("y:");

    Coordinates *coordinates = (Coordinates *)g_hash_table_lookup(layout_hash, hash_key);

    GtkWidget *spin_button_x = gtk_spin_button_new_with_range(0, 1000, 1);
    gtk_entry_set_alignment(GTK_ENTRY(spin_button_x), 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button_x), coordinates->x);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), x_label, 1, top, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), spin_button_x, 2, top, 1, 1);
    g_signal_connect(GTK_WIDGET(spin_button_x), "value-changed", G_CALLBACK(update_config), &(coordinates->x));

    GtkWidget *spin_button_y = gtk_spin_button_new_with_range(0, 1000, 1);
    gtk_entry_set_alignment(GTK_ENTRY(spin_button_y), 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button_y), coordinates->y);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), y_label, 3, top, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), spin_button_y, 4, top, 1, 1);
    g_signal_connect(GTK_WIDGET(spin_button_y), "value-changed", G_CALLBACK(update_config), &(coordinates->y));
}

/**
 * Creates a view for displaying and capturing changes to layout configuration.
 * @param data_passer Pointer to user data.
 * @return Pointer to the view.
*/
GtkWidget *make_configuration_view(Data_passer *data_passer) {
    GtkWidget *local_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label_layout = gtk_label_new("Layout");
    gtk_box_pack_start(GTK_BOX(local_vbox), label_layout, TRUE, TRUE, 0);

    GtkWidget *grid_layout_fields = gtk_grid_new();

    add_one_value_configuration("Right margin screen",
                                &(data_passer->right_margin_screen),
                                0,
                                grid_layout_fields);

    add_one_value_configuration("Right margin print front",
                                &(data_passer->right_margin_print_front),
                                1,
                                grid_layout_fields);

    add_one_value_configuration("Right margin print back",
                                &(data_passer->right_margin_print_back),
                                2,
                                grid_layout_fields);

    add_one_value_configuration("Font size text",
                                &(data_passer->font_size_print_dynamic),
                                3,
                                grid_layout_fields);

    add_one_value_configuration("Font size amounts",
                                &(data_passer->font_size_amount),
                                4,
                                grid_layout_fields);

    add_two_value_configuration("Name label",
                                "name_label",
                                data_passer->layout,
                                5,
                                grid_layout_fields);

    add_two_value_configuration("Name value",
                                "name_value",
                                data_passer->layout,
                                6,
                                grid_layout_fields);

    add_two_value_configuration("Account label",
                                "account_label",
                                data_passer->layout,
                                7,
                                grid_layout_fields);

    add_two_value_configuration("Account value",
                                "account_value",
                                data_passer->layout,
                                8,
                                grid_layout_fields);



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