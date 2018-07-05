#include <gtk/gtk.h>

void do_cmd_itouch(GtkButton *button, gpointer data)
{
	gtk_label_set_text(data, "IPhone 5C");
}

void do_cmd_ipod(GtkButton *button, gpointer data)
{
	GList *main_box_children_list = gtk_container_get_children (data);
	GList *main_box_child = g_list_first (main_box_children_list);
	GtkLabel *label_hello;
	GtkButton *button_cmd1;

	while (main_box_child != NULL)
	{
		if (strcmp(gtk_buildable_get_name (main_box_child->data), "label_hello") == 0)
		{
			label_hello = main_box_child->data;
		}
		if (strcmp(gtk_buildable_get_name (main_box_child->data), "button_cmd1") == 0)
		{
			button_cmd1 = main_box_child->data;
		}
		main_box_child = g_list_next (main_box_child);
	}
	gtk_label_set_text(label_hello,"Steve Jobs");
	gtk_button_set_label(button_cmd1,"Steve Wozniak");
}

void do_new_window(GtkButton *button, gpointer data)
{
	GtkWidget *window = data;
	g_signal_connect(window, "delete-event",
			  G_CALLBACK (gtk_widget_hide_on_delete), &window);
	gtk_widget_show_all(window);
}

int main(int argc, char *argv[])
{
	GtkWidget *window = NULL;
	GtkBuilder *builder;
	GAction *action;
	GError *err = NULL;

	gtk_init (NULL, NULL);

	builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, "first_demo.glade", &err);

	gtk_builder_connect_signals (builder, NULL);
	window = GTK_WIDGET (gtk_builder_get_object (builder, "main_window"));

	g_signal_connect (window, "destroy",
			  G_CALLBACK (gtk_main_quit), &window);

	//button = gtk_builder_get_object (builder, "button_cmd1");
	//g_signal_connect (button, "clicked", G_CALLBACK (do_cmd_touch), NULL);

	g_object_unref(G_OBJECT(builder));

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
