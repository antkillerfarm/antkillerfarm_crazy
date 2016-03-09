#include <gtk/gtk.h>
#include <gst/gst.h>
#include <string.h>
#include <totem-pl-parser.h>

#define MAX_PATH 256
#define MAX_TITLE_LENGTH 128

typedef struct{
  GtkStatusbar *statusbar;
  gint contextId;
}MainWindowSubWidget;

typedef struct{
  GstElement *playbin;
  guint bus_watch_id;
}GstData;

typedef struct{
  gchar title[MAX_TITLE_LENGTH];
}MediaInfo;

char g_filename[MAX_PATH];

MainWindowSubWidget main_window_sub_widget;
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
      g_strlcpy(g_filename, filename, MAX_PATH);
      gtk_statusbar_push(GTK_STATUSBAR(main_window_sub_widget.statusbar), 
		     main_window_sub_widget.contextId, g_filename);
      g_free (filename);
    }

  gtk_widget_destroy (dialog);

}

G_MODULE_EXPORT void do_button_play_clicked(GtkButton *button, gpointer data)
{
  gchar *uri;
  if (gst_uri_is_valid (g_filename))
    {
      uri = g_strdup (g_filename);
    }
  else
    {
      uri = gst_filename_to_uri (g_filename, NULL);
    }
  g_object_set (gst_data.playbin, "uri", uri, NULL);
  g_free (uri);

  gst_element_set_state (gst_data.playbin, GST_STATE_PLAYING);
}

G_MODULE_EXPORT void do_button_test_clicked(GtkButton *button, gpointer data)
{
  gchar *p = g_strrstr(g_filename, ".");
  g_print ("%s: %s\n", __func__, p + 1);
  if (g_strcmp0(p + 1, "m3u") == 0)
    {
      g_print ("%s: OK\n", __func__);
    }
}

static void playlist_entry_parsed (TotemPlParser *parser, const gchar *uri, GHashTable *metadata, gpointer user_data)
{
  gchar *title = g_hash_table_lookup (metadata, TOTEM_PL_PARSER_FIELD_TITLE);
  if (title != NULL)
    g_print ("Entry title: %s\n", title);
  else
    g_print ("Entry (URI: %s) has no title.\n", uri);
}

G_MODULE_EXPORT void do_button_test2_clicked(GtkButton *button, gpointer data)
{
  gchar *uri;
  if (gst_uri_is_valid (g_filename))
    {
      uri = g_strdup (g_filename);
    }
  else
    {
      uri = gst_filename_to_uri (g_filename, NULL);
    }
  g_print ("%s: %s\n", __func__, uri);
  TotemPlParser *pl = totem_pl_parser_new ();
  g_object_set (pl, "recurse", FALSE, "disable-unsafe", TRUE, NULL);
  //g_signal_connect (G_OBJECT (pl), "playlist-started", G_CALLBACK (playlist_started), NULL);
  g_signal_connect (G_OBJECT (pl), "entry-parsed", G_CALLBACK (playlist_entry_parsed), NULL);

  if (totem_pl_parser_parse (pl, uri, FALSE) != TOTEM_PL_PARSER_RESULT_SUCCESS)
    {
      g_print ("Playlist parsing failed.");
    }

  g_print ("Playlist parsing finished.");
  g_object_unref (pl);
}

void ui_init()
{
  GtkWidget *window = NULL;
  GtkBuilder *builder;
  GError *err = NULL;

  gtk_init(NULL, NULL);

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "audio_player.glade", &err);

  gtk_builder_connect_signals(builder, NULL);
  window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

  main_window_sub_widget.statusbar = GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar1"));

  main_window_sub_widget.contextId = gtk_statusbar_get_context_id(
                   GTK_STATUSBAR(main_window_sub_widget.statusbar), "Editor Messages");

  g_signal_connect(window, "destroy",
		   G_CALLBACK (gtk_main_quit), &window);

  g_object_unref(G_OBJECT(builder));

  gtk_widget_show_all(window);
}

void media_init()
{
  GstBus *bus;

  gst_init (NULL, NULL);

  gst_data.playbin = gst_element_factory_make ("playbin", NULL);
  if (!gst_data.playbin) {
    g_print ("'playbin' gstreamer plugin missing\n");
    return;
  }

  bus = gst_element_get_bus (gst_data.playbin);
  gst_data.bus_watch_id = gst_bus_add_watch (bus, bus_call, NULL);
  g_object_unref (bus);
}

void media_cleanup()
{
  gst_element_set_state (gst_data.playbin, GST_STATE_NULL);
  g_object_unref (gst_data.playbin);
  g_source_remove (gst_data.bus_watch_id);
}

gint main (gint argc, gchar * argv[])
{
  ui_init();
  media_init();

  gtk_main();

  media_cleanup();

  return 0;
}
