/* This must be a NULL terminated array of accepted languages. NULL
 * means use WebKit's default value. */
static const gchar *accepted_language[] = { NULL };

/* If WebKit's rendering process crashes or loading of a web site
 * failed, a reload will be triggered after a short delay.
 * Note: This does not apply to iframes which failed to load. */
static int autoreload_delay = 2;

/* Go fullscreen by default? */
static gboolean fullscreen = FALSE;

/* Default geometry to use when starting up? */
static gchar *geometry = "800x600";

/* Standard zoom level. */
static gdouble global_zoom = 1.0;

/* NULL means use WebKit's default value. */
static gchar *user_agent = NULL;
