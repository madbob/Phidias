/*  Copyright (C) 2010 Roberto Guido <madbob@users.barberaware.org>
 *
 *  This file is part of Phidias
 *
 *  Phidias is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FEED_SETTINGS_H__
#define __FEED_SETTINGS_H__

#include "common.h"

#define FEED_SETTINGS_TYPE		(feed_settings_get_type())
#define FEED_SETTINGS(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o),	\
					 FEED_SETTINGS_TYPE,			\
					 FeedSettings))
#define FEED_SETTINGS_CLASS(c)		(G_TYPE_CHECK_CLASS_CAST ((c),		\
					 FEED_SETTINGS_TYPE,			\
					 FeedSettingsClass))
#define IS_FEED_SETTINGS(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o),	\
					 FEED_SETTINGS_TYPE))
#define IS_FEED_SETTINGS_CLASS(c)	(G_TYPE_CHECK_CLASS_TYPE ((c),		\
					 FEED_SETTINGS_TYPE))
#define FEED_SETTINGS_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o),	\
					 FEED_SETTINGS_TYPE,			\
					 FeedSettingsClass))

typedef struct _FeedSettings		FeedSettings;
typedef struct _FeedSettingsClass	FeedSettingsClass;
typedef struct _FeedSettingsPrivate	FeedSettingsPrivate;

struct _FeedSettings {
	GtkVBox parent;
	FeedSettingsPrivate *priv;
};

struct _FeedSettingsClass {
	GtkVBoxClass parent;
};

GType		feed_settings_get_type			();

GtkWidget*	feed_settings_new			();
void		feed_settings_set_id			(FeedSettings *setts, gchar *id);
void		feed_settings_set_download_enclosures	(FeedSettings *setts, gboolean download);
void		feed_settings_set_download_path		(FeedSettings *setts, gchar *path);
void		feed_settings_set_enclosures_maxsize	(FeedSettings *setts, int maxsize);
void		feed_settings_set_autoexpiry_interval	(FeedSettings *setts, int expiry);

#endif /* __FEED_SETTINGS_H__ */
