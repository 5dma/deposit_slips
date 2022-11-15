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
 * Adds a field for selecting fonts.
 * @param label Field's label.
 * @param value Spin button's initial value.
 * @param top Row in which the field is placed within the grid.
 * @param grid_layout_fields Grid into which the field is added.
 */
void add_font_configuration(const gchar *label,
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

static void font_button_clicked(GtkWidget *widget, gpointer user_data) {
    Data_passer *data_passer = (Data_passer *)user_data;
    gchar font_description[100];
    
    strcpy(font_description, gtk_font_chooser_get_font(GTK_FONT_CHOOSER(widget)));

    PangoFontDescription *pango_font_description = pango_font_description_from_string(font_description);
    PangoFontMask font_mask = pango_font_description_get_set_fields(pango_font_description);

    if (font_mask & (PANGO_FONT_MASK_FAMILY)) {
        strcpy(data_passer->font_family_sans, pango_font_description_get_family(pango_font_description));
    }
    if (font_mask & (PANGO_FONT_MASK_STYLE)) {
        g_print("Font mask style was set\n");
       PangoStyle pango_style = pango_font_description_get_style(pango_font_description);
    }

    if (font_mask & (PANGO_FONT_MASK_SIZE)) {
        gint font_size = pango_font_description_get_size(pango_font_description);
        if (!pango_font_description_get_size_is_absolute(pango_font_description)) {
            font_size = font_size / 1024; /* Would be nice to know where 1024 comes from */
        }
        data_passer->font_size_print_dynamic = font_size;
    }
}

/**
 * Creates a view for displaying and capturing changes to layout configuration.
 * @param data_passer Pointer to user data.
 * @return Pointer to the view.
 */
GtkWidget *make_configuration_view(Data_passer *data_passer) {
    /* Set up the view for layout selection */
    GtkWidget *layout_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label_layout = gtk_label_new("Layout");
    gtk_label_set_xalign(GTK_LABEL(label_layout), 0.0);
    gtk_box_pack_start(GTK_BOX(layout_vbox), label_layout, TRUE, TRUE, 0);

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

    GtkWidget *scrolled_window_layout = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window_layout), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window_layout), 190);

    gtk_container_add(GTK_CONTAINER(scrolled_window_layout), grid_layout_fields);
    gtk_box_pack_start(GTK_BOX(layout_vbox), scrolled_window_layout, TRUE, TRUE, 0);

    /* Set up the view for font selection */
    GtkWidget *font_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label_font = gtk_label_new("Fonts");
    gtk_label_set_xalign(GTK_LABEL(label_font), 0.0);
    gtk_box_pack_start(GTK_BOX(font_vbox), label_font, TRUE, TRUE, 0);

    GtkWidget *grid_layout_fonts = gtk_grid_new();

    GtkWidget *label_field_name = gtk_label_new("Sans serif");
    gtk_label_set_xalign(GTK_LABEL(label_field_name), 0.0);

    gchar font_button_label[100];
    g_snprintf(font_button_label, sizeof(font_button_label), "%s %f", data_passer->font_family_sans, data_passer->font_size_print_dynamic);
    GtkWidget *btn_font_sans_serif = gtk_font_button_new_with_font(font_button_label);
    gtk_font_button_set_title(GTK_FONT_BUTTON(btn_font_sans_serif), "Font");
    gtk_font_button_set_show_size(GTK_FONT_BUTTON(btn_font_sans_serif), TRUE);
    // g_signal_connect(btn_font_sans_serif, "font-set", G_CALLBACK(set_font_callback), data_passer);
    g_signal_connect(btn_font_sans_serif, "font-set", G_CALLBACK(font_button_clicked), data_passer);

    gtk_grid_attach(GTK_GRID(grid_layout_fonts), label_field_name, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_layout_fonts), btn_font_sans_serif, 1, 0, 1, 1);

    GtkWidget *scrolled_window_fonts = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window_fonts), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window_fonts), 190);

    gtk_container_add(GTK_CONTAINER(scrolled_window_fonts), grid_layout_fonts);
    gtk_box_pack_start(GTK_BOX(font_vbox), scrolled_window_fonts, TRUE, TRUE, 0);

    GtkWidget *hbox_configuration = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    gtk_box_pack_start(GTK_BOX(hbox_configuration), layout_vbox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_configuration), font_vbox, TRUE, TRUE, 0);

    return hbox_configuration;
}
