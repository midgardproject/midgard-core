/*
 * Copyright (c) 2005 Jukka Zitting <jz@yukatan.fi>
 * Copyright (C) 2008, 2009, 2010 Piotr Pokora <piotrek.pokora@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef MIDGARD_TIMESTAMP_H
#define MIDGARD_TIMESTAMP_H

#include <glib.h>
#include <glib-object.h>

#define MIDGARD_DEFAULT_DATETIME "0001-01-01 00:00:00+0000"
#define MIDGARD_TIMESTAMP_DEFAULT MIDGARD_DEFAULT_DATETIME

typedef struct {
	glong year;
	gint month;
	gint day;
	gint hour;
	gint minute;
	gint second;
	gulong nano;
	glong offset;
	guint64 date_value;
	gchar *date_string;
} MidgardTimestamp;

#define MIDGARD_TYPE_TIMESTAMP (midgard_timestamp_get_type())

GType midgard_timestamp_get_type(void);

void			midgard_timestamp_set_current_time	(const GValue *value);
gchar 			*midgard_timestamp_get_string_from_value (const GValue *value);
gchar 			*midgard_timestamp_get_string		(MidgardTimestamp *mt);
MidgardTimestamp 	*midgard_timestamp_new			(void);
void			midgard_timestamp_new_current		(GValue *value);
GValue 			*midgard_timestamp_new_value_from_iso8601 (const gchar *iso_date);
MidgardTimestamp 	*midgard_timestamp_new_from_iso8601	(const gchar *iso_date);
void 			midgard_timestamp_free			(MidgardTimestamp *mt);

void			midgard_core_timestamp_set_current_time (MidgardTimestamp *tm);

#endif
