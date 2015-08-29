#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>


static WebKitWebView *client_new(const gchar *);
static void changed_title(GObject *, GParamSpec *, gpointer);
static gboolean crashed_web_view(WebKitWebView *, gpointer);
static gboolean crashed_web_view_reload(gpointer);
static gchar *ensure_uri_scheme(const gchar *);


struct Client
{
    gchar *hover_uri;
    GtkWidget *location;
    GtkWidget *progress;
    GtkWidget *top_box;
    GtkWidget *vbox;
    GtkWidget *web_view;
    GtkWidget *win;
};


#include "config.h"


static gchar *window_class = __NAME_CAPITALIZED__;
static gchar *window_name = __NAME__;


WebKitWebView *
client_new(const gchar *uri)
{
    struct Client *c;
    WebKitWebContext *wc;
    gchar *f;

    c = malloc(sizeof(struct Client));
    if (!c)
    {
        fprintf(stderr, __NAME__": fatal: malloc failed\n");
        exit(EXIT_FAILURE);
    }

    c->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_wmclass(GTK_WINDOW(c->win), window_name, window_class);
    gtk_window_set_title(GTK_WINDOW(c->win), __NAME__);

    c->web_view = webkit_web_view_new();
    wc = webkit_web_view_get_context(WEBKIT_WEB_VIEW(c->web_view));
    webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(c->web_view), global_zoom);

    g_signal_connect(G_OBJECT(c->win), "destroy", G_CALLBACK(gtk_main_quit), c);
    g_signal_connect(G_OBJECT(c->web_view), "notify::title",
                     G_CALLBACK(changed_title), c);
    g_signal_connect(G_OBJECT(c->web_view), "close",
                     G_CALLBACK(gtk_main_quit), c);
    g_signal_connect(G_OBJECT(c->web_view), "web-process-crashed",
                     G_CALLBACK(crashed_web_view), c);

    if (accepted_language[0] != NULL)
        webkit_web_context_set_preferred_languages(wc, accepted_language);

    if (user_agent != NULL)
        g_object_set(G_OBJECT(webkit_web_view_get_settings(WEBKIT_WEB_VIEW(c->web_view))),
                     "user-agent", user_agent, NULL);

    gtk_container_add(GTK_CONTAINER(c->win), c->web_view);
    gtk_widget_show_all(c->web_view);

    if (fullscreen)
        gtk_window_fullscreen(GTK_WINDOW(c->win));
    else
        gtk_window_parse_geometry(GTK_WINDOW(c->win), geometry);

    /* First, we "showed" the web view and its children. Then we might
     * have applied a geometry. Now it's time to show the window. This
     * follows the example here:
     * https://developer.gnome.org/gtk3/stable/GtkWindow.html#gtk-window-parse-geometry */
    gtk_widget_show_all(c->win);

    f = ensure_uri_scheme(uri);
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(c->web_view), f);
    g_free(f);

    return WEBKIT_WEB_VIEW(c->web_view);
}

void
changed_title(GObject *obj, GParamSpec *pspec, gpointer data)
{
    const gchar *t;
    struct Client *c = (struct Client *)data;

    t = webkit_web_view_get_title(WEBKIT_WEB_VIEW(c->web_view));
    gtk_window_set_title(GTK_WINDOW(c->win), (t == NULL ? __NAME__ : t));
}

gboolean
crashed_web_view(WebKitWebView *web_view, gpointer data)
{
    fprintf(stderr, __NAME__": WebView crashed!\n");
    if (crash_autoreload_delay >= 1)
    {
        fprintf(stderr, __NAME__": Reloading WebView in %d seconds.\n",
                crash_autoreload_delay);
        g_timeout_add_seconds(crash_autoreload_delay, crashed_web_view_reload,
                              web_view);
    }

    return TRUE;
}

gboolean
crashed_web_view_reload(gpointer data)
{
    webkit_web_view_reload_bypass_cache(WEBKIT_WEB_VIEW(data));

    return G_SOURCE_REMOVE;
}

gchar *
ensure_uri_scheme(const gchar *t)
{
    gchar *f;

    f = g_ascii_strdown(t, -1);
    if (!g_str_has_prefix(f, "http:") &&
        !g_str_has_prefix(f, "https:") &&
        !g_str_has_prefix(f, "file:") &&
        !g_str_has_prefix(f, "about:"))
    {
        g_free(f);
        f = g_strdup_printf("http://%s", t);
        return f;
    }
    else
        return g_strdup(t);
}

int
main(int argc, char **argv)
{
    int opt;

    gtk_init(&argc, &argv);

    while ((opt = getopt(argc, argv, "c:fg:i:z:")) != -1)
    {
        switch (opt)
        {
            case 'c':
                window_class = g_strdup(optarg);
                break;
            case 'f':
                fullscreen = TRUE;
                break;
            case 'g':
                geometry = g_strdup(optarg);
                break;
            case 'i':
                window_name = g_strdup(optarg);
                break;
            case 'z':
                global_zoom = atof(optarg);
                break;
            default:
                fprintf(stderr, "Usage: "__NAME__" [OPTION]... [URI]...\n");
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc)
    {
        client_new(argv[optind]);
        gtk_main();
    }
    else
        fprintf(stderr, "URI is required\n");

    exit(EXIT_SUCCESS);
}
