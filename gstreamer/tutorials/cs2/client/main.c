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
  GstElement *tee;
  GstElement *queue0;
  GstElement *queue1;
  GstElement *decode_bin;
  GstElement *tcp_sink;
  GstElement *audio_sink;
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

void send_cmd_to_server(gchar *cmd)
{
  GError * error = NULL;

  /* use the connection */
  //GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (control_service_data.connection));
  GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (control_service_data.connection));
  g_output_stream_write  (ostream,
                          cmd, /* your message goes here */
                          strlen(cmd), /* length of your message */
                          NULL,
                          &error);
  /* don't forget to check for errors */
  if (error != NULL)
  {
    g_print ("%s", error->message);
  }
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
  gchar *cmd = "EOS\n";
  send_cmd_to_server(cmd);
}

G_MODULE_EXPORT void do_button_pause_clicked(GtkButton *button, gpointer data)
{
  gchar *cmd = "Pause\n";
  send_cmd_to_server(cmd);
}

G_MODULE_EXPORT void do_button_continue_clicked(GtkButton *button, gpointer data)
{
  gchar *cmd = "Play\n";
  send_cmd_to_server(cmd);
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

static void pad_added_handler (GstElement *src, GstPad *new_pad, gpointer data)
{
  GstPad *sink_pad = gst_element_get_static_pad (gst_data.audio_sink, "sink");
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;
  guint caps_size = 0, i;

  g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));
  g_print ("sink_pad: '%s'\n", GST_PAD_NAME (sink_pad));

  if (gst_pad_is_linked (sink_pad)) {
    g_print ("We are already linked. Ignoring.\n");
    goto exit;
  }

  new_pad_caps = gst_pad_get_current_caps(new_pad);
  caps_size = gst_caps_get_size(new_pad_caps);
  g_print ("caps_size : %d\n", caps_size);
  for (i = 0; i < caps_size; i++)
    {
      new_pad_struct = gst_caps_get_structure(new_pad_caps, i);
      new_pad_type = gst_structure_get_name(new_pad_struct);
      g_print ("new_pad_type %d: '%s'\n", i, new_pad_type);
      if (strstr(new_pad_type, "audio/x-raw"))
	{
	    ret = gst_pad_link (new_pad, sink_pad);
	    if (GST_PAD_LINK_FAILED (ret)) {
	      g_print ("Type is '%s' but link failed.\n", new_pad_type);
	    } else {
	      g_print ("Link succeeded (type '%s').\n", new_pad_type);
	    }
	    break;
	}
    }

exit:
  /* Unreference the new pad's caps, if we got them */
  if (new_pad_caps != NULL)
    gst_caps_unref (new_pad_caps);
   
  /* Unreference the sink pad */
  gst_object_unref (sink_pad);
}

void media_init()
{
  GstBus *bus;

  gst_init (NULL, NULL);

  gst_data.playbin = gst_pipeline_new("audio_player_client");
  gst_data.source = gst_element_factory_make ("filesrc", "source");
  gst_data.tee = gst_element_factory_make ("tee", "tee");
  gst_data.queue0 = gst_element_factory_make ("queue", "queue0");
  gst_data.queue1 = gst_element_factory_make ("queue", "queue1");
  gst_data.decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
  gst_data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");
  gst_data.tcp_sink = gst_element_factory_make ("tcpclientsink", "tcp_sink");

  if (!gst_data.playbin || !gst_data.source || !gst_data.tcp_sink || !gst_data.tee || !gst_data.queue0 || !gst_data.queue1 || !gst_data.decode_bin || !gst_data.audio_sink)
    {
      g_print ("Not all elements could be created.\n");
    }

  gst_bin_add_many (GST_BIN (gst_data.playbin), gst_data.source, gst_data.tcp_sink, gst_data.tee, gst_data.queue0, gst_data.queue1, gst_data.decode_bin, gst_data.audio_sink, NULL);

  if (gst_element_link_many (gst_data.source, gst_data.tee, NULL) != TRUE)
    {
      g_print ("Elements could not be linked. 1\n");
      gst_object_unref (gst_data.playbin);
    }

  if (gst_element_link_many (gst_data.tee, gst_data.queue0, gst_data.tcp_sink, NULL) != TRUE)
    {
      g_print ("Elements could not be linked. 2\n");
      gst_object_unref (gst_data.playbin);
    }

  if (gst_element_link_many (gst_data.tee, gst_data.queue1, gst_data.decode_bin, NULL) != TRUE)
    {
      g_print ("Elements could not be linked. 3\n");
      gst_object_unref (gst_data.playbin);
    }

  g_signal_connect (gst_data.decode_bin, "pad-added", G_CALLBACK (pad_added_handler), NULL);
  
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

