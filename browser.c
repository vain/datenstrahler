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
    GtkWidget *scroll;
    GtkWidget *top_box;
    GtkWidget *vbox;
    GtkWidget *web_view;
    GtkWidget *win;
};

struct DownloadManager
{
    GtkWidget *scroll;
    GtkWidget *toolbar;
    GtkWidget *win;
} dm;


#include "config.h"


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
    gtk_window_set_wmclass(GTK_WINDOW(c->win), __NAME__, __NAME_CAPITALIZED__);
    gtk_window_set_default_size(GTK_WINDOW(c->win), 800, 600);
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

    c->scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(c->scroll), c->web_view);
    gtk_container_add(GTK_CONTAINER(c->win), c->scroll);
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
    if (argc > 1)
    {
        gtk_init(&argc, &argv);
        client_new(argv[1]);
        gtk_main();
    }

    exit(EXIT_SUCCESS);
}
