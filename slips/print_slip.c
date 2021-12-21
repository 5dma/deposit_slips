#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

void draw_page(GtkPrintOperation *self, GtkPrintContext *context, gint page_nr, gpointer user_data) {
    g_print("Inside draw_page\n");
    cairo_t *cr;
    PangoLayout *layout;
    gdouble width, text_height;
    gint layout_height;
    PangoFontDescription *desc;

    cr = gtk_print_context_get_cairo_context(context);
    width = gtk_print_context_get_width(context);

    cairo_rectangle(cr, 0, 0, width, 200);

    cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
    cairo_fill(cr);

    layout = gtk_print_context_create_pango_layout(context);

    desc = pango_font_description_from_string("sans 14");
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    pango_layout_set_text(layout, "some text", -1);
    pango_layout_set_width(layout, width * PANGO_SCALE);
    pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);

    pango_layout_get_size(layout, NULL, &layout_height);
    text_height = (gdouble)layout_height / PANGO_SCALE;

    cairo_move_to(cr, width / 2, (200 - text_height) / 2);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
}

void begin_print ( GtkPrintOperation* self, GtkPrintContext* context, gpointer user_data ) {
    g_print("Beginning the pring!\n");
}

void end_print ( GtkPrintOperation* self, GtkPrintContext* context, gpointer user_data ) {
        g_print("Ending the pring!\n");
}

void print_deposit_slip(GtkButton *self, gpointer data) {
   //  system("lpr -P Brother-HL-L2350DW-series -o InputSlot=Manual /home/abba/Desktop/amazon_buy.txt");
    GHashTable *pointer_passer = (GHashTable *)data;
    GtkWidget *application_window = GTK_WIDGET(g_hash_table_lookup(pointer_passer, &KEY_APPLICATION_WINDOW));
    g_print("The button is clicked\n");
    GtkPrintOperation *operation;
    GError *error;
    gint res;

    operation = gtk_print_operation_new();

    gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
    gtk_print_operation_set_use_full_page(operation, TRUE);
    gtk_print_operation_set_allow_async(operation, TRUE);
     gtk_print_operation_set_n_pages(operation, 1);

    g_signal_connect(G_OBJECT(operation), "begin_print", G_CALLBACK(begin_print), NULL);
    g_signal_connect(G_OBJECT(operation), "draw_page", G_CALLBACK(draw_page), NULL);
    g_signal_connect(G_OBJECT(operation), "end_print", G_CALLBACK(end_print), NULL);
    res = gtk_print_operation_run(operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW(application_window), &error);
    switch (res) {
        case GTK_PRINT_OPERATION_RESULT_ERROR:
            g_print("Error\n");
        break;
        case GTK_PRINT_OPERATION_RESULT_APPLY:
            g_print("Apply\n");
        break;
        case GTK_PRINT_OPERATION_RESULT_CANCEL:
            g_print("Cancel\n");
        break;
        case GTK_PRINT_OPERATION_RESULT_IN_PROGRESS:
            g_print("In progress\n");
        break;
        default:
            g_print("Default\n");
    }
    g_object_unref(operation);
    g_print("All done\n");
}