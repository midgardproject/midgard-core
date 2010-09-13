/* MidgardCore Config routines
 *    
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef _MIDGARD_CORE_CONFIG_H_
#define _MIDGARD_CORE_CONFIG_H_

#include <glib-object.h>
#include "midgard3.h"
#include "midgard_local.h"

typedef enum {
	MIDGARD_CORE_DB_TYPE_MYSQL = 1,
	MIDGARD_CORE_DB_TYPE_POSTGRES,
	MIDGARD_CORE_DB_TYPE_FREETDS,
	MIDGARD_CORE_DB_TYPE_SQLITE,
	MIDGARD_CORE_DB_TYPE_ODBC,
	MIDGARD_CORE_DB_TYPE_ORACLE
} MidgardCoreDBType;

gboolean	midgard_core_config_read_file		(MidgardConfig *config, const gchar *name, gboolean user, GError **error);
gboolean        midgard_core_config_read_file_at_path	(MidgardConfig *self, const gchar *filepath, GError **error);
gboolean        midgard_core_config_read_data           (MidgardConfig *self, const gchar *data, GError **error);
gchar           **midgard_core_config_list_files        (gboolean user);
gboolean        midgard_core_config_save_file           (MidgardConfig *self, const gchar *name, gboolean user, GError **error);
gboolean        midgard_core_config_create_blobdir      (MidgardConfig *self);
gchar 		*midgard_core_config_get_default_confdir();

#endif /* _MIDGARD_CORE_CONFIG_H_ */
