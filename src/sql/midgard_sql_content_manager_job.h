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

#ifndef MIDGARD_SQL_CONTENT_MANAGER_JOB_H
#define MIDGARD_SQL_CONTENT_MANAGER_JOB_H

#include <glib-object.h>
#include "midgard_connection.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB (midgard_sql_content_manager_job_get_type()) 
#define MIDGARD_SQL_CONTENT_MANAGER_JOB(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB, MidgardSqlContentManagerJob))
#define MIDGARD_SQL_CONTENT_MANAGER_JOB_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB, MidgardSqlContentManagerJobClass))
#define MIDGARD_IS_SQL_CONTENT_MANAGER_JOB(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB))
#define MIDGARD_IS_SQL_CONTENT_MANAGER_JOB_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB))
#define MIDGARD_SQL_CONTENT_MANAGER_JOB_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB, MidgardSqlContentManagerJobClass))

typedef struct _MidgardSqlContentManagerJob MidgardSqlContentManagerJob;
typedef struct _MidgardSqlContentManagerJobClass MidgardSqlContentManagerJobClass;
typedef struct _MidgardSqlContentManagerJobPrivate MidgardSqlContentManagerJobPrivate; 

struct _MidgardSqlContentManagerJobClass {
	GObjectClass parent;
};

struct _MidgardSqlContentManagerJob {
	GObject parent;

	/* < private > */
	MidgardSqlContentManagerJobPrivate *priv;
};

GType 			midgard_sql_content_manager_job_get_type	(void);
MidgardConnection*	midgard_sql_content_manager_job_get_connection	(MidgardSqlContentManagerJob *self, GError **error);

G_END_DECLS

#endif /* MIDGARD_SQL_CONTENT_MANAGER_JOB_H */
