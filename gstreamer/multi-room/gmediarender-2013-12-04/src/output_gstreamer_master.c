#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gst/gst.h>

#include "upnp_control_point.h"
#include "output_module.h"
#include "output_gstreamer.h"

static void master_pad_added_handler (GstElement *src, GstPad *new_pad, gpointer data)
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

# if 0
int add_slave_to_pipeline(char* ip_addr)
{
	return 0;
}

int output_gstreamer_init_master(void)
{
	GstElement *source;
	GstElement *rtppay;
	GstElement *decode_bin;

	player_ = gst_pipeline_new("audio_player_master");
	source = gst_element_factory_make ("uridecodebin", "source");
	rtppay = gst_element_factory_make ("rtpgstpay", "rtppay");
	decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
        gst_data.udp_sink = gst_element_factory_make ("udpsink", "udp_sink");

	if (!player_ || !source || !rtppay || !decode_bin || !gst_data.udp_sink)
	{
		g_print ("Not all elements could be created.\n");
	}

	gst_bin_add_many (GST_BIN (player_), source, rtppay, decode_bin, gst_data.udp_sink, NULL);
	
	g_signal_connect (player_, "pad-added", G_CALLBACK (master_pad_added_handler), NULL);
	return 0;
}
#endif

# if 1
int add_slave_to_pipeline(char* ip_addr)
{
	GstElement *queue1;
	GstElement *tcp_sink;
        queue1 = gst_element_factory_make ("queue", "queue1");
	tcp_sink = gst_element_factory_make ("tcpclientsink", "tcp_sink");

	gst_bin_add_many (GST_BIN (player_), queue1, tcp_sink, NULL);
	
	if (gst_element_link_many (gst_data.tee, queue1, tcp_sink, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 2\n");
		//gst_object_unref (player_);
	}

	g_object_set (tcp_sink, "host", ip_addr, NULL);
	g_print ("add_slave_to_pipeline %s\n", ip_addr);
	g_object_set (tcp_sink, "port", MEDIA_PORT, NULL);
	return 0;
}

int output_gstreamer_init_master(void)
{
	GstElement *source;
	GstElement *queue0;
	GstElement *decode_bin;

	player_ = gst_pipeline_new("audio_player_master");
	source = gst_element_factory_make ("souphttpsrc", "source");
	gst_data.tee = gst_element_factory_make ("tee", "tee");
	queue0 = gst_element_factory_make ("queue", "queue");
	decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
	gst_data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");

	if (!player_ || !source || !gst_data.tee || !queue0 || !decode_bin || !gst_data.audio_sink)
	{
		g_print ("Not all elements could be created.\n");
	}

	gst_bin_add_many (GST_BIN (player_), source, gst_data.tee, queue0, decode_bin, gst_data.audio_sink, NULL);

	if (gst_element_link_many (source, gst_data.tee, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 1\n");
		//gst_object_unref (player_);
	}

	if (gst_element_link_many (gst_data.tee, queue0, decode_bin, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 2\n");
		//gst_object_unref (player_);
	}
	
	g_signal_connect (player_, "pad-added", G_CALLBACK (master_pad_added_handler), NULL);
	return 0;
}
#endif

int add_slave_to_control(struct UpDeviceNode *devnode)
{

	return 0;
}

