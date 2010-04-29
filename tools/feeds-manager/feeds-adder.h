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

#ifndef __FEEDS_ADDER_H__
#define __FEEDS_ADDER_H__

#include "common.h"

#define FEEDS_ADDER_TYPE		(feeds_adder_get_type())
#define FEEDS_ADDER(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o),	\
					 FEEDS_ADDER_TYPE,			\
					 FeedsAdder))
#define FEEDS_ADDER_CLASS(c)		(G_TYPE_CHECK_CLASS_CAST ((c),		\
					 FEEDS_ADDER_TYPE,			\
					 FeedsAdderClass))
#define IS_FEEDS_ADDER(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o),	\
					 FEEDS_ADDER_TYPE))
#define IS_FEEDS_ADDER_CLASS(c)		(G_TYPE_CHECK_CLASS_TYPE ((c),		\
					 FEEDS_ADDER_TYPE))
#define FEEDS_ADDER_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o),	\
					 FEEDS_ADDER_TYPE,			\
					 FeedsAdderClass))

typedef struct _FeedsAdder		FeedsAdder;
typedef struct _FeedsAdderClass		FeedsAdderClass;
typedef struct _FeedsAdderPrivate	FeedsAdderPrivate;

struct _FeedsAdder {
	GtkVBox parent;
	FeedsAdderPrivate *priv;
};

struct _FeedsAdderClass {
	GtkVBoxClass parent;
};

GType		feeds_adder_get_type		();

GtkWidget*	feeds_adder_new			();
void		feeds_adder_wire_tracker	(FeedsAdder *adder, TrackerClient *tracker_client);

#endif /* __FEEDS_ADDER_H__ */
