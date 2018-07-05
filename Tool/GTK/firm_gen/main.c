#include <gtk/gtk.h>
#include <libxml/xmlreader.h>

enum{
NAME_COLUMN = 0,
TOGGLE_VISIBLE_COLUMN,
TOGGLE_TOGGLED_COLUMN,
NBR_COLUMNS
};

G_MODULE_EXPORT void do_item_toggled(GtkCellRendererToggle *cell_renderer,gchar *path,gpointer user_data)  
{
	GtkTreeStore *model = (GtkTreeStore*)user_data;
	GtkTreeIter iter;
	GValue value = {0};
	gboolean active_status;
        if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(model), &iter, path))
	{
		gtk_tree_model_get_value(GTK_TREE_MODEL(model), &iter, TOGGLE_TOGGLED_COLUMN, &value);
		active_status = g_value_get_boolean(&value);
		g_value_set_boolean(&value, !active_status);
		gtk_tree_store_set_value(model, &iter, TOGGLE_TOGGLED_COLUMN, &value);
		g_print("Hi Jobs:%d:%s\r\n", active_status, path);
		g_value_unset(&value);
	}
}

int main(int argc, char *argv[])
{
	GtkWidget *window = NULL;
	GtkBuilder *builder;
	GAction *action;
	GError *err = NULL;
	GtkTreeStore *firm_items_tree_store = NULL;
	GtkTreeIter iter;
	GtkTreeIter child;
	xmlTextReaderPtr xr;

	gtk_init (NULL, NULL);

	builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, "firm_gen.glade", &err);

	gtk_builder_connect_signals (builder, NULL);
	window = GTK_WIDGET (gtk_builder_get_object (builder, "main_window"));

	g_signal_connect (window, "destroy",
			  G_CALLBACK (gtk_main_quit), &window);

	xr = xmlNewTextReaderFilename("firm_items.xml");
	/*while (xmlTextReaderRead(xr) == 1)
	{
		g_print("Apple:%s\r\n", xmlTextReaderConstName(xr));
		}*/
	g_print("Apple:%s\r\n", xmlTextReaderConstString(xr, "sub_item"));
	xmlFreeTextReader(xr);
	firm_items_tree_store = GTK_TREE_STORE (gtk_builder_get_object (builder, "firm_items_tree_store"));
	gtk_tree_store_append (firm_items_tree_store, &iter, NULL);
	gtk_tree_store_set (firm_items_tree_store, &iter, 
			    NAME_COLUMN, "Pin Reused",
			    TOGGLE_VISIBLE_COLUMN, FALSE,
			    TOGGLE_TOGGLED_COLUMN, FALSE, -1);

	gtk_tree_store_append (firm_items_tree_store, &child, &iter);
	gtk_tree_store_set (firm_items_tree_store, &child,
			    NAME_COLUMN, "GPIO0",
			    TOGGLE_VISIBLE_COLUMN, TRUE,
			    TOGGLE_TOGGLED_COLUMN, TRUE, -1);

	gtk_tree_store_append (firm_items_tree_store, &child, &iter);
	gtk_tree_store_set (firm_items_tree_store, &child,
			    NAME_COLUMN, "GPIO1",
			    TOGGLE_VISIBLE_COLUMN, TRUE,
			    TOGGLE_TOGGLED_COLUMN, TRUE, -1);
	g_object_unref(G_OBJECT(builder));

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
