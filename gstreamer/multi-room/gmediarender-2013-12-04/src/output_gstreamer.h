/* output_gstreamer.h - Definitions for GStreamer output module
 *
 * Copyright (C) 2005-2007   Ivo Clarysse
 *
 * This file is part of GMediaRender.
 *
 * GMediaRender is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GMediaRender is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GMediaRender; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301, USA.
 *
 */

#ifndef _OUTPUT_GSTREAMER_H
#define _OUTPUT_GSTREAMER_H

#define MEDIA_PORT 1500
#define CONTROL_PORT 1501

#if 0
typedef struct{
	GstElement *audio_sink;
	GstElement *udp_sink;
}GstData;
#endif

#if 1
typedef struct{
	GstElement *audio_sink;
	GstElement *tee;
}GstData;
#endif

extern struct output_module gstreamer_output;
extern GstData gst_data;
extern GstElement *player_;
extern gchar *audio_sink;
extern gchar *audio_device;
extern gchar *videosink;

int output_gstreamer_init_master(void);
int output_gstreamer_init_slave(void);
int output_gstreamer_control_init_slave(void);
int add_slave_to_pipeline(char* ip_addr);
int add_slave_to_control(struct UpDeviceNode *devnode);
gboolean my_bus_callback(GstBus * bus, GstMessage * msg, gpointer data);

#endif /*  _OUTPUT_GSTREAMER_H */
