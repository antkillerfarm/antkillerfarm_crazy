#include <gtk/gtk.h>

void on_window_destroy (GtkWidget *widget, gpointer user_data)
{
	gtk_main_quit ();
	 /* quit main loop when windows closes */ 
}


int main (int argc, char *argv[])
{
	GtkWidget	*window, *label;

	gtk_init (&argc,&argv);
	 /* initialize GTK+ */


	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	 /* create the main window */

	label = gtk_label_new ("Hello world !");
	 /* create a label with sample text */

	gtk_container_add (GTK_CONTAINER (window), label);
	 /* define the label as window child*/

	gtk_widget_show_all (window);
	 /* display window and children */


	g_signal_connect (window, "destroy", G_CALLBACK(on_window_destroy), NULL);
	 /* connect the window closing event with the "on_window_destroy" function */

	gtk_main ();
	 /* MAIN LOOP */

	return 0;
}
