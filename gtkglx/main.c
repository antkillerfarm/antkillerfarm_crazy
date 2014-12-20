#include "gtkglx.h"

gboolean init_flag = FALSE;
static void opengl_scene_init (void)
{
        GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat position[] = { 0.0, 1.0, 1.0, 0.0 };

        /* 设置光源 */
        glLightfv (GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv (GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv (GL_LIGHT0, GL_POSITION, position);
        glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
        glEnable (GL_AUTO_NORMAL);
        glEnable (GL_NORMALIZE);

        /* 启用深度测试（隐藏面摘除） */
        glEnable (GL_DEPTH_TEST);
}

static void opengl_scene_configure (void)
{
        /* 设置投影矩阵 */
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        glOrtho (-1., 1., -1., 1., -1., 20.);

        /* 设置模型视图 */
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();
        glTranslatef (0., 0., -10.);
//        gluLookAt (0., 0., 10., 0., 0., 0., 0., 1., 0.);
}

static void draw_a_sphere (unsigned int solid, double radius, int slices, int stacks)
{
        GLUquadricObj *quadObj = NULL;
        quadObj = gluNewQuadric ();

        if (solid)
                gluQuadricDrawStyle (quadObj, GLU_FILL);
        else
                gluQuadricDrawStyle (quadObj, GLU_LINE);

        gluQuadricNormals (quadObj, GLU_SMOOTH);
        gluSphere (quadObj, radius, slices, stacks);
}

static void
opengl_scene_display (void)
{
        /* 背景 */
        glClearColor (0.2, 0.4, 0.6, 1.0);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* 绘制几何体 */
        draw_a_sphere (1, 0.5f, 100, 100);
}

static void 
glwidget_show (GtkWidget *widget, gpointer userdata)
{
	gint attributes[] ={GLX_RGBA,
                            GLX_RED_SIZE, 8,
                            GLX_GREEN_SIZE, 8,
                            GLX_BLUE_SIZE, 8,
                            GLX_DEPTH_SIZE,24,
                            GLX_DOUBLEBUFFER,
                            None};
        
        gtk_glx_enable (widget, attributes);
	gtk_glx_make_current (widget);

        opengl_scene_init ();
	init_flag = TRUE;
}

static gboolean glwidget_configure (GtkWidget *widget, GdkEventConfigure *event, gpointer userdata)
{       
        GtkAllocation alc;
	if (init_flag == FALSE)
	{
		glwidget_show(widget, NULL);
	}
        gtk_widget_get_allocation (widget, &alc);
       
        gtk_glx_make_current (widget);

        glViewport (0, 0, alc.height, alc.height);
	opengl_scene_configure ();
        
        return TRUE;
}

static int
glwidget_draw (GtkWidget *widget, cairo_t *cr, gpointer userdata)
{       
        gtk_glx_make_current (widget);

        opengl_scene_display ();

        gtk_glx_swap_buffers (widget);
 
        return TRUE;
}

static void glwidget_destory (GtkWidget *widget,  gpointer userdata)
{
        gtk_glx_disable (widget);
}

int main (int argc, char **argv)
{
        GtkWidget *main_window = NULL;
	GtkWidget *window = NULL;
	GError *err = NULL;
	GtkBuilder *builder;

        gtk_init (&argc, &argv);
 
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "demo.glade", &err);
	gtk_builder_connect_signals(builder, NULL);

	main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
        gtk_window_set_title (GTK_WINDOW (main_window), "The OpenGL support of GTK+ 3.0");
	g_signal_connect (main_window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

        window = GTK_WIDGET(gtk_builder_get_object(builder, "drawingarea1"));
        g_signal_connect (window, "configure-event", G_CALLBACK (glwidget_configure), NULL);
        g_signal_connect (window, "draw", G_CALLBACK (glwidget_draw), NULL);
        g_signal_connect (window, "destroy", G_CALLBACK (glwidget_destory), NULL);

        gtk_widget_show_all(main_window);

        gtk_main ();
        
        return 0;
}
