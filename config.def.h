/* This must be a NULL terminated array of accepted languages. NULL
 * means use WebKit's default value. */
static const gchar *accepted_language[] = { NULL };

/* If WebKit's rendering process crashes or loading of a web site
 * failed, a reload will be triggered after a short delay.
 * Note: This does not apply to iframes which failed to load. */
static int autoreload_delay = 2;

/* Whether to ignore cookies. Note that datenstrahler never stores
 * cookies on disk anyway. */
static gboolean dont_accept_cookies = FALSE;

/* Go fullscreen by default? */
static gboolean fullscreen = FALSE;

/* Default geometry to use when starting up? */
static gchar *geometry = "800x600";

/* Standard zoom level. */
static gdouble global_zoom = 1.0;

/* Automatically reload the URL after this many seconds. Disabled if 0. */
static int reload_interval = 0;

/* NULL means use WebKit's default value. */
static gchar *user_agent = NULL;

/* Use a "pseudo desktop window"? Internally, this sets the
 * "override_redirect" flag, causing the window manager to ignore the
 * X11 window. It also tries to move the window to the bottom of the
 * stack. */
static gboolean pseudo_desktop_window = FALSE;
