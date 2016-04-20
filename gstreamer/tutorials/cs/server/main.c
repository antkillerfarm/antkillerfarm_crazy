#include <gst/gst.h>
#include <string.h>

#define MAX_PATH 256

typedef struct{
  GstElement *playbin;
  GstElement *source;
  GstElement *decode_bin;
  GstElement *audio_sink;
}GstData;

char g_filename[MAX_PATH];

GstData gst_data;

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

void media_init()
{
  GstBus *bus;

  gst_init (NULL, NULL);

  gst_data.playbin = gst_pipeline_new("audio_player_server");
  gst_data.source = gst_element_factory_make ("tcpserversrc", "source");
  gst_data.decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
  gst_data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");

  if (!gst_data.playbin || !gst_data.source || !gst_data.decode_bin || !gst_data.audio_sink)
    {
      g_print ("Not all elements could be created.\n");
    }

  gst_bin_add_many (GST_BIN (gst_data.playbin), gst_data.source, gst_data.decode_bin, gst_data.audio_sink, NULL);

  if (gst_element_link_many (gst_data.source, gst_data.decode_bin, NULL) != TRUE)
    {
      g_print ("Elements could not be linked.\n");
      gst_object_unref (gst_data.playbin);
    }

  g_object_set (gst_data.source, "port", 3000, NULL);

  bus = gst_element_get_bus (gst_data.playbin);
  gst_bus_add_watch (bus, bus_call, NULL);
  g_object_unref (bus);
}

void media_cleanup()
{
  gst_element_set_state (gst_data.playbin, GST_STATE_NULL);
  g_object_unref (gst_data.playbin);
}

static GMainLoop *main_loop_ = NULL;
static void exit_loop_sighandler(int sig)
{
  if (main_loop_)
    {
      g_main_loop_quit(main_loop_);
    }
}

int server_main_loop()
{
  /* Create a main loop that runs the default GLib main context */
  main_loop_ = g_main_loop_new(NULL, FALSE);

  signal(SIGINT, &exit_loop_sighandler);
  signal(SIGTERM, &exit_loop_sighandler);

  g_main_loop_run(main_loop_);

  return 0;
}

gint main (gint argc, gchar * argv[])
{
  media_init();

  server_main_loop();

  return 0;
}

