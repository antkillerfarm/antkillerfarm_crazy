/*
 * Sample code for: Synchronised multi-device media playback with GStreamer
 * This player syncs with the clock created by the leader player
 *
 * Copyright (c) 2016 Luis de Bethencourt <luisbg@osg.samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <gst/net/gstnetclientclock.h>
#include <gst/net/gstnettimeprovider.h>

GstElement *playbin;

static GstClockTime
get_base_time (guint16 *clock_port)
{
  GstClockTime base_time;
  FILE *fp;

  fp = fopen ("/tmp/shared_time", "rb");
  if (!fp)
    g_print ("problem reading shared_time file");
  fread (clock_port, sizeof (guint16), 1, fp);
  fread (&base_time, sizeof (GstClockTime), 1, fp);
  fclose (fp);

  return base_time;
}

int main(int argc, char *argv[]) {
  GMainLoop *main_loop;
  GstClock *client_clock, *tmp_clock;
  guint16 clock_port;
  GstClockTime base_time;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);
  base_time = get_base_time (&clock_port);

  client_clock = gst_net_client_clock_new (NULL, "127.0.0.1", clock_port, 0);

  /* Wait 0.5 seconds for the clock to stabilise */
  g_usleep (G_USEC_PER_SEC / 2);

  /* Create the elements */
  playbin = gst_element_factory_make ("playbin", "playbin");
  g_object_set (playbin, "uri", "file:///home/luisbg/samples/big_buck_bunny_1080p_h264.mov", NULL);

  gst_pipeline_use_clock (GST_PIPELINE (playbin), client_clock);
  gst_element_set_base_time (playbin, base_time);
  gst_element_set_start_time (playbin, GST_CLOCK_TIME_NONE);
  gst_pipeline_set_latency (GST_PIPELINE (playbin), GST_SECOND / 2);

  gst_element_set_state (playbin, GST_STATE_PLAYING);

  /* Create a GLib Main Loop and set it to run */
  main_loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (main_loop);

  /* Free resources */
  g_main_loop_unref (main_loop);
  gst_element_set_state (playbin, GST_STATE_NULL);
  gst_object_unref (playbin);
  return 0;
}
