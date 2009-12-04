/* 
 * Copyright (C) 2006, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_OBJECT_PARAMETER_H
#define MIDGARD_OBJECT_PARAMETER_H

#include "midgard_object.h"

G_BEGIN_DECLS

const GValue	*midgard_object_get_parameter		(MidgardObject *self, const gchar *domain, const gchar *name);
gboolean 	midgard_object_set_parameter		(MidgardObject *self, const gchar *domain, const gchar *name, GValue *value);
MidgardObject 	**midgard_object_list_parameters	(MidgardObject *self, const gchar *domain);
gboolean 	midgard_object_delete_parameters	(MidgardObject *self, guint n_params, const GParameter *parameters);
gboolean 	midgard_object_purge_parameters		(MidgardObject *self, guint n_params, const GParameter *parameters);
MidgardObject 	**midgard_object_find_parameters	(MidgardObject *self, guint n_params, const GParameter *parameters);
gboolean 	midgard_object_has_parameters		(MidgardObject *self);		

G_END_DECLS

#endif /* MIDGARD_OBJECT_PARAMETER_H */
