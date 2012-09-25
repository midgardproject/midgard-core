/* 
 * Copyright (C) 2012 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_CORE_SQL_HELPER_H
#define MIDGARD_CORE_SQL_HELPER_H

#include "midgard_sql_content_manager_job.h"

G_BEGIN_DECLS

struct _MidgardSqlContentManagerJobPrivate {
	GObject *content_object;
	MidgardObjectReference *reference;
	MidgardModel *model;
	MidgardConnection *connection;
	gboolean is_running;
	gboolean is_executed;
	gboolean is_failed;
	gboolean is_valid;
};

void	midgard_core_sql_content_manager_job_running	(MidgardSqlContentManagerJob *self);
void	midgard_core_sql_content_manager_job_failed	(MidgardSqlContentManagerJob *self);
void	midgard_core_sql_content_manager_job_succeed	(MidgardSqlContentManagerJob *self);

G_END_DECLS

#endif /* MIDGARD_CORE_SQL_HELPER_H */
