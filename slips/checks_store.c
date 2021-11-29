#include <gtk/gtk.h>
#include "../headers.h"



GtkListStore *make_checks_store() {

    GtkListStore *local_store = gtk_list_store_new(SLIP_COLUMNS,G_TYPE_STRING,G_TYPE_BOOLEAN);

    return local_store;

}