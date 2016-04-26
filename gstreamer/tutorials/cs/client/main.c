#include <gtk/gtk.h>
#include <gst/gst.h>
#include <string.h>

#define MAX_PATH 256
#define MEDIA_PORT 1500
#define CONTROL_PORT 1501

typedef struct{
  GtkStatusbar *statusbar;
  gint contextId;
}MainWindowSubWidget;

typedef struct{
  GstElement *playbin;
  GstElement *source;
  GstElement *tcp_sink;
}GstData;

typedef struct{
  GSocketConnection *connection;
  GSocketClient *client;
}ControlServiceData;

char g_filename[MAX_PATH];

MainWindowSubWidget main_window_sub_widget;
GstData gst_data;
ControlServiceData control_service_data;

static gboolean bus_call (GstBus * bus, GstMessage * msg, gpointer data)
{
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:{
      gst_element_set_state (gst_data.playbin, GST_STATE_NULL);
      g_print ("End-of-stream\n");
      break;
    }
    case GST_MESSAGE_ERROR:{
      gchar *debug;
      GError *err;
      gst_element_set_state (gst_data.playbin, GST_STATE_NULL);
      gst_message_parse_error (msg, &err, &debug);
      g_printerr ("Debugging info: %s\n", (debug) ? debug : "none");
      g_free (debug);

      g_print ("Error: %s\n", err->message);
      g_error_free (err);

      break;
    }
    default:
      break;
  }
  return TRUE;
}

G_MODULE_EXPORT void do_button_open_clicked(GtkButton *button, gpointer data)
{
  GtkWidget *dialog;
  GtkWindow *window = (GtkWindow *)data;

  dialog = gtk_file_chooser_dialog_new ("Open File",
					window,
					GTK_FILE_CHOOSER_ACTION_OPEN,
				        "Cancel", GTK_RESPONSE_CANCEL,
				        "Open", GTK_RESPONSE_ACCEPT,
					NULL);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gchar *filename;

      filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
      g_strlcpy(g_filename, filename, MAX_PATH);
      gtk_statusbar_push(GTK_STATUSBAR(main_window_sub_widget.statusbar), 
		     main_window_sub_widget.contextId, g_filename);
      g_free (filename);
    }

  gtk_widget_destroy (dialog);

}

G_MODULE_EXPORT void do_button_play_clicked(GtkButton *button, gpointer data)
{
  gchar *uri;
  /*if (gst_uri_is_valid (g_filename))
    uri = g_strdup (g_filename);
  else
  uri = gst_filename_to_uri (g_filename, NULL);*/
  uri = g_strdup (g_filename);
  //g_object_set (gst_data.playbin, "uri", uri, NULL);
  g_object_set (gst_data.source, "location", uri, NULL);
  g_object_set (gst_data.tcp_sink, "port", MEDIA_PORT, NULL);
  g_free (uri);

  gst_element_set_state (gst_data.playbin, GST_STATE_PLAYING);
}

G_MODULE_EXPORT void do_button_next_clicked(GtkButton *button, gpointer data)
{
  GError * error = NULL;
  gchar *str = "Hello server!";

  /* use the connection */
  //GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (control_service_data.connection));
  GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (control_service_data.connection));
  g_output_stream_write  (ostream,
                          str, /* your message goes here */
                          strlen(str) + 1, /* length of your message */
                          NULL,
                          &error);
  g_print ("%s\n", __func__);
  /* don't forget to check for errors */
  if (error != NULL)
  {
    g_print ("%s", error->message);
  }
  
}

void ui_init()
{
  GtkWidget *window = NULL;
  GtkBuilder *builder;
  GError *err = NULL;
  gchar *exec_name = NULL;
  gchar *tmp = NULL;
  gchar exe_path[MAX_PATH];

  gtk_init(NULL, NULL);

  builder = gtk_builder_new();
  exec_name = g_file_read_link("/proc/self/exe", NULL);
  tmp = g_strrstr(exec_name, "/");
  *tmp = 0;
  g_strlcpy(exe_path, exec_name, MAX_PATH);
  g_strlcat(exe_path, "/audio_player.glade", MAX_PATH);
  g_free(exec_name);

  gtk_builder_add_from_file(builder, exe_path, &err);

  gtk_builder_connect_signals(builder, NULL);
  window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

  main_window_sub_widget.statusbar = GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar1"));

  main_window_sub_widget.contextId = gtk_statusbar_get_context_id(
                   GTK_STATUSBAR(main_window_sub_widget.statusbar), "Editor Messages");

  g_signal_connect(window, "destroy",
		   G_CALLBACK (gtk_main_quit), &window);

  g_object_unref(G_OBJECT(builder));

  gtk_widget_show_all(window);
}

void media_init()
{
  GstBus *bus;

  gst_init (NULL, NULL);

  gst_data.playbin = gst_pipeline_new("audio_player_client");
  gst_data.source = gst_element_factory_make ("filesrc", "source");
  gst_data.tcp_sink = gst_element_factory_make ("tcpclientsink", "tcp_sink");

  if (!gst_data.playbin || !gst_data.source || !gst_data.tcp_sink)
    {
      g_print ("Not all elements could be created.\n");
    }

  gst_bin_add_many (GST_BIN (gst_data.playbin), gst_data.source, gst_data.tcp_sink, NULL);

  if (gst_element_link_many (gst_data.source, gst_data.tcp_sink, NULL) != TRUE)
    {
      g_print ("Elements could not be linked.\n");
      gst_object_unref (gst_data.playbin);
    }

  bus = gst_element_get_bus (gst_data.playbin);
  gst_bus_add_watch (bus, bus_call, NULL);
  g_object_unref (bus);
}

void media_cleanup()
{
  gst_element_set_state (gst_data.playbin, GST_STATE_NULL);
  g_object_unref (gst_data.playbin);
}


void cortrol_service_init()
{
  GError * error = NULL;
  
  /* create a new connection */
  control_service_data.client = g_socket_client_new();

    /* connect to the host */
  control_service_data.connection = g_socket_client_connect_to_host (control_service_data.client,
                                               (gchar*)"localhost",
                                                CONTROL_PORT,
                                                NULL,
                                                &error);
    /* don't forget to check for errors */
  if (error != NULL)
  {
    g_print ("%s", error->message);
  }
  else
  {
    g_print ("Connection successful!\n");
  }
}

void cortrol_service_cleanup()
{
  g_object_unref(control_service_data.client);
  if (control_service_data.connection != NULL)
  {
    g_object_unref(control_service_data.connection);
  }
}

gint main (gint argc, gchar * argv[])
{
  ui_init();
  media_init();
  cortrol_service_init();

  gtk_main();

  media_cleanup();
  cortrol_service_cleanup();

  return 0;
}
