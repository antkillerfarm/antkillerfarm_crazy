#include <windows.h> 
#include <GL/gl.h>
#include "gtkgl.h"
#include <gdk/gdkwin32.h>

static GSList *gtk_wgl_context_list    = NULL;

typedef struct _GLW32Context GLW32Context;
struct _GLW32Context {
	HWND hwnd;
	HDC hdc;
	HGLRC hglrc;
	};

typedef struct _GtkGLContext GtkGLContext;
struct _GtkGLContext {
	GtkWidget  *widget;
	GLW32Context context;
	};

static gint find_context_according_widget (gconstpointer gtkglcontext, gconstpointer gtkwidget)
{
	const GtkGLContext *gtkgc = gtkglcontext;
	const GtkWidget *widget = gtkwidget;

	if (gtkgc->widget == widget)
		return 0;
	else
		return -1;
}

static GSList* gtk_wgl_get_context (GtkWidget *widget)
{
	g_return_val_if_fail (gtk_wgl_context_list != NULL, NULL);

	GSList *node = g_slist_find_custom (gtk_wgl_context_list,
										widget,
										find_context_according_widget);
	return node;
}

BOOL bSetupPixelFormat(HDC hdc)
{ 
	PIXELFORMATDESCRIPTOR *ppfd;
	PIXELFORMATDESCRIPTOR pfd = { 
	sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
	1,                     // version number  
	PFD_DRAW_TO_WINDOW |   // support window  
	PFD_SUPPORT_OPENGL |   // support OpenGL  
	PFD_DOUBLEBUFFER,      // double buffered  
	PFD_TYPE_RGBA,         // RGBA type  
	24,                    // 24-bit color depth  
	0, 0, 0, 0, 0, 0,      // color bits ignored  
	8,                     // no alpha buffer  
	0,                     // shift bit ignored  
	0,                     // no accumulation buffer  
	0, 0, 0, 0,            // accum bits ignored  
	32,                    // 32-bit z-buffer      
	0,                     // no stencil buffer  
	0,                     // no auxiliary buffer  
	PFD_MAIN_PLANE,        // main layer  
	0,                     // reserved  
	0, 0, 0                // layer masks ignored  
	};
	int pixelformat; 

	ppfd = &pfd; 

	pixelformat = ChoosePixelFormat(hdc, ppfd); 

	if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
	{
		return FALSE; 
	}

	if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
	{
		return FALSE; 
	}

	return TRUE; 
}

void gtk_gl_enable (GtkWidget *widget)
{
	HWND hwnd;
	HDC hdc;
	HGLRC hglrc;
	GtkGLContext *gtkgc;

	gtk_widget_set_app_paintable (widget, TRUE);
	gtk_widget_set_double_buffered (widget, FALSE);

	gtkgc = g_slice_new0 (GtkGLContext);

	hwnd = GDK_WINDOW_HWND(gtk_widget_get_window(widget));
	hdc = GetDC (hwnd);
	g_return_if_fail (bSetupPixelFormat(hdc));
	hglrc = wglCreateContext(hdc); 

	gtkgc->widget  = widget;
	gtkgc->context.hwnd = hwnd;
	gtkgc->context.hdc = hdc;
	gtkgc->context.hglrc = hglrc;

	gtk_wgl_context_list = g_slist_prepend (gtk_wgl_context_list, gtkgc);
}

void
gtk_gl_make_current (GtkWidget *widget)
{
	GSList *node = gtk_wgl_get_context (widget);
	GtkGLContext *gtkgc = node->data;
	wglMakeCurrent(gtkgc->context.hdc, gtkgc->context.hglrc);
}


void
gtk_gl_swap_buffers (GtkWidget *widget)
{
	GSList *node = gtk_wgl_get_context (widget);
	GtkGLContext *gtkgc = node->data;
	SwapBuffers(gtkgc->context.hdc);
}

void
gtk_gl_disable (GtkWidget *widget)
{
	GSList *node = gtk_wgl_get_context (widget);
	GtkGLContext *gtkgc = node->data;

	if (gtkgc->context.hglrc) 
		wglDeleteContext(gtkgc->context.hglrc); 

	gtk_wgl_context_list = g_slist_remove (gtk_wgl_context_list, gtkgc);
	g_slice_free (GtkGLContext, gtkgc);
}
