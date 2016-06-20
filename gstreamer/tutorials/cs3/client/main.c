#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <string.h>
#include <gst/net/gstnetclientclock.h>
#include <gst/net/gstnettimeprovider.h>

#define MAX_PATH 256
#define MEDIA_PORT 1500
#define CONTROL_PORT 1501

#define SRC_TYPE_FILE 0
#define SRC_TYPE_HTTP 1
#define SRC_TYPE SRC_TYPE_FILE

#define TRANS_TYPE_TCP 0
#define TRANS_TYPE_RTP 1
#define TRANS_TYPE TRANS_TYPE_RTP

typedef struct{
  GtkStatusbar *statusbar;
  gint contextId;
  GtkEntry *entry1;
  GtkScale *slider;
  GtkLabel *label;
}MainWindowSubWidget;

typedef struct{
  GstElement *playbin;
  GstElement *source;
  GstElement *tee;
  GstElement *queue0;
  GstElement *decode_bin;
  GstElement *convert;
  GstElement *audio_sink;
}GstData;

typedef struct{
  char *server_ip;
  GSocketConnection *connection;
  GSocketClient *client;
}ControlServiceData;

char g_filename[MAX_PATH];

MainWindowSubWidget main_window_sub_widget;
GstData gst_data;
gint clock_port;

#define SERVER_LIST_NUM 1
#define CLIENT_IP "192.168.3.105"

ControlServiceData control_service_data[] =
{
  {"192.168.3.105", NULL, NULL},
  //{"192.168.3.103", NULL, NULL},
};

static GstState get_current_player_state() {
  GstState state = GST_STATE_PLAYING;
  GstState pending = GST_STATE_NULL;
  gst_element_get_state(gst_data.playbin, &state, &pending, 0);
  return state;
}

static GstNetTimeProvider* create_net_clock (gint *port)
{
  GstClock *clock;
  GstNetTimeProvider *net_time;

  clock = gst_system_clock_obtain ();
  net_time = gst_net_time_provider_new (clock, NULL, 0);
  g_object_get (net_time, "port", port, NULL);
  gst_object_unref (clock);

  return net_time;
}

static gboolean bus_call (GstBus * bus, GstMessage * msg, gpointer data)
{
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:{
      //gst_element_set_state (gst_data.playbin, GST_STATE_READY);
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

  int i;
  for (i = 0; i < SERVER_LIST_NUM; i++)
    {
      /* use the connection */
      //GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (control_service_data[i].connection));
      GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (control_service_data[i].connection));
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
      //g_strlcpy(g_filename, filename, MAX_PATH);
      gtk_statusbar_push(GTK_STATUSBAR(main_window_sub_widget.statusbar), 
		     main_window_sub_widget.contextId, filename);
      gtk_entry_set_text(main_window_sub_widget.entry1, filename);
      g_free (filename);
    }

  gtk_widget_destroy (dialog);
}

gboolean update_slider(gpointer user_data)
{
  gint64 nanosecs, duration_nanosecs;
  double duration, position;
  char str[64];
  if (get_current_player_state() != GST_STATE_PLAYING)
    {
      return FALSE;
    }

  gst_element_query_position(gst_data.playbin, GST_FORMAT_TIME, &nanosecs);
  gst_element_query_duration(gst_data.playbin, GST_FORMAT_TIME, &duration_nanosecs);

  duration = (double)(duration_nanosecs) / GST_SECOND;
  position = (double)(nanosecs) / GST_SECOND;

  gtk_range_set_range(GTK_RANGE(main_window_sub_widget.slider), 0, duration);
  gtk_range_set_value(GTK_RANGE(main_window_sub_widget.slider), position);
  sprintf(str, "%d:%02d", (int)(position / 60), (int)(position) % 60);

  g_print("%s:%f %s\n", __FUNCTION__, duration, str);
  gtk_label_set_text(GTK_LABEL(main_window_sub_widget.label), str);
  return TRUE;
}

G_MODULE_EXPORT void do_button_play_clicked(GtkButton *button, gpointer data)
{
  gchar *uri;
  const gchar * content = gtk_entry_get_text(main_window_sub_widget.entry1);
  g_strlcpy(g_filename, content, MAX_PATH);
  gst_element_set_state(gst_data.playbin, GST_STATE_READY);

#if (TRANS_TYPE == TRANS_TYPE_TCP)
  uri = g_strdup (g_filename);
  g_object_set (gst_data.source, "location", uri, NULL);
#else
  if (gst_uri_is_valid (g_filename))
    uri = g_strdup (g_filename);
  else
    uri = gst_filename_to_uri (g_filename, NULL);
  g_object_set (gst_data.source, "uri", uri, NULL);
#endif

  g_print("%s: %s", __FUNCTION__, uri);
  g_free (uri);

  gst_element_set_state (gst_data.playbin, GST_STATE_PLAYING);
  g_timeout_add(1000, update_slider, NULL);
}

G_MODULE_EXPORT void do_button_next_clicked(GtkButton *button, gpointer data)
{
  gchar *cmd = "Stop\n";
  send_cmd_to_server(cmd);
}

G_MODULE_EXPORT void do_button_pause_clicked(GtkButton *button, gpointer data)
{
#if (TRANS_TYPE == TRANS_TYPE_TCP)
  gchar *cmd = "Pause\n";
  send_cmd_to_server(cmd);
#endif
  gst_element_set_state(gst_data.playbin, GST_STATE_PAUSED);
}

G_MODULE_EXPORT void do_button_continue_clicked(GtkButton *button, gpointer data)
{
#if (TRANS_TYPE == TRANS_TYPE_TCP)
  gchar *cmd = "Play\n";
  send_cmd_to_server(cmd);
#endif
  gst_element_set_state(gst_data.playbin, GST_STATE_PLAYING);
}

G_MODULE_EXPORT void do_button_clock_clicked(GtkButton *button, gpointer data)
{
  gchar cmd[64];
  sprintf(cmd, "Clock#%s#%d\n", CLIENT_IP, clock_port);
  send_cmd_to_server(cmd);
}

G_MODULE_EXPORT gboolean slider_change_value (GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data)
{
  gint64 position_nanos = (gint64)(value * GST_SECOND);
  g_print("%s:%f\n", __FUNCTION__, value);
  gst_element_seek(gst_data.playbin, 1.0, GST_FORMAT_TIME,
		   GST_SEEK_FLAG_FLUSH,
		   GST_SEEK_TYPE_SET, position_nanos,
		   GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
  return FALSE;
}

void ui_init()
{
  GtkWidget *window = NULL;
  GtkBuilder *builder;
  GError *err = NULL;
  gchar *exec_name = NULL;
  gchar *tmp = NULL;
  gchar exe_path[MAX_PATH];
  GtkEntry *entry;

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

  entry = GTK_ENTRY(gtk_builder_get_object(builder, "entry1"));
  main_window_sub_widget.entry1 = entry;

  main_window_sub_widget.slider = GTK_SCALE(gtk_builder_get_object(builder, "scale1"));
  gtk_range_set_range(GTK_RANGE(main_window_sub_widget.slider), 0, 100);
  //gtk_scale_set_draw_value(GTK_SCALE(main_window_sub_widget.slider), FALSE);
  
  main_window_sub_widget.label = GTK_LABEL(gtk_builder_get_object(builder, "label1"));

  g_signal_connect(window, "destroy",
		   G_CALLBACK (gtk_main_quit), &window);

  g_object_unref(G_OBJECT(builder));

  gtk_widget_show_all(window);
}

static void pad_added_handler (GstElement *src, GstPad *new_pad, gpointer data)
{
#if (TRANS_TYPE == TRANS_TYPE_TCP)
  GstPad *sink_pad = gst_element_get_static_pad (gst_data.convert, "sink");
#else
  GstPad *sink_pad = gst_element_get_static_pad (gst_data.tee, "sink");
#endif
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

int server_num = 0;

int add_server_to_pipeline_tcp(char* ip_addr)
{
  GstElement *queue1;
  GstElement *tcp_sink;
  char name[20];
	
  sprintf(name, "queue%d", server_num);
  queue1 = gst_element_factory_make ("queue", name);
  sprintf(name, "tcp_sink%d", server_num);
  tcp_sink = gst_element_factory_make ("tcpclientsink", name);
  server_num++;

  gst_bin_add_many (GST_BIN (gst_data.playbin), queue1, tcp_sink, NULL);
	
  if (gst_element_link_many (gst_data.tee, queue1, tcp_sink, NULL) != TRUE)
    {
      g_print ("Elements could not be linked. 2\n");
      //gst_object_unref (player_);
    }

  g_object_set (tcp_sink, "host", ip_addr, NULL);
  g_print("%s: %s\n", __FUNCTION__, ip_addr);
  g_object_set (tcp_sink, "port", MEDIA_PORT, NULL);
  return 0;
}

int add_server_to_pipeline_rtp(char* ip_addr)
{
  GstElement *queue1;
  GstElement *udp_sink;
  GstElement *rtppay;
  char name[20];
	
  sprintf(name, "queue%d", server_num);
  queue1 = gst_element_factory_make ("queue", name);
  sprintf(name, "rtppay%d", server_num);
  rtppay = gst_element_factory_make ("rtpgstpay", name);
  sprintf(name, "udp_sink%d", server_num);
  udp_sink = gst_element_factory_make ("udpsink", name);
  server_num++;

  gst_bin_add_many (GST_BIN (gst_data.playbin), queue1, rtppay, udp_sink, NULL);
	
  if (gst_element_link_many (gst_data.tee, queue1, rtppay, udp_sink, NULL) != TRUE)
    {
      g_print ("Elements could not be linked. 1\n");
      //gst_object_unref (player_);
    }

  g_object_set (udp_sink, "host", ip_addr, NULL);
  g_print("%s: %s\n", __FUNCTION__, ip_addr);
  g_object_set (udp_sink, "port", MEDIA_PORT, NULL);
  return 0;
}

void gst_pipeline_tcp_init()
{
  gst_data.playbin = gst_pipeline_new("audio_player_client");
#if (SRC_TYPE == SRC_TYPE_HTTP)
  gst_data.source = gst_element_factory_make ("souphttpsrc", "source");
#else
  gst_data.source = gst_element_factory_make ("filesrc", "source");
#endif
  gst_data.tee = gst_element_factory_make ("tee", "tee");
  gst_data.queue0 = gst_element_factory_make ("queue", "queue");
  gst_data.decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
  gst_data.convert = gst_element_factory_make("audioconvert", "convert");
  gst_data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");

  if (!gst_data.playbin || !gst_data.source || !gst_data.tee || !gst_data.queue0 || !gst_data.decode_bin || !gst_data.convert || !gst_data.audio_sink)
    {
      g_print ("Not all elements could be created.\n");
    }

  gst_bin_add_many (GST_BIN (gst_data.playbin), gst_data.source, gst_data.tee, gst_data.queue0, gst_data.decode_bin, gst_data.convert, gst_data.audio_sink, NULL);

  if (gst_element_link_many (gst_data.source, gst_data.tee, NULL) != TRUE)
    {
      g_print ("Elements could not be linked. 1\n");
      gst_object_unref (gst_data.playbin);
    }

  if (gst_element_link_many (gst_data.tee, gst_data.queue0, gst_data.decode_bin, NULL) != TRUE)
    {
      g_print ("Elements could not be linked. 2\n");
      gst_object_unref (gst_data.playbin);
    }
  
  if (gst_element_link_many (gst_data.convert, gst_data.audio_sink, NULL) != TRUE)
    {
      g_print ("Elements could not be linked. 3\n");
      gst_object_unref (gst_data.playbin);
    }

    g_signal_connect (gst_data.decode_bin, "pad-added", G_CALLBACK (pad_added_handler), NULL);
}

void gst_pipeline_rtp_init()
{
  gst_data.playbin = gst_pipeline_new("audio_player_client");
  gst_data.source = gst_element_factory_make ("uridecodebin", "source");
  gst_data.tee = gst_element_factory_make ("tee", "tee");
  gst_data.queue0 = gst_element_factory_make ("queue", "queue");

  gst_data.convert = gst_element_factory_make("audioconvert", "convert");
  gst_data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");

  if (!gst_data.playbin || !gst_data.source || !gst_data.tee || !gst_data.queue0 || !gst_data.convert || !gst_data.audio_sink)
    {
      g_print ("Not all elements could be created.\n");
    }

  gst_bin_add_many (GST_BIN (gst_data.playbin), gst_data.source, gst_data.tee, gst_data.queue0, gst_data.convert, gst_data.audio_sink, NULL);

  if (gst_element_link_many (gst_data.tee, gst_data.queue0, gst_data.convert, gst_data.audio_sink, NULL) != TRUE)
    {
      g_print ("Elements could not be linked.\n");
      gst_object_unref (gst_data.playbin);
    }

  g_signal_connect (gst_data.source, "pad-added", G_CALLBACK (pad_added_handler), NULL);
}

void media_init()
{
  GstBus *bus;
  GstClock *client_clock;
  GstNetTimeProvider *prov_clock;

  gst_init (NULL, NULL);
  //gst_debug_set_default_threshold(GST_LEVEL_MEMDUMP);

  prov_clock = create_net_clock(&clock_port);
  g_print("clock port: %d\n", clock_port);
  client_clock = gst_net_client_clock_new(NULL, CLIENT_IP, clock_port, 0);
  g_usleep (G_USEC_PER_SEC / 2);

#if (TRANS_TYPE == TRANS_TYPE_TCP)
  gst_pipeline_tcp_init();
#else
  gst_pipeline_rtp_init();
#endif

  gst_pipeline_use_clock (GST_PIPELINE (gst_data.playbin), client_clock);
  gst_element_set_start_time (gst_data.playbin, GST_CLOCK_TIME_NONE);
  gst_pipeline_set_latency (GST_PIPELINE (gst_data.playbin), GST_SECOND / 2);
  
  int i;
  for (i = 0; i < SERVER_LIST_NUM; i++)
    {
#if (TRANS_TYPE == TRANS_TYPE_TCP)
      add_server_to_pipeline_tcp(control_service_data[i].server_ip);
#else
      add_server_to_pipeline_rtp(control_service_data[i].server_ip);
#endif
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
  int i;

  for (i = 0; i < SERVER_LIST_NUM; i++)
    {
      /* create a new connection */
      control_service_data[i].client = g_socket_client_new();

      /* connect to the host */
      control_service_data[i].connection = g_socket_client_connect_to_host (control_service_data[i].client,
          control_service_data[i].server_ip, CONTROL_PORT, NULL, &error);
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
}

void cortrol_service_cleanup()
{
  int i;
  for (i = 0; i < SERVER_LIST_NUM; i++)
    {
      g_object_unref(control_service_data[i].client);
      if (control_service_data[i].connection != NULL)
	{
	  g_object_unref(control_service_data[i].connection);
	}
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

