#ifndef GTK_GL_H
#define GTK_GL_H

#include <gtk/gtk.h>
#include <gdk/gdkwin32.h>
#include <GL/glu.h>

void gtk_gl_enable (GtkWidget *widget);
void gtk_gl_disable (GtkWidget *widget);

void gtk_gl_make_current (GtkWidget *widget);
void gtk_gl_swap_buffers (GtkWidget *widget);

#endif /* GTK_GL_H */
