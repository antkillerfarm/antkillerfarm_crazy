#include <gtk/gtk.h>
#include <webkit/webkit.h>

static GtkWidget* main_window;
static GtkWidget* uri_entry;
static GtkStatusbar* main_statusbar;
static WebKitWebView* web_view;
static gchar* main_title;
static gint load_progress;
static guint status_context_id;

static gboolean
show_web_view_cb (WebKitWebView* web_view)
{
    GtkWidget* window = gtk_widget_get_toplevel(GTK_WIDGET(web_view));
    gtk_widget_show_all(window);
    g_print("show_web_view_cb\r\n");
    return TRUE;
}

static WebKitWebView*
create_web_view_cb (WebKitWebView* web_view, WebKitWebFrame* web_frame)
{
    GtkWidget* new_window;
    GtkWidget* scrolled_window;
    GtkWidget* new_web_view = webkit_web_view_new();
    g_print("create_web_view_cb\r\n");

    g_signal_connect (G_OBJECT (new_web_view), "web-view-ready", G_CALLBACK (show_web_view_cb), NULL);

    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (new_window), 800, 600);
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolled_window);
    gtk_container_add(GTK_CONTAINER(new_window), scrolled_window);
    gtk_container_add(GTK_CONTAINER(scrolled_window), new_web_view);

    return WEBKIT_WEB_VIEW(new_web_view);
}

static void
activate_uri_entry_cb (GtkWidget* entry, gpointer data)
{
    const gchar* uri = gtk_entry_get_text (GTK_ENTRY (entry));
    g_assert (uri);
    webkit_web_view_open (web_view, uri);
    g_print("activate_uri_entry_cb\r\n");
}

static void
update_title (GtkWindow* window)
{
    GString* string = g_string_new (main_title);
    g_string_append (string, " - WebKit Launcher");
    if (load_progress < 100)
        g_string_append_printf (string, " (%d%%)", load_progress);
    gchar* title = g_string_free (string, FALSE);
    gtk_window_set_title (window, title);
    g_free (title);
}

static void
link_hover_cb (WebKitWebView* page, const gchar* title, const gchar* link, gpointer data)
{
    /* underflow is allowed */
    gtk_statusbar_pop (main_statusbar, status_context_id);
    if (link)
        gtk_statusbar_push (main_statusbar, status_context_id, link);
}

static void
title_change_cb (WebKitWebView* web_view, WebKitWebFrame* web_frame, const gchar* title, gpointer data)
{
    if (main_title)
        g_free (main_title);
    main_title = g_strdup (title);
    update_title (GTK_WINDOW (main_window));
}

static void
progress_change_cb (WebKitWebView* page, gint progress, gpointer data)
{
    load_progress = progress;
    update_title (GTK_WINDOW (main_window));
}

static void
load_commit_cb (WebKitWebView* page, WebKitWebFrame* frame, gpointer data)
{
    const gchar* uri = webkit_web_frame_get_uri(frame);
    if (uri)
        gtk_entry_set_text (GTK_ENTRY (uri_entry), uri);
}

static void
destroy_cb (GtkWidget* widget, gpointer data)
{
    gtk_main_quit ();
}

static void
go_back_cb (GtkWidget* widget, gpointer data)
{
    webkit_web_view_go_back (web_view);
}

static void
go_forward_cb (GtkWidget* widget, gpointer data)
{
    webkit_web_view_go_forward (web_view);
}

static GtkWidget*
create_browser ()
{
    GtkWidget* scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    web_view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
    gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (web_view));

    g_signal_connect (G_OBJECT (web_view), "create-web-view", G_CALLBACK (create_web_view_cb), NULL);
    g_signal_connect (G_OBJECT (web_view), "title-changed", G_CALLBACK (title_change_cb), web_view);
    g_signal_connect (G_OBJECT (web_view), "load-progress-changed", G_CALLBACK (progress_change_cb), web_view);
    g_signal_connect (G_OBJECT (web_view), "load-committed", G_CALLBACK (load_commit_cb), web_view);
    g_signal_connect (G_OBJECT (web_view), "hovering-over-link", G_CALLBACK (link_hover_cb), web_view);

    return scrolled_window;
}

static GtkWidget*
create_statusbar ()
{
    main_statusbar = GTK_STATUSBAR (gtk_statusbar_new ());
    status_context_id = gtk_statusbar_get_context_id (main_statusbar, "Link Hover");

    return (GtkWidget*)main_statusbar;
}

static GtkWidget*
create_toolbar ()
{
    GtkWidget* toolbar = gtk_toolbar_new ();

    gtk_orientable_set_orientation (GTK_ORIENTABLE (toolbar), GTK_ORIENTATION_HORIZONTAL);
    gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH_HORIZ);

    GtkToolItem* item;

    /* the back button */
    item = gtk_tool_button_new(NULL, NULL);
    //item = gtk_tool_button_new_from_stock (GTK_STOCK_GO_BACK);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON (item), "gtk-go-back-ltr");
    g_signal_connect (G_OBJECT (item), "clicked", G_CALLBACK (go_back_cb), NULL);
    gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

    /* The forward button */
    item = gtk_tool_button_new(NULL, NULL);
    //item = gtk_tool_button_new_from_stock (GTK_STOCK_GO_FORWARD);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON (item), "gtk-go-forward-ltr");
    g_signal_connect (G_OBJECT (item), "clicked", G_CALLBACK (go_forward_cb), NULL);
    gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

    /* The URL entry */
    item = gtk_tool_item_new ();
    gtk_tool_item_set_expand (item, TRUE);
    uri_entry = gtk_entry_new ();
    gtk_container_add (GTK_CONTAINER (item), uri_entry);
    g_signal_connect (G_OBJECT (uri_entry), "activate", G_CALLBACK (activate_uri_entry_cb), NULL);
    gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

    /* The go button */
    item = gtk_tool_button_new(NULL, NULL);
    //item = gtk_tool_button_new_from_stock (GTK_STOCK_OK);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON (item), "gtk-ok");
    g_signal_connect_swapped (G_OBJECT (item), "clicked", G_CALLBACK (activate_uri_entry_cb), (gpointer)uri_entry);
    gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

    return toolbar;
}

static GtkWidget*
create_window ()
{
    GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
    gtk_widget_set_name (window, "GtkLauncher");
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_cb), NULL);

    return window;
}

int
main (int argc, char* argv[])
{
    gtk_init (&argc, &argv);

    GtkWidget* vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox), create_toolbar (), FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), create_browser (), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), create_statusbar (), FALSE, FALSE, 0);

    main_window = create_window ();
    gtk_container_add (GTK_CONTAINER (main_window), vbox);

    gchar* uri = (gchar*) (argc > 1 ? argv[1] : "http://news.sohu.com/");
    webkit_web_view_open (web_view, uri);

    gtk_widget_grab_focus (GTK_WIDGET (web_view));
    gtk_widget_show_all (main_window);
    gtk_main ();

    return 0;
}
