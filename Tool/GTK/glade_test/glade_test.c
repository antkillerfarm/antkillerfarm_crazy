#include <gtk/gtk.h>

G_MODULE_EXPORT void do_cmd_itouch(GtkButton *button, gpointer data)
{
  g_print("IPhone 5C!");
}

G_MODULE_EXPORT void do_cmd_ipod(GtkButton *button, gpointer data)
{
  g_print("IPhone 5S!");
}

void on_window_destroy (GtkWidget *widget, gpointer user_data)
{
  gtk_main_quit ();
  /* quit main loop when windows closes */ 
}

int main (int argc, char *argv[])
{
  GtkWidget *window, *label;
  GtkBuilder *builder;

  gtk_init (&argc,&argv);
  /* initialize GTK+ */

  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "./glade_test.glade", NULL);
  gtk_builder_connect_signals (builder, NULL);
  window = GTK_WIDGET (gtk_builder_get_object (builder, "main_window"));

  g_signal_connect (window, "destroy", G_CALLBACK(on_window_destroy), NULL);
  /* connect the window closing event with the "on_window_destroy" function */

  gtk_widget_show_all (window);
  gtk_main ();
  /* MAIN LOOP */

  return 0;
}
