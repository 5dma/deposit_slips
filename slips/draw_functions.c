#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

void draw_background(GtkWidget *widget, cairo_t *cr, gpointer data) {
    GHashTable *pointer_passer = (GHashTable *)data;
    GtkStyleContext *context;

    gint drawing_operation = *(gint *)(g_hash_table_lookup(pointer_passer, &KEY_DRAWING_ACTION));

    switch (drawing_operation) {
        case 0:  // Can we use a constant label for case?
            g_print("I AM IN CASE 0\n");

            context = gtk_widget_get_style_context(widget);

            guint width = gtk_widget_get_allocated_width(widget);
            guint height = 0.45 * width;

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

            //cairo_save(cr);

            //  g_hash_table_replace (pointer_passer, &KEY_DRAWING_ACTION, &DRAWING_OPERATION_NONE);
            break;
        case 1:

            cairo_save(cr);

            cairo_push_group(cr);
            cairo_rectangle(cr, 0, 0, 1, 1);

            cairo_clip(cr);

            g_print("I AM IN CASE 1\n");

            //    cairo_clip(cr);
            //cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
            cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr, 16);
            cairo_move_to(cr, 104, 10);
            cairo_show_text(cr, "BARF");
            //  cairo_reset_clip(cr);
            cairo_pop_group_to_source(cr);

            cairo_restore(cr);
            g_hash_table_replace(pointer_passer, &KEY_DRAWING_ACTION, &DRAWING_OPERATION_NONE);
            break;
        default:
            g_print("I AM IN DEFAULT\n");
            break;
    }
    gulong draw_handler = *(gint *)(g_hash_table_lookup(pointer_passer, &KEY_DRAW_HANDLER));
    GtkWidget *drawing_area = (GtkWidget *)(g_hash_table_lookup(pointer_passer, &KEY_DRAWING_AREA));

  //  g_signal_handler_block(G_OBJECT(drawing_area), draw_handler);
}

/* void draw_single_amount(GHashTable *pointer_passer, gchar *new_amount) {
    GtkTreeView *treeview = GTK_TREE_VIEW(g_hash_table_lookup(pointer_passer, &KEY_CHECK_TREE_VIEW));

    cairo_t *cr = (cairo_t *)g_hash_table_lookup(pointer_passer, &KEY_CAIRO_CONTEXT);


} */