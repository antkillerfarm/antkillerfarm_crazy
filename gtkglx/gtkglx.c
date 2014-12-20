#include <GL/glu.h>
#include <GL/glx.h>
#include <gdk/gdkx.h>

#include "gtkglx.h"

static GSList *gtk_glx_context_list    = NULL;

typedef struct _GtkGLContext GtkGLContext;
struct _GtkGLContext {
        GtkWidget  *widget;
        GLXContext context;
};

static gint
find_context_according_widget (gconstpointer gtkglcontext, gconstpointer gtkwidget)
{
        const GtkGLContext *gtkgc = gtkglcontext;
        const GtkWidget *widget = gtkwidget;

        if (gtkgc->widget == widget)
                return 0;
        else
                return -1;
}

static GSList *
gtk_glx_get_context (GtkWidget *widget)
{
        g_return_val_if_fail (gtk_glx_context_list != NULL, NULL);
        
        GSList *node = g_slist_find_custom (gtk_glx_context_list,
                                            widget,
                                            find_context_according_widget);
        return node;
}

void
gtk_glx_enable (GtkWidget *widget, gint *attributes)
{
        GdkScreen *screen;
        GdkVisual *visual;
        Display *xdisplay;
        XVisualInfo *xvi;
        gint xscreen;
        
        GtkGLContext *gtkgc;
        
        gtk_widget_set_app_paintable (widget, TRUE);
        gtk_widget_set_double_buffered (widget, FALSE);

        gtkgc = g_slice_new0 (GtkGLContext);
        
        xdisplay = gdk_x11_get_default_xdisplay ();
        xscreen = gdk_x11_get_default_screen ();
        
        g_return_if_fail (NULL != (xvi = glXChooseVisual (xdisplay, xscreen, attributes)));
        screen = gtk_widget_get_screen (widget);
        g_return_if_fail (NULL != (visual = gdk_x11_screen_lookup_visual (screen, xvi->visualid)));
        gtk_widget_set_visual (widget, visual);
        
        gtkgc->widget  = widget;
        g_return_if_fail (NULL != (gtkgc->context = glXCreateContext (xdisplay, xvi, NULL, GL_TRUE)));
        
        gtk_glx_context_list = g_slist_prepend (gtk_glx_context_list, gtkgc);
}

void
gtk_glx_make_current (GtkWidget *widget)
{
        Display *display = gdk_x11_get_default_xdisplay ();
        Window window = gdk_x11_window_get_xid (gtk_widget_get_window (widget));
        GSList *node = gtk_glx_get_context (widget);
        GtkGLContext *gtkgc = node->data;
        g_return_if_fail (True == glXMakeCurrent (display, window, gtkgc->context));
}


void
gtk_glx_swap_buffers (GtkWidget *widget)
{
        Display *display = gdk_x11_get_default_xdisplay ();
        Window window = gdk_x11_window_get_xid (gtk_widget_get_window (widget));
        glXSwapBuffers (display, window);
}

void
gtk_glx_disable (GtkWidget *widget)
{
        Display *display = gdk_x11_get_default_xdisplay ();
        g_return_if_fail (True == glXMakeCurrent (display, None, NULL));
        
        GSList *node = gtk_glx_get_context (widget);
        GtkGLContext *gtkgc = node->data;
        glXDestroyContext (display, gtkgc->context);

        gtk_glx_context_list = g_slist_remove (gtk_glx_context_list, gtkgc);
        g_slice_free (GtkGLContext, gtkgc);
}
