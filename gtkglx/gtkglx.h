#ifndef GTK_GLX_H
#define GTK_GLX_H

#include <gtk/gtk.h>
#include <GL/glu.h>
#include <GL/glx.h>

void gtk_glx_enable (GtkWidget *widget, gint *attributes);
void gtk_glx_disable (GtkWidget *widget);

void gtk_glx_make_current (GtkWidget *widget);
void gtk_glx_swap_buffers (GtkWidget *widget);

#endif /* GTK_GLX_H */
