#include <gtk/gtk.h>
#include <stdlib.h>
#include "headers.h"

/*! \mainpage Main Page
 * This small GTK 3.0 application prints deposit slips.
 *
 * To generate this 
 * code documentation, run doxygen Doxyfile. The output is in the `html/` 
 * directory of this source code.
 */


int main(int argc, char *argv[]) {

    GtkApplication *app = gtk_application_new(
        "net.lautman.DepositSlips",
        G_APPLICATION_FLAGS_NONE);

    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    /* Decrease reference count because assigning it in on_app_activate */
    g_object_unref(app); 
    return status;
}