#include <gtk/gtk.h>

#include "../constants.h"
#include "../headers.h"

void draw_background(GHashTable *pointer_passer) {
    g_print("Starting the backgrouund\n");

    cairo_t *cr = (cairo_t *)g_hash_table_lookup(pointer_passer, &KEY_CAIRO_CONTEXT);
    GtkWidget *drawing_area = (GtkWidget *)g_hash_table_lookup(pointer_passer, &KEY_DRAWING_AREA);

    GtkTreeIter iter;

    //  GtkStyleContext *context;

    //  context = gtk_widget_get_style_context(widget);

    guint width = gtk_widget_get_allocated_width(drawing_area);
    guint height = 0.45 * width;

    //  gtk_render_background(context, cr, 0, 0, width, height);

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

    /* Draw individual deposit lines */

    GtkTreeView *treeview = GTK_TREE_VIEW(g_hash_table_lookup(pointer_passer, &KEY_CHECK_TREE_VIEW));
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    //draw_background(pointer_passer);
    gboolean checks_exist = gtk_tree_model_get_iter_first(model, &iter);
    g_print("The value of checks_exist is %d\n", checks_exist);

    if (checks_exist) {
        gtk_tree_model_foreach(model, print_deposit_amounts, cr);
    }
}

gboolean print_deposit_amounts(GtkTreeModel *model,
                               GtkTreePath *path,
                               GtkTreeIter *iter,
                               gpointer data) {
    cairo_t *cr = (cairo_t *)data;
    gchar *amount;
    gtk_tree_model_get(model, iter, CHECK_AMOUNT, &amount, -1);
    gchar *pathstring = gtk_tree_path_to_string(path);
    g_print("The amount is %s and the string is %s\n", amount, pathstring);

    gboolean not_at_end = gtk_tree_model_get_iter(model, iter, path);

    guint64 row_number;
    GError *gerror = NULL;

    if (not_at_end) {
        gboolean string_to_int = g_ascii_string_to_unsigned(
            pathstring,  /* path of the current row */
            10,          /* Base 10 */
            0,           /* minimum value */
            100,         /* maximum value */
            &row_number, /* returned row number */
            &gerror);    /* pointer for GError *. */

        cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
        cairo_select_font_face(cr, "DejaVuSans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 16);
        cairo_move_to(cr, 50, 50);
        cairo_show_text(cr, "BARF");
        return FALSE;
    } else {
        //   g_print("There are NO more\n");
        return TRUE;
    }
}