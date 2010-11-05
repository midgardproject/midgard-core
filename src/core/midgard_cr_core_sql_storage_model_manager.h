/* MidgardCore SQLStorageModelManager routines
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

#ifndef _MIDGARD_CR_CORE_SQL_STORAGE_MODEL_MANAGER_H_
#define _MIDGARD_CR_CORE_SQL_STORAGE_MODEL_MANAGER_H_

#include <glib-object.h>
#include "midgard_local.h"

struct _MidgardCRSQLStorageModelManager;
struct _MidgardCRModel;

void midgard_cr_core_sql_table_model_manager_prepare_create (struct _MidgardCRSQLStorageModelManager *manager, struct _MidgardCRModel *model, GError **error);
void midgard_cr_core_sql_table_model_manager_prepare_update (struct _MidgardCRSQLStorageModelManager *manager, GError **error);
void midgard_cr_core_sql_table_model_manager_prepare_save (struct _MidgardCRSQLStorageModelManager *manager, GError **error);
void midgard_cr_core_sql_table_model_manager_prepare_delete (struct _MidgardCRSQLStorageModelManager *manager, GError **error);
void midgard_cr_core_sql_table_model_manager_prepare_purge (struct _MidgardCRSQLStorageModelManager *manager, GError **error);
void midgard_cr_core_sql_table_model_manager_execute (struct _MidgardCRSQLStorageModelManager *manager, GError **error);

#endif /* _MIDGARD_CR_CORE_SQL_STORAGE_MODEL_MANAGER_H_ */
