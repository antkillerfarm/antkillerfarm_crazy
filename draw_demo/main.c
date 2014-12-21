#include <cairo.h>
#include <gtk/gtk.h>

#define MOUSE_LBUTTON 1
#define MOUSE_RBUTTON 3

gint animation_time = 0;

static void do_drawing(cairo_t *cr, gint _t)
{
	cairo_set_source_rgb(cr, 0, 0.5, 0);
	cairo_set_line_width(cr, 1);
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 200, 150);
	cairo_stroke(cr);    
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{
	if (flag)
	{
		do_drawing1(cr);
	}
	else
	{
		do_drawing2(cr);
	}
	return FALSE;
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
	if (event->button == MOUSE_LBUTTON)
	{
		gtk_widget_queue_draw(widget);
	}

	if (event->button == MOUSE_RBUTTON)
	{
		gtk_widget_queue_draw(widget);
	}
	
	return TRUE;
}


int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *darea;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), darea);
 
	gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

	g_signal_connect(G_OBJECT(darea), "draw", 
			 G_CALLBACK(on_draw_event), NULL); 
	g_signal_connect(window, "destroy",
			 G_CALLBACK(gtk_main_quit), NULL);  
    
	g_signal_connect(window, "button-press-event", 
			 G_CALLBACK(clicked), NULL);
 
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 300); 
	gtk_window_set_title(GTK_WINDOW(window), "Lines");

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}
