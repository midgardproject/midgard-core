/*
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_EXECUTABLE_H
#define MIDGARD_EXECUTABLE_H

#include <glib-object.h>
#include "midgard_pool.h"

G_BEGIN_DECLS

typedef enum {
	MIDGARD_EXECUTION_ERROR_DEPENDENCE_INVALID,
	MIDGARD_EXECUTION_ERROR_COMMAND_INVALID,
	MIDGARD_EXECUTION_ERROR_COMMAND_INVALID_DATA,
	MIDGARD_EXECUTION_ERROR_INTERNAL
} MidgardExecutionError;
#define MIDGARD_EXECUTION_ERROR midgard_execution_error_quark ()

/* convention macros */
#define MIDGARD_TYPE_EXECUTABLE (midgard_executable_get_type())
#define MIDGARD_EXECUTABLE(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_EXECUTABLE, MidgardExecutable))
#define MIDGARD_IS_EXECUTABLE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_EXECUTABLE))
#define MIDGARD_EXECUTABLE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_EXECUTABLE, MidgardExecutableIFace))

typedef struct _MidgardExecutable MidgardExecutable; /* dummy */
typedef struct _MidgardExecutableIFace MidgardExecutableIFace;

struct _MidgardExecutableIFace {
	GTypeInterface	parent;

	void		(*execute)		(MidgardExecutable *self, GError **error);

	/* signals */
	void            (*execution_start)      (GObject *object);
	void            (*execution_end)        (GObject *object);
};

GType		midgard_executable_get_type		(void);
void		midgard_executable_execute		(MidgardExecutable *self, GError **error);
GQuark		midgard_execution_error_quark 		(void);

G_END_DECLS

#endif /* MIDGARD_EXECUTABLE_H */
