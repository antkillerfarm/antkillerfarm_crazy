/* output_gstreamer.c - Output module for GStreamer
 *
 * Copyright (C) 2005-2007   Ivo Clarysse
 *
 * Adapted to gstreamer-0.10 2006 David Siorpaes
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <gst/gst.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <totem-pl-parser.h>

#include "logging.h"
#include <upnp/upnp.h>
#include <upnp/ithread.h>
#include <upnp/upnptools.h>
#include "upnp_control.h"
#include "upnp_control_point.h"
#include "upnp_connmgr.h"
#include "upnp_transport.h"
#include "output.h"
#include "output_module.h"
#include "output_gstreamer.h"
#include "xmldoc.h"

int g_device_play_mode = DEVICE_PLAY_MODE_SINGLE;


#define MAX_PATH 256
#define MAX_TITLE_LENGTH 64

typedef struct{
	gchar title[MAX_TITLE_LENGTH];
	gchar uri[MAX_PATH];
}MediaInfo;

typedef struct{
	GSList* play_list;
	gint current_idx;
}PlayListInfo;

typedef struct{
	gboolean is_play_list;
}AppState;

PlayListInfo play_list_info = {0};
const gchar play_list_suffix[] = ".m3u .pls .xspf";
AppState app_state = {0};
GstData gst_data = {0};

//void load_playlist(const char* file_name);
//void load_playlist_file(const char* file_name);
gint get_next_current_idx(void);

static void scan_caps(const GstCaps * caps)
{
	guint i;

	g_return_if_fail(caps != NULL);

	if (gst_caps_is_any(caps)) {
		return;
	}
	if (gst_caps_is_empty(caps)) {
		return;
	}

	for (i = 0; i < gst_caps_get_size(caps); i++) {
		GstStructure *structure = gst_caps_get_structure(caps, i);
		const char *mime_type = gst_structure_get_name(structure);
		register_mime_type(mime_type);
	}
	// There seem to be all kinds of mime types out there that start with
	// "audio/" but are not explicitly supported by gstreamer. Let's just
	// tell the controller that we can handle everything "audio/*" and hope
	// for the best.
	register_mime_type("audio/*");
}

static void scan_pad_templates_info(GstElement *element,
				    GstElementFactory *factory)
{
	const GList *pads;
	GstPadTemplate *padtemplate;
	GstElementClass *class;

	class = GST_ELEMENT_GET_CLASS(element);

	if (!class->numpadtemplates) {
		return;
	}

	pads = class->padtemplates;
	while (pads) {
		padtemplate = (GstPadTemplate *) (pads->data);
		//GstPad *pad = (GstPad *) (pads->data);
		pads = g_list_next(pads);

		if ((padtemplate->direction == GST_PAD_SINK) &&
		    ((padtemplate->presence == GST_PAD_ALWAYS) ||
		     (padtemplate->presence == GST_PAD_SOMETIMES) ||
		     (padtemplate->presence == GST_PAD_REQUEST)) &&
		    (padtemplate->caps)) {
			scan_caps(padtemplate->caps);
		}
	}

}

static void scan_mime_list(void)
{
	GstRegistry *registry = NULL;
	GList *plugins = NULL;

#if (GST_VERSION_MAJOR < 1)
	registry = gst_registry_get_default();
	plugins = gst_default_registry_get_plugin_list();
#else
	registry = gst_registry_get();
	plugins = gst_registry_get_plugin_list(registry);
#endif

	while (plugins) {
		GList *features;
		GstPlugin *plugin;

		plugin = (GstPlugin *) (plugins->data);
		plugins = g_list_next(plugins);

		features =
			gst_registry_get_feature_list_by_plugin(registry,
							    gst_plugin_get_name
							    (plugin));

		while (features) {
			GstPluginFeature *feature;

			feature = GST_PLUGIN_FEATURE(features->data);

			if (GST_IS_ELEMENT_FACTORY(feature)) {
				GstElementFactory *factory;
				GstElement *element;
				factory = GST_ELEMENT_FACTORY(feature);
				element =
				    gst_element_factory_create(factory, NULL);
				if (element) {
					scan_pad_templates_info(element,
								factory);
				}
			}

			features = g_list_next(features);
		}
	}
}

GstElement *player_ = NULL;
GstElement *player2_ = NULL;
static char *gsuri_ = NULL;         // locally strdup()ed
static char *gs_next_uri_ = NULL;   // locally strdup()ed
static struct SongMetaData song_meta_;

static output_transition_cb_t play_trans_callback_ = NULL;
static output_update_meta_cb_t meta_update_callback_ = NULL;

struct track_time_info {
	gint64 duration;
	gint64 position;
};
static struct track_time_info last_known_time_ = {0, 0};

static GstState get_current_player_state() {
	GstState state = GST_STATE_PLAYING;
	GstState pending = GST_STATE_NULL;
	gst_element_get_state(player_, &state, &pending, 0);
	return state;
}

static void output_gstreamer_set_next_uri(const char *uri) {
	Log_info("gstreamer", "Set next uri to '%s'", uri);
	free(gs_next_uri_);
	gs_next_uri_ = (uri && *uri) ? strdup(uri) : NULL;
	app_state.is_play_list = FALSE;
}

static void output_gstreamer_set_uri(const char *uri,
				     output_update_meta_cb_t meta_cb) {
	Log_info("gstreamer", "Set uri to '%s'", uri);
	free(gsuri_);
	gsuri_ = (uri && *uri) ? strdup(uri) : NULL;
	app_state.is_play_list = FALSE;
	meta_update_callback_ = meta_cb;
	SongMetaData_clear(&song_meta_);
}

static void free_media_info (gpointer data)
{
	//MediaInfo *p_media_info = (MediaInfo *)data;
	//g_print ("%s: %s %s\n", __func__, p_media_info->title, p_media_info->uri);
	g_free(data);
}

static void playlist_entry_parsed (TotemPlParser *parser, const gchar *uri, GHashTable *metadata, gpointer user_data)
{
	gchar *title = g_hash_table_lookup (metadata, TOTEM_PL_PARSER_FIELD_TITLE);
	gchar *uri0;
	MediaInfo *p_media_info = (MediaInfo*)g_malloc0(sizeof(MediaInfo));
	if (gst_uri_is_valid (uri))
	{
		uri0 = g_strdup (uri);
	}
	else
	{
		uri0 = gst_filename_to_uri (uri, NULL);
	}
	g_strlcpy(p_media_info->uri, uri0, MAX_PATH);
	if (title != NULL)
	{
		//g_print ("Entry title: %s\n%s\n", title,uri0);
		Log_error("gstreamer", "%s: Entry title: %s", __func__, title);
		g_strlcpy(p_media_info->title, title, MAX_TITLE_LENGTH);
	}
	else
	{
		//g_print ("Entry (URI: %s) has no title.\n", uri0);
		Log_error("gstreamer", "%s: Entry (URI: %s) has no title.", __func__, uri0);
	}
	play_list_info.play_list = g_slist_append(play_list_info.play_list, p_media_info);
//	p_media_info; we will free it next parsed time;
	free(uri0);
}

static int load_playlist(const char* file_name)
{
	gchar *uri;
	gint ret = 0;
	if (play_list_info.play_list != NULL){
		g_slist_free_full(play_list_info.play_list, free_media_info);
		play_list_info.play_list = NULL;
		play_list_info.current_idx = 0;
		//memset(&play_list_info, 0, sizeof(play_list_info));
	}
	if (gst_uri_is_valid (file_name)){
		uri = g_strdup (file_name);
	}
	else{
		uri = gst_filename_to_uri (file_name, NULL);
	}
	//g_print ("%s: %s\n", __func__, uri);
	TotemPlParser *pl = totem_pl_parser_new ();
	g_object_set (pl, "recurse", FALSE, "disable-unsafe", TRUE, NULL);
	//g_signal_connect (G_OBJECT (pl), "playlist-started", G_CALLBACK (playlist_started), NULL);
	g_signal_connect (G_OBJECT (pl), "entry-parsed", G_CALLBACK (playlist_entry_parsed), NULL);
	TotemPlParserResult res = totem_pl_parser_parse (pl, uri, FALSE);
	if ( res != TOTEM_PL_PARSER_RESULT_SUCCESS)
	{
		//g_print ("Playlist parsing failed.\n");
		Log_error("gstreamer", "%s: Playlist parsing failed.%x", __func__, res);
		ret = -1;
	}
	//g_print ("Playlist parsing finished.\n");
	Log_error("gstreamer", "%s: Playlist parsing finished.", __func__);
	free(uri);
	g_object_unref (pl);
	return ret;
}

static int load_playlist_file(const char* file_name)
{
	gchar *p = g_strrstr(file_name, ".");   //get suffix
	gint ret;
	if (g_strstr_len(play_list_suffix, sizeof(play_list_suffix), p) != NULL)
	{
		app_state.is_play_list = TRUE;
		ret = load_playlist(file_name);
	}
	else
	{
		app_state.is_play_list = FALSE;
		g_print ("Invalid playlist file.\n");
		ret = -1;
	}
	return ret;
}

gint get_next_current_idx(void)
{
	gint play_list_num = g_slist_length(play_list_info.play_list);
	gint next_current_idx = (play_list_info.current_idx + 1) % play_list_num;
	g_print ("%s: %d\n", __func__, next_current_idx);
	return next_current_idx;
}

static int output_gstreamer_set_playlist(const char *uri) 
{
	gint ret;
	ret = load_playlist_file(uri);
	if(ret)
		return -1;
	MediaInfo *p_media_info = g_slist_nth_data(play_list_info.play_list, play_list_info.current_idx);
	if (p_media_info)
	{
		char * uri0 = p_media_info->uri;
		free(gsuri_);
		gsuri_ = (uri0 && *uri0) ? strdup(uri0) : NULL;
		ret = 0;
	}else
	{
		ret = -1;
	}
	meta_update_callback_ = NULL;
	return ret;
}

static int output_gstreamer_play(output_transition_cb_t callback) {
	play_trans_callback_ = callback;
	if (get_current_player_state() != GST_STATE_PAUSED) {
		if (gst_element_set_state(player_, GST_STATE_READY) ==
		    GST_STATE_CHANGE_FAILURE) {
			Log_error("gstreamer", "setting play state failed (1)");
			// Error, but continue; can't get worse :)
		}
		if (g_device_play_mode == DEVICE_PLAY_MODE_MASTER)
		{
#if (TRANS_TYPE==TRANS_TYPE_RTP)
			g_object_set(G_OBJECT(gst_data.source), "uri", gsuri_, NULL);
#else
			g_object_set(G_OBJECT(gst_data.source), "location", gsuri_, NULL);
#endif
			Log_info("gstreamer", "setting play state location");
		}
		else
		{
			g_object_set(G_OBJECT(player_), "uri", gsuri_, NULL);
		}
	}
	if (gst_element_set_state(player_, GST_STATE_PLAYING) ==
	    GST_STATE_CHANGE_FAILURE) {
		Log_error("gstreamer", "setting play state failed (2)");
		return -1;
	}
	return 0;
}

static int output_gstreamer_stop(void) {
	if (gst_element_set_state(player_, GST_STATE_READY) ==
	    GST_STATE_CHANGE_FAILURE) {
		return -1;
	} else {
		return 0;
	}
}

static int output_gstreamer_pause(void) {
	if (gst_element_set_state(player_, GST_STATE_PAUSED) ==
	    GST_STATE_CHANGE_FAILURE) {
		return -1;
	} else {
		return 0;
	}
}

static int output_gstreamer_seek(gint64 position_nanos) {
	if (gst_element_seek(player_, 1.0, GST_FORMAT_TIME,
			     GST_SEEK_FLAG_FLUSH,
			     GST_SEEK_TYPE_SET, position_nanos,
			     GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
		return -1;
	} else {
		return 0;
	}
}

#if 0
static const char *gststate_get_name(GstState state)
{
	switch(state) {
	case GST_STATE_VOID_PENDING:
		return "VOID_PENDING";
	case GST_STATE_NULL:
		return "NULL";
	case GST_STATE_READY:
		return "READY";
	case GST_STATE_PAUSED:
		return "PAUSED";
	case GST_STATE_PLAYING:
		return "PLAYING";
	default:
		return "Unknown";
	}
}
#endif

// This is crazy. I want C++ :)
struct MetaModify {
	struct SongMetaData *meta;
	int any_change;
};

static void MetaModify_add_tag(const GstTagList *list, const gchar *tag,
			       gpointer user_data) {
	struct MetaModify *data = (struct MetaModify*) user_data;
	const char **destination = NULL;
	if (strcmp(tag, GST_TAG_TITLE) == 0) {
		destination = &data->meta->title;
	} else if (strcmp(tag, GST_TAG_ARTIST) == 0) {
		destination = &data->meta->artist;
	} else if (strcmp(tag, GST_TAG_ALBUM) == 0) {
		destination = &data->meta->album;
	} else if (strcmp(tag, GST_TAG_GENRE) == 0) {
		destination = &data->meta->genre;
	} else if (strcmp(tag, GST_TAG_COMPOSER) == 0) {
		destination = &data->meta->composer;
	}
	if (destination != NULL) {
		char *replace = NULL;
		gst_tag_list_get_string(list, tag, &replace);
		if (replace != NULL &&
		    (*destination == NULL
		     || strcmp(replace, *destination) != 0)) {
			free((char*)*destination);
			*destination = replace;
			data->any_change++;
		} else {
			free(replace);
		}
	}
}

gboolean my_bus_callback(GstBus * bus, GstMessage * msg, gpointer data)
{
	//GMainLoop *loop = (GMainLoop *) data;
	GstMessageType msgType;
	const GstObject *msgSrc;
	const gchar *msgSrcName;
	gboolean play_flag = FALSE;

	msgType = GST_MESSAGE_TYPE(msg);
	msgSrc = GST_MESSAGE_SRC(msg);
	msgSrcName = GST_OBJECT_NAME(msgSrc);

	switch (msgType) {
	case GST_MESSAGE_EOS:
		Log_info("gstreamer", "%s: End-of-stream", msgSrcName);
		if (g_device_play_mode == DEVICE_PLAY_MODE_SLAVE)
		{
			gst_element_set_state (player_, GST_STATE_NULL);
			gst_element_set_state (player_, GST_STATE_PLAYING);
		}
		else
		{
			if (app_state.is_play_list)
			{
				free(gsuri_);
				play_list_info.current_idx = get_next_current_idx();
				MediaInfo *p_media_info = g_slist_nth_data(play_list_info.play_list, play_list_info.current_idx);
				char * uri0 = p_media_info->uri;
				gsuri_ = (uri0 && *uri0) ? strdup(uri0) : NULL;
				play_flag = TRUE;
			}
			else
			{
				if (gs_next_uri_ != NULL) {
					// If playbin does not support gapless (old
					// versions didn't), this will trigger.
					free(gsuri_);
					gsuri_ = gs_next_uri_;
					gs_next_uri_ = NULL;
					play_flag = TRUE;
				} else if (play_trans_callback_) {
					play_trans_callback_(PLAY_STOPPED);
				}
			}
		
			if (play_flag)
			{
				gst_element_set_state(player_, GST_STATE_READY);
				if (g_device_play_mode == DEVICE_PLAY_MODE_MASTER)
				{
#if (TRANS_TYPE==TRANS_TYPE_RTP)
					g_object_set(G_OBJECT(gst_data.source), "uri", gsuri_, NULL);
#else
					g_object_set(G_OBJECT(gst_data.source), "location", gsuri_, NULL);
#endif
				}
				else
				{
					g_object_set(G_OBJECT(player_), "uri", gsuri_, NULL);
				}
				gst_element_set_state(player_, GST_STATE_PLAYING);
				if (play_trans_callback_) {
					play_trans_callback_(PLAY_STARTED_NEXT_STREAM);
				}
			}
		}
		break;

	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *err;

		gst_message_parse_error(msg, &err, &debug);

		Log_error("gstreamer", "%s: Error: %s (Debug: %s)",
			  msgSrcName, err->message, debug);
		g_error_free(err);
		g_free(debug);

		break;
	}
	case GST_MESSAGE_WARNING: {
		gchar *debug;
		GError *err;

		gst_message_parse_warning(msg, &err, &debug);

		Log_error("gstreamer", "%s: Error: %s (Debug: %s)",
			  msgSrcName, err->message, debug);
		g_error_free(err);
		g_free(debug);

		break;
	}
	case GST_MESSAGE_STATE_CHANGED: {
		GstState oldstate, newstate, pending;
		gst_message_parse_state_changed(msg, &oldstate, &newstate,
						&pending);
		/*
		g_print("GStreamer: %s: State change: '%s' -> '%s', "
			"PENDING: '%s'\n", msgSrcName,
			gststate_get_name(oldstate),
			gststate_get_name(newstate),
			gststate_get_name(pending));
		*/
		break;
	}

	case GST_MESSAGE_TAG: {
		GstTagList *tags = NULL;
    
		if (meta_update_callback_ != NULL) {
			gst_message_parse_tag(msg, &tags);
			/*g_print("GStreamer: Got tags from element %s\n",
				GST_OBJECT_NAME (msg->src));
			*/
			struct MetaModify modify;
			modify.meta = &song_meta_;
			modify.any_change = 0;
			gst_tag_list_foreach(tags, &MetaModify_add_tag, &modify);
			gst_tag_list_free(tags);
			if (modify.any_change) {
				meta_update_callback_(&song_meta_);
			}
		}
		break;
	}

	case GST_MESSAGE_BUFFERING:
		/* not caring about these right now */
		break;
	default:
		/*
		g_print("GStreamer: %s: unhandled message type %d (%s)\n",
		        msgSrcName, msgType, gst_message_type_get_name(msgType));
		*/
		break;
	}

	return TRUE;
}

gboolean my_bus_callback2(GstBus * bus, GstMessage * msg, gpointer data)
{
	GstMessageType msgType;
	const GstObject *msgSrc;
	const gchar *msgSrcName;

	msgType = GST_MESSAGE_TYPE(msg);
	msgSrc = GST_MESSAGE_SRC(msg);
	msgSrcName = GST_OBJECT_NAME(msgSrc);

	switch (msgType) {
	case GST_MESSAGE_EOS: {
		Log_info("gstreamer", "%s: End-of-stream", msgSrcName);
		
	}
		break;

	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *err;

		gst_message_parse_error(msg, &err, &debug);

		Log_error("gstreamer", "%s: Error: %s (Debug: %s)",
			  msgSrcName, err->message, debug);
		g_error_free(err);
		g_free(debug);

		break;
	}
	case GST_MESSAGE_WARNING: {
		gchar *debug;
		GError *err;

		gst_message_parse_warning(msg, &err, &debug);

		Log_error("gstreamer", "%s: Error: %s (Debug: %s)",
			  msgSrcName, err->message, debug);
		g_error_free(err);
		g_free(debug);

		break;
	}
	default:
		/*
		g_print("GStreamer: %s: unhandled message type %d (%s)\n",
		        msgSrcName, msgType, gst_message_type_get_name(msgType));
		*/
		break;
	}

	return TRUE;
}

gchar *audio_sink = NULL;
gchar *audio_device = NULL;
gchar *videosink = NULL;
static double initial_db = 0.0;

/* Options specific to output_gstreamer */
static GOptionEntry option_entries[] = {
        { "gstout-audiosink", 0, 0, G_OPTION_ARG_STRING, &audio_sink,
          "GStreamer audio sink to use "
	  "(autoaudiosink, alsasink, osssink, esdsink, ...)",
	  NULL },
        { "gstout-audiodevice", 0, 0, G_OPTION_ARG_STRING, &audio_device,
          "GStreamer device for the given audiosink. ",
	  NULL },
        { "gstout-videosink", 0, 0, G_OPTION_ARG_STRING, &videosink,
          "GStreamer video sink to use "
	  "(autovideosink, xvimagesink, ximagesink, ...)",
	  NULL },
        { "gstout-initial-volume-db", 0, 0, G_OPTION_ARG_DOUBLE, &initial_db,
          "GStreamer inital volume in decibel (e.g. 0.0 = max; -6 = 1/2 max) ",
	  NULL },
        { NULL }
};

static int output_gstreamer_add_options(GOptionContext *ctx)
{
	GOptionGroup *option_group;
	option_group = g_option_group_new("gstout", "GStreamer Output Options",
	                                  "Show GStreamer Output Options",
	                                  NULL, NULL);
	g_option_group_add_entries(option_group, option_entries);

	g_option_context_add_group (ctx, option_group);
	
	g_option_context_add_group (ctx, gst_init_get_option_group ());
	return 0;
}

static int output_gstreamer_get_position(gint64 *track_duration,
					 gint64 *track_pos) {
	*track_duration = last_known_time_.duration;
	*track_pos = last_known_time_.position;

	int rc = 0;
	if (get_current_player_state() != GST_STATE_PLAYING) {
		return rc;  // playbin2 only returns valid values then.
	}
#if (GST_VERSION_MAJOR < 1)
	GstFormat fmt = GST_FORMAT_TIME;
	GstFormat* query_type = &fmt;
#else
	GstFormat query_type = GST_FORMAT_TIME;
#endif	
	if (!gst_element_query_duration(player_, query_type, track_duration)) {
		Log_error("gstreamer", "Failed to get track duration.");
		rc = -1;
	}
	if (!gst_element_query_position(player_, query_type, track_pos)) {
		Log_error("gstreamer", "Failed to get track pos");
		rc = -1;
	}
	// playbin2 does not allow to query while paused. Remember in case
	// we're asked then (it actually returns something, but it is bogus).
	last_known_time_.duration = *track_duration;
	last_known_time_.position = *track_pos;
	return rc;
}

static int output_gstreamer_get_volume(float *v) {
	double volume;
	g_object_get(player_, "volume", &volume, NULL);
	Log_info("gstreamer", "Query volume fraction: %f", volume);
	*v = volume;
	return 0;
}
static int output_gstreamer_set_volume(float value) {
	Log_info("gstreamer", "Set volume fraction to %f", value);
	g_object_set(player_, "volume", (double) value, NULL);
	return 0;
}

static int output_gstreamer_get_mute(int *m) {
	gboolean val;
	g_object_get(player_, "mute", &val, NULL);
	*m = val;
	return 0;
}
static int output_gstreamer_set_mute(int m) {
	Log_info("gstreamer", "Set mute to %s", m ? "on" : "off");
	g_object_set(player_, "mute", (gboolean) m, NULL);
	return 0;
}

static void prepare_next_stream(GstElement *obj, gpointer userdata) {
	Log_info("gstreamer", "about-to-finish cb: setting uri %s",
		 gs_next_uri_);
	free(gsuri_);
	if (app_state.is_play_list)
	{
		play_list_info.current_idx = get_next_current_idx();
		MediaInfo *p_media_info = g_slist_nth_data(play_list_info.play_list, play_list_info.current_idx);
		char * uri0 = p_media_info->uri;
		gsuri_ = (uri0 && *uri0) ? strdup(uri0) : NULL;
	}
	else
	{
		gsuri_ = gs_next_uri_;
		gs_next_uri_ = NULL;
	}

	if (gsuri_ != NULL) {
		if (g_device_play_mode == DEVICE_PLAY_MODE_MASTER)
		{
#if (TRANS_TYPE==TRANS_TYPE_RTP)
			g_object_set(G_OBJECT(gst_data.source), "uri", gsuri_, NULL);
#else
			g_object_set(G_OBJECT(gst_data.source), "location", gsuri_, NULL);
#endif
		}
		else
		{
			g_object_set(G_OBJECT(player_), "uri", gsuri_, NULL);
		}

		if (play_trans_callback_) {
			// TODO(hzeller): can we figure out when we _actually_
			// start playing this ? there are probably a couple
			// of seconds between now and actual start.
			play_trans_callback_(PLAY_STARTED_NEXT_STREAM);
		}
	}
}

/*

static struct xmlelement *gen_group_info(struct xmldoc *doc,
                                          const char* groupid, const char* grouprole)
{
        struct xmlelement *top;

        top=xmlelement_new(doc, "Group");

        add_value_element(doc, top, "GroupID", groupid);
        add_value_element(doc, top, "GroupRole", grouprole);

        return top;
}

static struct xmldoc *generate_gmediarender(void)
{
	struct xmldoc *doc;
	struct xmlelement *root;
	struct xmlelement *child;

	doc = xmldoc_new();

	root=xmldoc_new_topelement(doc, "Gmediarender", "gmediarender-1-0");
	child=gen_group_info(doc, "groupid", "grouprole");
	xmlelement_add_element(doc, root, child);
	fprintf(stdout, "\n%s\n", xmldoc_tostring(doc));
	return doc;
}
*/



static char *output_gstreamer_get_groupid(void)
{
	char *groupid = NULL;
	struct xmldoc *doc = xmldoc_fromdoc(RENDERXML);
	if(doc){
		struct xmlelement *render_node = find_element_in_doc(doc, "Gmediarender");
		struct xmlelement *group_node = find_element_in_element(render_node, "Group");
		struct xmlelement *value_node = find_element_in_element(group_node, "GroupID");
		groupid = get_node_value(value_node);
		xmldoc_free(doc);
	}
	return groupid;
}
static int output_gstreamer_set_groupid(const char*groupid)
{
	int ret = -1;
	struct xmldoc *doc = xmldoc_fromdoc(RENDERXML);
	char *xmlstring;
	if(doc){
		struct xmlelement *render_node = find_element_in_doc(doc, "Gmediarender");
		struct xmlelement *group_node = find_element_in_element(render_node, "Group");
		struct xmlelement *value_node = find_element_in_element(group_node, "GroupID");
		if(value_node)
			ret = set_node_value(value_node, groupid);
		xmlstring = xmldoc_tostring(doc);
		xmlstringtofile(RENDERXML, xmlstring);
		xmldoc_free(doc);
		free(xmlstring);
	}

	return ret;
}

static char * output_gstreamer_get_grouprole(void)
{
	char *grouprole = NULL;
	struct xmldoc *doc = xmldoc_fromdoc(RENDERXML);
	if(doc){
		struct xmlelement *render_node = find_element_in_doc(doc, "Gmediarender");
		struct xmlelement *group_node = find_element_in_element(render_node, "Group");
		struct xmlelement *value_node = find_element_in_element(group_node, "GroupRole");
		grouprole = get_node_value(value_node);
		xmldoc_free(doc);
	}
	return grouprole;
}
static int output_gstreamer_set_grouprole(const char*grouprole)
{
	int ret = -1;
	struct xmldoc *doc = xmldoc_fromdoc(RENDERXML);
	char *xmlstring;
	if(doc){
		struct xmlelement *render_node = find_element_in_doc(doc, "Gmediarender");
		struct xmlelement *group_node = find_element_in_element(render_node, "Group");
		struct xmlelement *value_node = find_element_in_element(group_node, "GroupRole");
		if(value_node)
			ret = set_node_value(value_node, grouprole);
		xmlstring = xmldoc_tostring(doc);
		xmlstringtofile(RENDERXML, xmlstring);
		xmldoc_free(doc);
		free(xmlstring);
	}
	return ret;

}

static char * output_gstreamer_get_devicetype(void)
{
	char *grouprole = NULL;
	struct xmldoc *doc = xmldoc_fromdoc(RENDERXML);
	if(doc){
		struct xmlelement *render_node = find_element_in_doc(doc, "Gmediarender");
		struct xmlelement *group_node = find_element_in_element(render_node, "Group");
		struct xmlelement *value_node = find_element_in_element(group_node, "DeviceType");
		grouprole = get_node_value(value_node);
		xmldoc_free(doc);
	}
	return grouprole;
}

static int output_gstreamer_get_device_role()
{
	char *p;
	int ret = -1;
	p = output_gstreamer_get_grouprole();
	if(p){
		if(!strcasecmp (p, SINGLE))
			ret = DEVICE_PLAY_MODE_SINGLE;
		else if(!strcasecmp (p, MASTER))
			ret = DEVICE_PLAY_MODE_MASTER;
		else if(!strcasecmp (p, SLAVE))
			ret = DEVICE_PLAY_MODE_SLAVE;
	}else{
		
		ret = DEVICE_PLAY_MODE_SINGLE;
	}
	free(p);
	return ret;
}

static int output_gstreamer_init_single(void)
{
	GstBus *bus;

#if (GST_VERSION_MAJOR < 1)
	const char player_element_name[] = "playbin2";
#else
	const char player_element_name[] = "playbin";
#endif

	player_ = gst_element_factory_make(player_element_name, "play");
	assert(player_ != NULL);

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

	g_signal_connect(G_OBJECT(player_), "about-to-finish",
			 G_CALLBACK(prepare_next_stream), NULL);
	output_gstreamer_set_mute(0);
	if (initial_db < 0) {
		output_gstreamer_set_volume(exp(initial_db / 20 * log(10)));
	}

	return 0;
}

static int output_gstreamer_init(void)
{
	SongMetaData_init(&song_meta_);
	scan_mime_list();
//	generate_gmediarender();
	g_device_play_mode = output_gstreamer_get_device_role();

	fprintf(stdout, "device mode = %d\n", g_device_play_mode);
	
	if (g_device_play_mode == DEVICE_PLAY_MODE_MASTER)
	{
		//output_gstreamer_init_master();
		output_gstreamer_init_master2();
		output_gstreamer_init_master3();
	}
	else if (g_device_play_mode == DEVICE_PLAY_MODE_SLAVE)
	{
		output_gstreamer_init_slave();
	}
	else
	{
		output_gstreamer_init_single();
	}

	return 0;
}

struct output_module gstreamer_output = {
        .shortname = "gst",
	.description = "GStreamer multimedia framework",
	.init        = output_gstreamer_init,
	.add_options = output_gstreamer_add_options,
	.set_uri     = output_gstreamer_set_uri,
	.set_next_uri= output_gstreamer_set_next_uri,
	.set_playlist= output_gstreamer_set_playlist,
	.play        = output_gstreamer_play,
	.stop        = output_gstreamer_stop,
	.pause       = output_gstreamer_pause,
	.seek        = output_gstreamer_seek,
	.get_position = output_gstreamer_get_position,
	.get_volume  = output_gstreamer_get_volume,
	.set_volume  = output_gstreamer_set_volume,
	.get_mute  = output_gstreamer_get_mute,
	.set_mute  = output_gstreamer_set_mute,
	.get_groupid = output_gstreamer_get_groupid,
	.set_groupid = output_gstreamer_set_groupid,
	.get_grouprole = output_gstreamer_get_grouprole,
	.set_grouprole = output_gstreamer_set_grouprole,
	.get_devicetype = output_gstreamer_get_devicetype,
};
