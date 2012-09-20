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

#include "midgard_core_sql_helper.h"

void	
midgard_core_sql_content_manager_job_running (MidgardSqlContentManagerJob *self)
{
	g_return_if_fail (MIDGARD_IS_SQL_CONTENT_MANAGER_JOB(self));
	self->priv->is_running = TRUE;
	self->priv->is_failed = FALSE;
	self->priv->is_executed = FALSE;
}

void	
midgard_core_sql_content_manager_job_failed (MidgardSqlContentManagerJob *self)
{
	g_return_if_fail (MIDGARD_IS_SQL_CONTENT_MANAGER_JOB(self));
	self->priv->is_running = FALSE;
	self->priv->is_failed = TRUE;
	self->priv->is_executed = TRUE;
}

void	
midgard_core_sql_content_manager_job_succeed (MidgardSqlContentManagerJob *self)
{
	g_return_if_fail (MIDGARD_IS_SQL_CONTENT_MANAGER_JOB(self));
	self->priv->is_running = FALSE;
	self->priv->is_failed = FALSE;
	self->priv->is_executed = TRUE;
}
