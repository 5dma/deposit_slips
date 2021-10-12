#include <gtk/gtk.h>
#include <stdlib.h>
#include "headers.h"


int main(int argc, char *argv[]) {

    printf("Here");
    GtkApplication *app = gtk_application_new(
        "net.lautman.DepositSlip",
        G_APPLICATION_FLAGS_NONE);

    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}