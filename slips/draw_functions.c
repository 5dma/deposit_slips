#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

gboolean print_deposit_amounts(GtkTreeModel *model,
                               GtkTreePath *path,
                               GtkTreeIter *iter,
                               gpointer data) {
    cairo_t *cr = (cairo_t *)data;
    gchar *amount;
    gtk_tree_model_get(model, iter, CHECK_AMOUNT, &amount, -1);
    gchar *pathstring = gtk_tree_path_to_string(path);
    g_print("The amount is %s and the string is %s\n", amount, pathstring);

   // gboolean not_at_end = gtk_tree_model_iter_next(model, iter);

    gboolean not_at_end = gtk_tree_model_get_iter (model, iter, path);

    if (not_at_end) {
       // g_print("There are more\n");
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 16);
        cairo_move_to(cr, 104, 40);
        cairo_show_text(cr, "BARF");
        return FALSE;
    } else {
     //   g_print("There are NO more\n");
        return TRUE;
    }
}

void draw_background(GtkWidget *widget, cairo_t *cr, gpointer data) {
    
    GHashTable *pointer_passer = (GHashTable *)data;

    GtkListStore *checks_store = (GtkListStore *)g_hash_table_lookup(pointer_passer, &KEY_CHECKS_STORE);

    GtkTreeIter iter;

    GtkStyleContext *context;

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

    gboolean checks_exist = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(checks_store), &iter);
    g_print("The value of checks_exist is %d\n", checks_exist);

    if (checks_exist) {
        gtk_tree_model_foreach(GTK_TREE_MODEL(checks_store), print_deposit_amounts, cr);
    }
}