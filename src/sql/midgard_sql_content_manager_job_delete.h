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

#ifndef MIDGARD_SQL_CONTENT_MANAGER_JOB_DELETE_H
#define MIDGARD_SQL_CONTENT_MANAGER_JOB_DELETE_H

#include <glib-object.h>
#include "midgard_sql_content_manager_job.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_DELETE (midgard_sql_content_manager_job_delete_get_type()) 
#define MIDGARD_SQL_CONTENT_MANAGER_JOB_DELETE(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_DELETE, MidgardSqlContentManagerJobDelete))
#define MIDGARD_SQL_CONTENT_MANAGER_JOB_DELETE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_DELETE, MidgardSqlContentManagerJobDeleteClass))
#define MIDGARD_IS_SQL_CONTENT_MANAGER_JOB_DELETE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_DELETE))
#define MIDGARD_IS_SQL_CONTENT_MANAGER_JOB_DELETE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_DELETE))
#define MIDGARD_SQL_CONTENT_MANAGER_JOB_DELETE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_DELETE, MidgardSqlContentManagerJobDeleteClass))

typedef struct _MidgardSqlContentManagerJobDelete MidgardSqlContentManagerJobDelete;
typedef struct _MidgardSqlContentManagerJobDeleteClass MidgardSqlContentManagerJobDeleteClass;

struct _MidgardSqlContentManagerJobDeleteClass {
	MidgardSqlContentManagerJobClass parent;
};

struct _MidgardSqlContentManagerJobDelete {
	MidgardSqlContentManagerJob parent;
};

GType 			midgard_sql_content_manager_job_delete_get_type	(void);

G_END_DECLS

#endif /* MIDGARD_SQL_CONTENT_MANAGER_JOB_DELETE_H */
