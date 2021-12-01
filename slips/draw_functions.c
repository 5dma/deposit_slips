#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

void draw_background(GtkWidget *widget, cairo_t *cr, gpointer data) {

    GHashTable *pointer_passer = (GHashTable *)data;
    g_hash_table_insert(pointer_passer, &KEY_CAIRO_CONTEXT, cr);

    cr = cairo_reference (cr);

    guint width, height;
    GdkRGBA color;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context(widget);

    width = gtk_widget_get_allocated_width(widget);
    height = 0.45 * width;

    gtk_render_background(context, cr, 0, 0, width, height);

    cairo_rectangle(cr, 0.0, 0.0, width, height);
    cairo_set_line_width(cr, 5.0);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_text_extents_t te;
    cairo_text_extents(cr, "a", &te);

    /* Write Deposit Ticket */
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 6);
    cairo_move_to(cr, 104, 10);
    cairo_show_text(cr, "DEPOSIT TICKET");

    /* Write Name */
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 6);
    cairo_move_to(cr, 27, 25);
    cairo_show_text(cr, "Name");

    /* Write Account */
    cairo_set_font_size(cr, 6);
    cairo_move_to(cr, 27, 52);
    cairo_show_text(cr, "Account");

    /* Write Date */
    cairo_set_font_size(cr, 5);
    cairo_move_to(cr, 27, 83);
    cairo_show_text(cr, "Date");
}

void draw_single_amount(GHashTable *pointer_passer, gchar *new_amount) {

    GtkTreeView *treeview = GTK_TREE_VIEW(g_hash_table_lookup(pointer_passer, &KEY_CHECK_TREE_VIEW));

    cairo_t *cr = (cairo_t *)g_hash_table_lookup(pointer_passer, &KEY_CAIRO_CONTEXT);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 16);
    cairo_move_to(cr, 104, 10);
    cairo_show_text(cr, new_amount);
}