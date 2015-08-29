/* This must be a NULL terminated array of accepted languages. NULL
 * means use WebKit's default value. */
static const gchar *accepted_language[] = { NULL };

/* If WebKit's rendering process crashes, the website will be reloaded
 * after a short delay. */
static int crash_autoreload_delay = 2;

/* Standard zoom level. */
static gdouble global_zoom = 1.0;

/* NULL means use WebKit's default value. */
static gchar *user_agent = NULL;
