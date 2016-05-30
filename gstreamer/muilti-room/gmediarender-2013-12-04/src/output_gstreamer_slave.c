#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gst/gst.h>
#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>
#include "output_module.h"
#include "output_gstreamer.h"

static void slave_pad_added_handler (GstElement *src, GstPad *new_pad, gpointer data)
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

#if 0
int output_gstreamer_init_slave(void)
{
        GstElement *source;
	GstElement *rtpdepay;
	GstElement *decode_bin;

	player_ = gst_pipeline_new("audio_player_slave");
	source = gst_element_factory_make ("udpsrc", "source");
	rtpdepay = gst_element_factory_make ("rtpgstdepay", "rtpdepay");
	decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
        gst_data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");
	
	if (!player_ || !source || !rtpdepay || !decode_bin || !gst_data.audio_sink)
	{
		g_print ("Not all elements could be created.\n");
	}

	gst_bin_add_many (GST_BIN (player_), source, rtpdepay, decode_bin, gst_data.audio_sink, NULL);

	if (gst_element_link_many (source, rtpdepay, decode_bin, NULL) != TRUE)
	{
		g_print ("Elements could not be linked.\n");
		gst_object_unref (player_);
	}

	g_object_set (source, "port", MEDIA_PORT, NULL);
	
	g_signal_connect (player_, "pad-added", G_CALLBACK (slave_pad_added_handler), NULL);

	return 0;
}
#endif

#if 1
int output_gstreamer_init_slave(void)
{
        GstElement *source;
	GstElement *decode_bin;

	player_ = gst_pipeline_new("audio_player_slave");
	source = gst_element_factory_make ("tcpserversrc", "source");
	decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
        gst_data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");
	
	if (!player_ || !source || !decode_bin || !gst_data.audio_sink)
	{
		g_print ("Not all elements could be created.\n");
	}

	gst_bin_add_many (GST_BIN (player_), source, decode_bin, gst_data.audio_sink, NULL);

	if (gst_element_link_many (source, decode_bin, NULL) != TRUE)
	{
		g_print ("Elements could not be linked.\n");
		gst_object_unref (player_);
	}
	
	g_object_set (source, "host", UpnpGetServerIpAddress(), NULL);
	g_print ("tcpserversrc %s\n", UpnpGetServerIpAddress());
	g_object_set (source, "port", MEDIA_PORT, NULL);
	
	g_signal_connect (player_, "pad-added", G_CALLBACK (slave_pad_added_handler), NULL);

	gst_element_set_state (gst_data.playbin, GST_STATE_PLAYING);
	return 0;
}
#endif

