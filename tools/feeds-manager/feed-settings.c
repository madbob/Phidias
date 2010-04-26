#include "feed-settings.h"
#include "marshal.h"

#define GET_PRIV(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), FEED_SETTINGS_TYPE, FeedSettingsPrivate))

struct _FeedSettingsPrivate {
	gchar *id;

	int original_expiry;
	gboolean original_download;
	gchar *original_path;
	int original_maxsize;

	GtkWidget *enable_expiry;
	GtkWidget *expiry_interval;

	GtkWidget *enable_enclosures;
	GtkWidget *download_path;
	GtkWidget *enclosure_maxsize;
};

enum {
	REMOVE,
	SAVE,
	UPDATE,
	LAST_SIGNAL
};

static guint signals [LAST_SIGNAL] = {0};

G_DEFINE_TYPE (FeedSettings, feed_settings, GTK_TYPE_VBOX);

static void check_and_save (FeedSettings *setts)
{
	int current_expiry;
	int current_maxsize;
	gboolean current_download;
	gboolean change;
	gchar *current_path;
	GString *sparql;
	GValue value;

	if (setts->priv->id != NULL) {
		change = FALSE;

		current_expiry = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (setts->priv->expiry_interval));
		current_download = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (setts->priv->enable_enclosures));
		current_path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (setts->priv->download_path));
		current_maxsize = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (setts->priv->enclosure_maxsize));

		sparql = g_string_new ("");

		if (current_expiry != setts->priv->original_expiry) {
			change = TRUE;
			g_string_append_printf (sparql, "DELETE {?s mfo:expiryInterval ?a} WHERE {<%s> mfo:feedSettings ?s . ?s mfo:expiryInterval ?a} "
							"INSERT {?s mfo:expiryInterval %d} WHERE {<%s> mfo:feedSettings ?s} ",
							setts->priv->id, current_expiry, setts->priv->id);

			memset (&value, 0, sizeof (GValue));
			g_value_init (&value, G_TYPE_INT);
			g_value_set_int (&value, current_expiry);
			g_signal_emit (setts, signals [UPDATE], 0, setts->priv->id, PREDICATE_EXPIRY, &value);
		}

		if (current_download != setts->priv->original_download) {
			change = TRUE;
			g_string_append_printf (sparql, "DELETE {?s mfo:downloadFlag ?a} WHERE {<%s> mfo:feedSettings ?s . ?s mfo:downloadFlag ?a} "
							"INSERT {?s mfo:downloadFlag %s} WHERE {<%s> mfo:feedSettings ?s} ",
							setts->priv->id, current_download ? "true" : "false", setts->priv->id);

			memset (&value, 0, sizeof (GValue));
			g_value_init (&value, G_TYPE_BOOLEAN);
			g_value_set_boolean (&value, current_download);
			g_signal_emit (setts, signals [UPDATE], 0, setts->priv->id, PREDICATE_DOWNLOAD, &value);
		}

		if (current_download == TRUE) {
			if (strcmp (current_path, setts->priv->original_path) != 0) {
				change = TRUE;
				g_string_append_printf (sparql, "DELETE {?s mfo:downloadPath ?a} WHERE {<%s> mfo:feedSettings ?s . ?s mfo:downloadPath ?a} "
								"INSERT {?s mfo:downloadPath \"%s\"} WHERE {<%s> mfo:feedSettings ?s} ",
								setts->priv->id, current_path, setts->priv->id);

				memset (&value, 0, sizeof (GValue));
				g_value_init (&value, G_TYPE_STRING);
				g_value_set_string (&value, current_path);
				g_signal_emit (setts, signals [UPDATE], 0, setts->priv->id, PREDICATE_DOWNPATH, &value);
			}

			if (current_maxsize != setts->priv->original_maxsize) {
				change = TRUE;
				g_string_append_printf (sparql, "DELETE {?s mfo:maxSize ?a} WHERE {<%s> mfo:feedSettings ?s . ?s mfo:maxSize ?a} "
								"INSERT {?s mfo:maxSize %d} WHERE {<%s> mfo:feedSettings ?s} ",
								setts->priv->id, current_maxsize, setts->priv->id);

				memset (&value, 0, sizeof (GValue));
				g_value_init (&value, G_TYPE_INT);
				g_value_set_int (&value, current_maxsize);
				g_signal_emit (setts, signals [UPDATE], 0, setts->priv->id, PREDICATE_MAXSIZE, &value);
			}
		}

		g_free (current_path);
		g_free (setts->priv->original_path);
		g_free (setts->priv->id);

		if (change == TRUE)
			g_signal_emit (setts, signals [SAVE], 0, g_string_free (sparql, FALSE), NULL);
		else
			g_string_free (sparql, TRUE);
	}
}

static void feed_settings_finalize (GObject *obj)
{
	FeedSettings *app;

	app = FEED_SETTINGS (obj);

	g_free (app->priv->original_path);
	g_free (app->priv->id);
}

static void feed_settings_class_init (FeedSettingsClass *klass)
{
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = feed_settings_finalize;

	signals [REMOVE] = g_signal_new ("remove-feed", G_TYPE_FROM_CLASS (klass),
					 G_SIGNAL_RUN_LAST, 0, NULL, NULL,
					 g_cclosure_marshal_VOID__STRING,
					 G_TYPE_NONE, 1, G_TYPE_STRING);

	signals [SAVE] = g_signal_new ("save-feed", G_TYPE_FROM_CLASS (klass),
				       G_SIGNAL_RUN_LAST, 0, NULL, NULL,
				       g_cclosure_marshal_VOID__STRING,
				       G_TYPE_NONE, 1, G_TYPE_STRING);

	signals [UPDATE] = g_signal_new ("update-feed", G_TYPE_FROM_CLASS (klass),
					 G_SIGNAL_RUN_LAST, 0, NULL, NULL,
					 marshal_VOID__STRING_INT_BOXED,
					 G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_VALUE);

	g_type_class_add_private (klass, sizeof (FeedSettingsPrivate));
}

static void toggle_section (GtkToggleButton *togglebutton, GtkWidget *section)
{
	gtk_widget_set_sensitive (section, gtk_toggle_button_get_active (togglebutton));
}

static void remove_feed (GtkButton *button, FeedSettings *setts)
{
	g_signal_emit (setts, signals [REMOVE], 0, setts->priv->id, NULL);
}

static void feed_settings_init (FeedSettings *app)
{
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *row;
	GtkWidget *button;

	app->priv = GET_PRIV (app);
	app->priv->id = NULL;

	frame = gtk_frame_new ("");
	gtk_container_border_width (GTK_CONTAINER (frame), 10);
	app->priv->enable_expiry = gtk_check_button_new_with_label ("Auto-Expiry");
	gtk_frame_set_label_widget (GTK_FRAME (frame), app->priv->enable_expiry);
	gtk_box_pack_start (GTK_BOX (app), frame, TRUE, TRUE, 0);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), 10);
	gtk_container_add (GTK_CONTAINER (frame), vbox);

	app->priv->expiry_interval = gtk_spin_button_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, G_MAXINT, 1, 1, 0)), 1, 0);
	row = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), gtk_label_new ("Items are automatically removed after "), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), app->priv->expiry_interval, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), gtk_label_new (" minutes"), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), row, FALSE, FALSE, 0);

	g_signal_connect (app->priv->enable_expiry, "toggled", G_CALLBACK (toggle_section), vbox);
	gtk_widget_set_sensitive (vbox, FALSE);

	frame = gtk_frame_new ("");
	gtk_container_border_width (GTK_CONTAINER (frame), 10);
	app->priv->enable_enclosures = gtk_check_button_new_with_label ("Enclosures");
	gtk_frame_set_label_widget (GTK_FRAME (frame), app->priv->enable_enclosures);
	gtk_box_pack_start (GTK_BOX (app), frame, TRUE, TRUE, 0);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), 10);
	gtk_container_add (GTK_CONTAINER (frame), vbox);

	app->priv->download_path = gtk_file_chooser_button_new ("", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	row = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), gtk_label_new ("Save enclosures in "), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), app->priv->download_path, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), row, FALSE, FALSE, 0);

	app->priv->enclosure_maxsize = gtk_spin_button_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, G_MAXINT, 1, 1, 0)), 1, 0);
	row = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), gtk_label_new ("Don't automatically download enclosures larger than "), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), app->priv->enclosure_maxsize, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (row), gtk_label_new (" kilobytes"), FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), row, FALSE, FALSE, 0);

	g_signal_connect (app->priv->enable_enclosures, "toggled", G_CALLBACK (toggle_section), vbox);
	gtk_widget_set_sensitive (vbox, FALSE);

	button = gtk_button_new_from_stock (GTK_STOCK_DELETE);
	g_signal_connect (button, "clicked", G_CALLBACK (remove_feed), app);
	gtk_box_pack_start (GTK_BOX (app), button, TRUE, TRUE, 0);
}

GtkWidget* feed_settings_new ()
{
	return g_object_new (FEED_SETTINGS_TYPE, NULL);
}

/*
	It is required to call this before every other feed_settings_set_*(), so to detect and
	eventually store changes applied to the previous ID
*/
void feed_settings_set_id (FeedSettings *setts, gchar *id)
{
	check_and_save (setts);
	setts->priv->id = g_strdup (id);
}

void feed_settings_set_download_enclosures (FeedSettings *setts, gboolean download)
{
	setts->priv->original_download = download;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (setts->priv->enable_enclosures), download);
}

void feed_settings_set_download_path (FeedSettings *setts, gchar *path)
{
	setts->priv->original_path = g_strdup (path);
	 gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (setts->priv->download_path), path);
}

void feed_settings_set_enclosures_maxsize (FeedSettings *setts, int maxsize)
{
	setts->priv->original_maxsize = maxsize;
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (setts->priv->enclosure_maxsize), maxsize);
}

void feed_settings_set_autoexpiry_interval (FeedSettings *setts, int expiry)
{
	if (expiry < 0)
		expiry = 0;

	setts->priv->original_expiry = expiry;
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (setts->priv->expiry_interval), expiry);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (setts->priv->enable_expiry), (expiry > 0));
}
