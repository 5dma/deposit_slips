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

    GtkWidget *spin_button;
    /* If displaying the font scaling spin button, allow for decimal places. Otherwise, all spin buttons are integers. */
    if (g_strcmp0 (label, "Static label scale") == 0) {
        GtkAdjustment *adjustment = gtk_adjustment_new (*value, 0.1, 1.0, 0.1, 0.1, 0.0);
        spin_button = gtk_spin_button_new (adjustment, 0.1, 1);
    } else {
        spin_button = gtk_spin_button_new_with_range(0, 1000, 1);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), *value);
    }

    gtk_entry_set_alignment(GTK_ENTRY(spin_button), 1);
    
    gtk_grid_attach(GTK_GRID(grid_layout_fields), label_field_name, 0, top, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_layout_fields), spin_button, 4, top, 1, 1);
    g_signal_connect(GTK_WIDGET(spin_button), "value-changed", G_CALLBACK(update_config), value);
}

/**
 * Adds a field and two spin buttosn to the passed grid.
 * @param label Field's label.
 * @param hash_key Key for looking up the configuration in `layout_hash`.
 * @param layout_hash Pointer to the hash table of coordinates.
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

/**
 * Callback fired after user closes the font selection dialog. The function captures the selected font family and size. (It does not capture the style or variant.)
 * @param widget Pointer to the closed font selection dialog.
 * @param data Pointer to user data.
*/
static void font_button_clicked(GtkWidget *widget, gpointer data) {
    Data_passer *data_passer = (Data_passer *)data;
    gchar font_description[100];

    /* Retrieve the selected font. */
    strcpy(font_description, gtk_font_chooser_get_font(GTK_FONT_CHOOSER(widget)));

    /* Extract the description from the selected font as well as values for the individual settings. */
    PangoFontDescription *pango_font_description = pango_font_description_from_string(font_description);
    PangoFontMask font_mask = pango_font_description_get_set_fields(pango_font_description);

    /* If the font family was selected, update the appropriate font field in data_passer. */
    if (font_mask & (PANGO_FONT_MASK_FAMILY)) {
        if (widget == data_passer->btn_font_sans_serif) {
            strcpy(data_passer->font_family_sans, pango_font_description_get_family(pango_font_description));
        } else {
            strcpy(data_passer->font_family_mono, pango_font_description_get_family(pango_font_description));
        }
    }

    /* If the font size was selected, update the appropriate font field in data_passer. */

    if (font_mask & (PANGO_FONT_MASK_SIZE)) {
        gint font_size = pango_font_description_get_size(pango_font_description);
        if (!pango_font_description_get_size_is_absolute(pango_font_description)) {
            font_size = font_size / 1024; /* Would be nice to know where 1024 comes from */
        }
        if (widget == data_passer->btn_font_sans_serif) {
            data_passer->font_size_sans_serif = font_size;
        } else {
            data_passer->font_size_monospace = font_size;
        }
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
    GtkWidget *label_layout = gtk_label_new(NULL);
    gtk_label_set_markup (GTK_LABEL (label_layout), "<b>Layout</b>");
    gtk_label_set_xalign(GTK_LABEL(label_layout), 0.0);
    gtk_box_pack_start(GTK_BOX(layout_vbox), label_layout, TRUE, TRUE, 0);

    GtkWidget *grid_layout_fields = gtk_grid_new();
     gtk_grid_set_column_spacing(GTK_GRID(grid_layout_fields), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid_layout_fields), 10);

    gint row_number = 0;

    add_one_value_configuration("Right margin screen",
                                &(data_passer->right_margin_screen),
                                row_number++,
                                grid_layout_fields);

    add_one_value_configuration("Right margin print front",
                                &(data_passer->right_margin_print_front),
                                row_number++,
                                grid_layout_fields);

    add_one_value_configuration("Right margin print back",
                                &(data_passer->right_margin_print_back),
                                row_number++,
                                grid_layout_fields);

    add_one_value_configuration("Font size text",
                                &(data_passer->font_size_sans_serif),
                                row_number++,
                                grid_layout_fields);

    add_one_value_configuration("Font size amounts",
                                &(data_passer->font_size_monospace),
                                row_number++,
                                grid_layout_fields);

    add_one_value_configuration("Static label scale",
                                &(data_passer->font_size_static_label_scaling),
                                row_number++,
                                grid_layout_fields);

    add_two_value_configuration("Name label",
                                "name_label",
                                data_passer->layout,
                                row_number++,
                                grid_layout_fields);

    add_two_value_configuration("Name value",
                                "name_value",
                                data_passer->layout,
                                row_number++,
                                grid_layout_fields);

    add_two_value_configuration("Account label",
                                "account_label",
                                data_passer->layout,
                                row_number++,
                                grid_layout_fields);

    add_two_value_configuration("Account value",
                                "account_value",
                                data_passer->layout,
                                row_number++,
                                grid_layout_fields);

    GtkWidget *scrolled_window_layout = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window_layout), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window_layout), 190);

    gtk_container_add(GTK_CONTAINER(scrolled_window_layout), grid_layout_fields);
    gtk_box_pack_start(GTK_BOX(layout_vbox), scrolled_window_layout, TRUE, TRUE, 0);

    /* Set up the view for font selection */
    GtkWidget *font_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label_font = gtk_label_new(NULL);
    gtk_label_set_markup (GTK_LABEL (label_font), "<b>Fonts</b>");
    gtk_label_set_xalign(GTK_LABEL(label_font), 0.0);
    gtk_box_pack_start(GTK_BOX(font_vbox), label_font, TRUE, TRUE, 0);

    GtkWidget *grid_layout_fonts = gtk_grid_new();
         gtk_grid_set_column_spacing(GTK_GRID(grid_layout_fonts), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid_layout_fonts), 10);

    /* Controls for sans-serif font selection. */

    GtkWidget *label_sans_serif = gtk_label_new("Sans serif");
    gtk_label_set_xalign(GTK_LABEL(label_sans_serif), 0.0);

    gchar sans_serif_button_label[100];
    g_snprintf(sans_serif_button_label, sizeof(sans_serif_button_label), "%s %f", data_passer->font_family_sans, data_passer->font_size_sans_serif);
    GtkWidget *btn_font_sans_serif = gtk_font_button_new_with_font(sans_serif_button_label);
    /* Save the button's memory address. */
    data_passer->btn_font_sans_serif = btn_font_sans_serif;
    gtk_font_button_set_title(GTK_FONT_BUTTON(btn_font_sans_serif), "Font");
    gtk_font_button_set_show_size(GTK_FONT_BUTTON(btn_font_sans_serif), TRUE);
    g_signal_connect(btn_font_sans_serif, "font-set", G_CALLBACK(font_button_clicked), data_passer);

    /* Controls for monospace font selection. */

    GtkWidget *label_monospace = gtk_label_new("Monospace");
    gtk_label_set_xalign(GTK_LABEL(label_monospace), 0.0);

    gchar monospace_button_label[100];
    g_snprintf(monospace_button_label, sizeof(monospace_button_label), "%s %f", data_passer->font_family_mono, data_passer->font_size_monospace);
    GtkWidget *btn_font_monospace = gtk_font_button_new_with_font(monospace_button_label);
    /* Save the button's memory address. */
    data_passer->btn_font_monospace = btn_font_monospace;

    gtk_font_button_set_title(GTK_FONT_BUTTON(btn_font_monospace), "Font");
    gtk_font_button_set_show_size(GTK_FONT_BUTTON(btn_font_monospace), TRUE);
    g_signal_connect(btn_font_monospace, "font-set", G_CALLBACK(font_button_clicked), data_passer);

    gtk_grid_attach(GTK_GRID(grid_layout_fonts), label_sans_serif, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_layout_fonts), btn_font_sans_serif, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid_layout_fonts), label_monospace, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid_layout_fonts), btn_font_monospace, 1, 1, 1, 1);

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
