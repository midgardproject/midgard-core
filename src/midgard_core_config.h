/* 
 * Copyright (C) 2007 Piotr Pokora <piotrek.pokora@gmail.com>
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
 *   */

#ifndef MIDGARD_CORE_CONFIG_H
#define MIDGARD_CORE_CONFIG_H

#include <glib.h>
#include <glib/gstdio.h>

#define MIDGARD_CONFIG_RW_SCHEMA        "schema"
#define MIDGARD_CONFIG_RW_SCHEMAS	"schemas"
#define MIDGARD_CONFIG_RW_VIEW          "view"
#define MIDGARD_CONFIG_RW_VIEWS		"views"

typedef enum { 
        MIDGARD_DB_TYPE_MYSQL = 1, 
        MIDGARD_DB_TYPE_POSTGRES, 
        MIDGARD_DB_TYPE_FREETDS, 
        MIDGARD_DB_TYPE_SQLITE, 
        MIDGARD_DB_TYPE_ODBC, 
        MIDGARD_DB_TYPE_ORACLE 
} MidgardDBType;

gchar 		*midgard_core_config_build_path			(const gchar **dirs, const gchar *filename, gboolean user);
gchar		*midgard_core_config_build_schemadir_path	(const gchar *dirname);
gchar		*midgard_core_config_build_viewdir_path		(const gchar *dirname);

#endif /* MIDGARD_CORE_CONFIG_H */
