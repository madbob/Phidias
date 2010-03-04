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

#ifndef PHIDIAS_EXPANDERS_H
#define PHIDIAS_EXPANDERS_H

#include "common.h"

#define CONTAINER_INFO_MAIN_CLASS	"mfo:FeedChannel"
#define ITEM_INFO_MAIN_CLASS		"mfo:FeedMessage"
#define ITEM_TO_CONTAINER_PREDICATE	"nmo:communicationChannel"
#define ITEM_FILTER_CRITERIA		ITEM_INFO_TITLE

extern gchar *CONTAINER_INFO_PREDICATES [];

typedef enum {
	CONTAINER_INFO_SUBJECT,
	#define CONTAINER_INFO(__i, __n)	CONTAINER_INFO_##__i,
	#include "container-conf.h"
	#undef CONTAINER_INFO
	CONTAINER_INFO_LAST
} CONTAINER_INFOS;

extern gchar *ITEM_INFO_PREDICATES [];

typedef enum {
	ITEM_INFO_SUBJECT,
	#define ITEM_INFO(__i, __n)		ITEM_INFO_##__i,
	#include "item-conf.h"
	#undef ITEM_INFO
	ITEM_INFO_LAST
} ITEM_INFOS;

#endif
