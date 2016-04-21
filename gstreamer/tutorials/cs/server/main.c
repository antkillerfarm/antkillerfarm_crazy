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
  g_signal_connect (gst_data.decode_bin, "pad-added", G_CALLBACK (pad_added_handler), NULL);

  bus = gst_element_get_bus (gst_data.playbin);
  gst_bus_add_watch (bus, bus_call, NULL);
  g_object_unref (bus);

  gst_element_set_state (gst_data.playbin, GST_STATE_PLAYING);
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

