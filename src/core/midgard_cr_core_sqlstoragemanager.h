/* MidgardCore SQLStorageManager routines
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

#ifndef _MIDGARD_CR_CORE_SQL_STORAGE_MANAGER_H_
#define _MIDGARD_CR_CORE_SQL_STORAGE_MANAGER_H_

#include <glib-object.h>
#include "midgard3.h"
#include "midgard_local.h"

gboolean midgard_cr_core_sql_storage_manager_open (MidgardCRSQLStorageManager *manager, GError **error);
gboolean midgard_cr_core_sql_storage_manager_close (MidgardCRSQLStorageManager *manager, GError **error);
gboolean midgard_cr_core_sql_storage_manager_initialize_storage (MidgardCRSQLStorageManager *manager, GError **error);

#endif /* _MIDGARD_CR_CORE_SQL_STORAGE_MANAGER_H_ */
