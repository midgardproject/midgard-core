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

#ifndef MIDGARD_VALIDABLE_H
#define MIDGARD_VALIDABLE_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef enum {
	MIDGARD_VALIDATION_ERROR_NAME_INVALID,
	MIDGARD_VALIDATION_ERROR_NAME_DUPLICATED,
	MIDGARD_VALIDATION_ERROR_ELEMENT_INVALID,
	MIDGARD_VALIDATION_ERROR_TYPE_INVALID,
	MIDGARD_VALIDATION_ERROR_KEY_INVALID,
	MIDGARD_VALIDATION_ERROR_VALUE_INVALID,
	MIDGARD_VALIDATION_ERROR_REFERENCE_INVALID,
	MIDGARD_VALIDATION_ERROR_PARENT_INVALID,
	MIDGARD_VALIDATION_ERROR_LOCATION_INVALID,
	MIDGARD_VALIDATION_ERROR_INTERNAL
} MidgardValidationError;
#define MIDGARD_VALIDATION_ERROR midgard_validation_error_quark ()

/* convention macros */
#define MIDGARD_TYPE_VALIDABLE (midgard_validable_get_type())
#define MIDGARD_VALIDABLE(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_VALIDABLE, MidgardValidable))
#define MIDGARD_IS_VALIDABLE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_VALIDABLE))
#define MIDGARD_VALIDABLE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_VALIDABLE, MidgardValidableIFace))

typedef struct _MidgardValidable MidgardValidable; /* dummy */
typedef struct _MidgardValidableIFace MidgardValidableIFace;

struct _MidgardValidableIFace {
	GTypeInterface	parent;

	void			(*validate)		(MidgardValidable *self, GError **error);
	gboolean		(*is_valid)		(MidgardValidable *self);
};

GType		midgard_validable_get_type		(void);
void		midgard_validable_validate		(MidgardValidable *self, GError **error);
gboolean	midgard_validable_is_valid		(MidgardValidable *self);
GQuark		midgard_validation_error_quark		(void);

G_END_DECLS

#endif /* MIDGARD_VALIDABLE_H */
