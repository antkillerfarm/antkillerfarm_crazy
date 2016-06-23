#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gst/gst.h>
#include <gst/net/gstnetclientclock.h>
#include <gst/net/gstnettimeprovider.h>

#include "logging.h"
#include "upnp_control_point.h"
#include "output_module.h"
#include "output_gstreamer.h"

static GstNetTimeProvider* create_net_clock (gint *port)
{
	GstClock *clock;
	GstNetTimeProvider *net_time;

	clock = gst_system_clock_obtain ();
	net_time = gst_net_time_provider_new (clock, UpnpGetServerIpAddress(), 0);
	g_object_get (net_time, "port", port, NULL);
	gst_object_unref (clock);

	return net_time;
}

int slave_num = 0;

#if (TRANS_TYPE==TRANS_TYPE_RTP)
int add_slave_to_pipeline(char* ip_addr)
{
	GstElement *queue1;
	GstElement *rtppay;
	GstElement *udp_sink;
	char name[20];

	sprintf(name, "queue%d", slave_num);
	queue1 = gst_element_factory_make ("queue", name);
	sprintf(name, "rtppay%d", slave_num);
	rtppay = gst_element_factory_make ("rtpgstpay", name);
	sprintf(name, "udp_sink%d", slave_num);
        udp_sink = gst_element_factory_make ("udpsink", name);
	slave_num++;

	gst_bin_add_many (GST_BIN (player_), queue1, rtppay, udp_sink, NULL);
	
	if (gst_element_link_many (gst_data.tee, queue1, rtppay, udp_sink, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 4\n");
		//gst_object_unref (player_);
	}

	g_object_set (udp_sink, "host", ip_addr, NULL);
	g_print ("add_slave_to_pipeline %s\n", ip_addr);
	g_object_set (udp_sink, "port", MEDIA_PORT, NULL);
	return 0;
}

int output_gstreamer_init_master(void)
{
	GstBus *bus;
	GstElement *queue0;
	GstElement *audio_sink0;
	GstElement *convert;

	GstClock *client_clock;
	//GstNetTimeProvider *prov_clock;
	
        create_net_clock(&(gst_data.clock_port));
	g_print("clock port: %d\n", gst_data.clock_port);
	client_clock = gst_net_client_clock_new(NULL, UpnpGetServerIpAddress(), gst_data.clock_port, 0);
	//g_usleep (G_USEC_PER_SEC / 2);

	player_ = gst_pipeline_new("audio_player_master");
	gst_data.source = gst_element_factory_make ("uridecodebin", "source");
	gst_data.tee = gst_element_factory_make ("tee", "tee");
	queue0 = gst_element_factory_make ("queue", "queue");
	convert = gst_element_factory_make("audioconvert", "convert");
        audio_sink0 = gst_element_factory_make ("autoaudiosink", "audio_sink");

	if (!player_ || !gst_data.source || !gst_data.tee || !queue0 || !convert || !audio_sink0)
	{
		g_print ("Not all elements could be created.\n");
	}

	gst_bin_add_many (GST_BIN (player_), gst_data.source, gst_data.tee, queue0, convert, audio_sink0, NULL);

	if (gst_element_link_many (gst_data.tee, queue0, convert, audio_sink0, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 1\n");
		//gst_object_unref (player_);
	}
	
	g_signal_connect (gst_data.source, "pad-added", G_CALLBACK (pad_added_handler), gst_data.tee);

	gst_pipeline_use_clock (GST_PIPELINE (player_), client_clock);
	//gst_element_set_start_time (player_, GST_CLOCK_TIME_NONE);
	//gst_pipeline_set_latency (GST_PIPELINE (player_), GST_SECOND / 2);

	bus = gst_pipeline_get_bus(GST_PIPELINE(player_));
	gst_bus_add_watch(bus, my_bus_callback, NULL);
	gst_object_unref(bus);

	if (audio_sink != NULL) {
		GstElement *sink = NULL;
		Log_info("gstreamer", "Setting audio sink to %s; device=%s\n",
			 audio_sink, audio_device ? audio_device : "");
		sink = gst_element_factory_make (audio_sink, "sink");
		if (sink == NULL) {
		  Log_error("gstreamer", "Couldn't create sink '%s'",
			    audio_sink);
		} else {
		  if (audio_device != NULL) {
		    g_object_set (G_OBJECT(sink), "device", audio_device, NULL);
		  }
		  g_object_set (G_OBJECT (player_), "audio-sink", sink, NULL);
		}
	}
	if (videosink != NULL) {
		GstElement *sink = NULL;
		Log_info("gstreamer", "Setting video sink to %s", videosink);
		sink = gst_element_factory_make (videosink, "sink");
		g_object_set (G_OBJECT (player_), "video-sink", sink, NULL);
	}

	if (gst_element_set_state(player_, GST_STATE_READY) ==
	    GST_STATE_CHANGE_FAILURE) {
		Log_error("gstreamer", "Error: pipeline doesn't become ready.");
	}
	gstreamer_output.get_volume = NULL;
	gstreamer_output.set_volume = NULL;
	return 0;
}
#else
int add_slave_to_pipeline(char* ip_addr)
{
	GstElement *queue1;
	GstElement *tcp_sink;
	char name[20];

	sprintf(name, "queue%d", slave_num);
        queue1 = gst_element_factory_make ("queue", name);
	sprintf(name, "tcp_sink%d", slave_num);
	tcp_sink = gst_element_factory_make ("tcpclientsink", name);
	slave_num++;

	gst_bin_add_many (GST_BIN (player_), queue1, tcp_sink, NULL);
	
	if (gst_element_link_many (gst_data.tee, queue1, tcp_sink, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 4\n");
		//gst_object_unref (player_);
	}

	g_object_set (tcp_sink, "host", ip_addr, NULL);
	g_print ("add_slave_to_pipeline %s\n", ip_addr);
	g_object_set (tcp_sink, "port", MEDIA_PORT, NULL);
	return 0;
}

int output_gstreamer_init_master(void)
{
	GstBus *bus;
	GstElement *queue0;
	GstElement *decode_bin;
	GstElement *audio_sink0;
	GstElement *convert;
	
	GstClock *client_clock;
	GstNetTimeProvider *prov_clock;
	
	prov_clock = create_net_clock(&(gst_data.clock_port));
	g_print("clock port: %d\n", gst_data.clock_port);
	client_clock = gst_net_client_clock_new(NULL, UpnpGetServerIpAddress(), gst_data.clock_port, 0);
	//g_usleep (G_USEC_PER_SEC / 2);	

	player_ = gst_pipeline_new("audio_player_master");
	gst_data.source = gst_element_factory_make ("souphttpsrc", "source");
	gst_data.tee = gst_element_factory_make ("tee", "tee");
	queue0 = gst_element_factory_make ("queue", "queue");
	decode_bin = gst_element_factory_make ("decodebin", "decode_bin");
	convert = gst_element_factory_make("audioconvert", "convert");
        audio_sink0 = gst_element_factory_make ("autoaudiosink", "audio_sink");

	if (!player_ || !gst_data.source || !gst_data.tee || !queue0 || !decode_bin || !convert || !audio_sink0)
	{
		g_print ("Not all elements could be created.\n");
	}

	gst_bin_add_many (GST_BIN (player_), gst_data.source, gst_data.tee, queue0, decode_bin, convert, audio_sink0, NULL);

	if (gst_element_link_many (gst_data.source, gst_data.tee, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 1\n");
		//gst_object_unref (player_);
	}

	if (gst_element_link_many (gst_data.tee, queue0, decode_bin, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 2\n");
		//gst_object_unref (player_);
	}

	if (gst_element_link_many (convert, audio_sink0, NULL) != TRUE)
	{
		g_print ("Elements could not be linked. 3\n");
		//gst_object_unref (player_);
	}
	
	g_signal_connect (decode_bin, "pad-added", G_CALLBACK (pad_added_handler), convert);

	gst_pipeline_use_clock (GST_PIPELINE (player_), client_clock);
	//gst_element_set_start_time (player_, GST_CLOCK_TIME_NONE);
	//gst_pipeline_set_latency (GST_PIPELINE (player_), GST_SECOND / 2);

	bus = gst_pipeline_get_bus(GST_PIPELINE(player_));
	gst_bus_add_watch(bus, my_bus_callback, NULL);
	gst_object_unref(bus);

	if (audio_sink != NULL) {
		GstElement *sink = NULL;
		Log_info("gstreamer", "Setting audio sink to %s; device=%s\n",
			 audio_sink, audio_device ? audio_device : "");
		sink = gst_element_factory_make (audio_sink, "sink");
		if (sink == NULL) {
		  Log_error("gstreamer", "Couldn't create sink '%s'",
			    audio_sink);
		} else {
		  if (audio_device != NULL) {
		    g_object_set (G_OBJECT(sink), "device", audio_device, NULL);
		  }
		  g_object_set (G_OBJECT (player_), "audio-sink", sink, NULL);
		}
	}
	if (videosink != NULL) {
		GstElement *sink = NULL;
		Log_info("gstreamer", "Setting video sink to %s", videosink);
		sink = gst_element_factory_make (videosink, "sink");
		g_object_set (G_OBJECT (player_), "video-sink", sink, NULL);
	}

	if (gst_element_set_state(player_, GST_STATE_READY) ==
	    GST_STATE_CHANGE_FAILURE) {
		Log_error("gstreamer", "Error: pipeline doesn't become ready.");
	}
	gstreamer_output.get_volume = NULL;
	gstreamer_output.set_volume = NULL;
	return 0;
}
#endif

int add_slave_to_control(struct UpDeviceNode *devnode)
{
	GError * error = NULL;
	g_print ("%s\n", __FUNCTION__);

	devnode->user_data.client = g_socket_client_new();
	devnode->user_data.connection = g_socket_client_connect_to_host (devnode->user_data.client, devnode->user_data.ip_addr, CONTROL_PORT, NULL, &error);
	if (error != NULL)
	{
		g_print ("%s\n", error->message);
	}
	else
	{
		g_print ("Connect to %s is successful!\n", devnode->user_data.ip_addr);
	}
	return 0;
}
