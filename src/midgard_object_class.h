/* 
 * Copyright (C) 2005, 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_OBJECT_CLASS_H
#define MIDGARD_OBJECT_CLASS_H

#include <glib.h>
#include "midgard_object.h"

const gchar 		*midgard_object_class_get_primary_property	(MidgardObjectClass *klass); 
const gchar 		*midgard_object_class_get_property_parent	(MidgardObjectClass *klass);
const gchar 		*midgard_object_class_get_property_up		(MidgardObjectClass *klass);
const gchar 		*midgard_object_class_get_property_unique	(MidgardObjectClass *klass);
MidgardObjectClass 	**midgard_object_class_list_children		(MidgardObjectClass *klass);
MidgardObject 		*midgard_object_class_factory			(MidgardConnection *mgd, MidgardObjectClass *klass, const GValue *val);
MidgardObject 		*midgard_object_class_get_object_by_guid	(MidgardConnection *mgd, const gchar *guid);
MidgardObject 		*midgard_object_class_get_object_by_path	(MidgardConnection *mgd, const gchar *classname, const gchar *object_path);
gboolean 		midgard_object_class_undelete	(MidgardConnection *mgd, const gchar *guid);
gboolean 		midgard_object_class_has_metadata		(MidgardObjectClass *klass);
GObjectClass		*midgard_object_class_get_metadata_class	(MidgardObjectClass *klass);
const gchar 		*midgard_object_class_get_schema_value		(MidgardObjectClass *klass, const gchar *name);

#endif /* MIDGARD_OBJECT_CLASS_H */

