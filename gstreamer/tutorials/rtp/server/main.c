#include <string.h>
#include <glib.h>
#include <gio/gio.h>
#include <gst/gst.h>

#define MAX_PATH 256
#define MEDIA_PORT 1500
#define CONTROL_PORT 1501

typedef struct{
  GstElement *playbin;
  GstElement *source;
  GstElement *rtpdepay;
  GstElement *decode_bin;
  GstElement *audio_sink;
  GstElement *queue;
}GstData;

typedef struct{
  GSocketService *service;
  GString *cmd_buf;
}ControlServiceData;

typedef struct{
  gchar cmd_name[10];
  gint arg_num;
  void (*cmd_handler)(gchar **arg_strv, gint arg_num);
}CommandFormat;

GstData gst_data;
ControlServiceData control_service_data;
guint source_id = 0;
GTimer *queue_timer = NULL;

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
      g_print ("Debugging info: %s\n", (debug) ? debug : "none");
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

gboolean queue_underrun_timeout_handler(gpointer user_data)
{
  g_print("%s\n", __FUNCTION__);
  source_id = 0;
  gst_element_set_state (gst_data.playbin, GST_STATE_NULL);
  gst_element_set_state (gst_data.playbin, GST_STATE_PLAYING);
  return FALSE;
}

#define IDLE_TIME 1 //unit:s
gint underrun_idx = 0;
void queue_underrun_handler(GstElement *queue, gpointer user_data)
{
  gdouble timer_elapse;
  g_print("%s 0\n", __FUNCTION__);
  if (queue_timer == NULL)
    {
      queue_timer = g_timer_new();
    }
  else
    {
      timer_elapse = g_timer_elapsed(queue_timer, NULL);
      g_print("%s : %f\n", __FUNCTION__, timer_elapse);
      g_timer_start(queue_timer);
      if (timer_elapse < IDLE_TIME)
	{
	  underrun_idx++;
	  if (underrun_idx > 8)
	    {
	      if (source_id)
		{
		  g_source_remove(source_id);
		}
	      source_id = g_timeout_add(IDLE_TIME * 1000, queue_underrun_timeout_handler, NULL);
	      g_print("%s 1\n", __FUNCTION__);
	    }
	}
      else
	{
	  underrun_idx = 0;
	  g_print("%s 2\n", __FUNCTION__);
	}
    }


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
  gst_data.source = gst_element_factory_make ("udpsrc", "source");
  gst_data.queue = gst_element_factory_make ("queue", "queue");
  gst_data.rtpdepay = gst_element_factory_make ("rtpgstdepay", "rtpdepay");
  gst_data.decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
  gst_data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");

  if (!gst_data.playbin || !gst_data.source || !gst_data.queue || !gst_data.rtpdepay || !gst_data.decode_bin || !gst_data.audio_sink)
    {
      g_print ("Not all elements could be created.\n");
    }

  gst_bin_add_many (GST_BIN (gst_data.playbin), gst_data.source, gst_data.queue, gst_data.rtpdepay, gst_data.decode_bin, gst_data.audio_sink, NULL);

  if (gst_element_link_many (gst_data.source, gst_data.queue, gst_data.rtpdepay, gst_data.decode_bin, NULL) != TRUE)
    {
      g_print ("Elements could not be linked.\n");
      gst_object_unref (gst_data.playbin);
    }

  g_object_set (gst_data.source, "port", MEDIA_PORT, NULL);

  GstCaps *caps = gst_caps_new_simple ("application/x-rtp",
				       "media", G_TYPE_STRING, "application",
				       "payload", G_TYPE_INT, 96,
				       "clock-rate", G_TYPE_INT, 90000,
				       "encoding-name", G_TYPE_STRING, "X-GST",
				       NULL);
  g_object_set (gst_data.source, "caps", caps, NULL);
  gst_caps_unref (caps);
  g_signal_connect (gst_data.decode_bin, "pad-added", G_CALLBACK (pad_added_handler), NULL);
  g_signal_connect (gst_data.queue, "underrun", G_CALLBACK (queue_underrun_handler), NULL);

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

void cortrol_service_cleanup()
{
  g_object_unref (control_service_data.service);
}

static GMainLoop *main_loop_ = NULL;
static void exit_loop_sighandler(int sig)
{
  media_cleanup();
  
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

void cmd_do_play(gchar **arg_strv, gint arg_num)
{
  g_print ("%s\n", __func__);
  if (gst_element_set_state(gst_data.playbin, GST_STATE_PLAYING) ==
      GST_STATE_CHANGE_FAILURE) {
    g_print("gstreamer setting play state failed (2)\n");
  }
}

void cmd_do_pause(gchar **arg_strv, gint arg_num)
{
  g_print ("%s\n", __func__);
  if (gst_element_set_state(gst_data.playbin, GST_STATE_PAUSED) ==
      GST_STATE_CHANGE_FAILURE) {
    g_print("gstreamer setting play state failed (3)\n");
  }
}

void cmd_do_stop(gchar **arg_strv, gint arg_num)
{
  g_print ("%s\n", __func__);
  gst_element_set_state (gst_data.playbin, GST_STATE_NULL);
}

CommandFormat cmd_format[] =
  {
    {"Play", 1, cmd_do_play},
    {"Pause", 1, cmd_do_pause},
    {"Stop", 1, cmd_do_stop}
  };

gboolean are_cmd_args_valid(gchar **arg_strv, gint arg_num)
{
  gint i;
  for (i = 0; i < arg_num; i++)
    {
      if (strlen(arg_strv[i]) == 0)
	{
	  return FALSE;
	}
    }
  return TRUE;
}

#define ARG_PIECES 4

void cmd_handler(gchar* cmd_str)
{
  gchar **arg_strv;
  gint strv_len, i, cmd_format_size;
  arg_strv = g_strsplit(cmd_str, "#", ARG_PIECES);
  strv_len = g_strv_length(arg_strv);
  if (strv_len == 0)
    {
      g_strfreev(arg_strv);
      return;
    }
  cmd_format_size = sizeof(cmd_format) / sizeof(CommandFormat);
  for (i = 0; i < cmd_format_size; i++)
    {
      if (g_strcmp0(arg_strv[0], cmd_format[i].cmd_name) == 0)
	{
	  if (are_cmd_args_valid(arg_strv, cmd_format[i].arg_num))
	    {
	      cmd_format[i].cmd_handler(arg_strv, cmd_format[i].arg_num);
	    }
	}
    }

  g_strfreev(arg_strv);
}

#define CMD_BUF_PIECES 4

void cmd_buf_handler()
{
  gchar **cmd_strv;
  gint strv_len, i;

  do
    {
      cmd_strv = g_strsplit(control_service_data.cmd_buf->str, "\n", CMD_BUF_PIECES);
      strv_len = g_strv_length(cmd_strv);
      if (strv_len == 0)
	{
	  g_strfreev(cmd_strv);
	  return;
	}
      for (i = 0; i < strv_len - 1; i++)
	{
	  if (strlen(cmd_strv[i]) > 0)
	    {
	      g_print("Cmd was : \"%s\"\n", cmd_strv[i]);
	      cmd_handler(cmd_strv[i]);
	    }
	}
      g_string_free(control_service_data.cmd_buf, TRUE);
      control_service_data.cmd_buf = g_string_new_len(cmd_strv[strv_len - 1], strlen(cmd_strv[strv_len -1]));
      g_strfreev(cmd_strv);
    } while (strv_len == CMD_BUF_PIECES);
}

/* this function will get called everytime a client attempts to connect */
#define MSG_LEN 256
gboolean incoming_callback  (GSocketService *service,
                    GSocketConnection *connection,
                    GObject *source_object,
                    gpointer user_data)
{
  gint read_cnt;
  g_print("Received Connection from client!\n");
  GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  gchar message[MSG_LEN];
  do
    {
      memset(message, 0, MSG_LEN);
      read_cnt = g_input_stream_read  (istream,
			    message,
			    MSG_LEN,
			    NULL,
			    NULL);
      g_print("Message was %d: \"%s\"\n", read_cnt, message);
      if (read_cnt > 0)
	{
	  if (control_service_data.cmd_buf == NULL)
	    {
	      control_service_data.cmd_buf = g_string_new_len(message, read_cnt);
	    }
	  else
	    {
	      control_service_data.cmd_buf = g_string_append_len(control_service_data.cmd_buf, message, read_cnt);
	    }
	  cmd_buf_handler();
	}
    } while (read_cnt > 0);
  g_print("Client disconnection!\n");
  if (control_service_data.cmd_buf != NULL)
    {
      g_string_free(control_service_data.cmd_buf, TRUE);
      control_service_data.cmd_buf = NULL;
    }
  return FALSE;
}

void cortrol_service_init()
{
  GError * error = NULL;
  control_service_data.cmd_buf = NULL;

  /* create the new socketservice */
  control_service_data.service = g_threaded_socket_service_new(1);

  /* connect to the port */
  g_socket_listener_add_inet_port ((GSocketListener*)control_service_data.service,
                                    CONTROL_PORT,
                                    NULL,
                                    &error);

  /* don't forget to check for errors */
  if (error != NULL)
  {
    g_error ("%s", error->message);
  }

  /* listen to the 'incoming' signal */
  g_signal_connect (control_service_data.service,
                    "run",
                    G_CALLBACK (incoming_callback),
                    NULL);

  /* start the socket service */
  g_socket_service_start (control_service_data.service);

  /* enter mainloop */
  g_print ("Waiting for client!\n");
}

gint main (gint argc, gchar * argv[])
{
  media_init();
  cortrol_service_init();

  server_main_loop();

  return 0;
}

