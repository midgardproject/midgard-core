/* 
 * Copyright (C) 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_CORE_OBJECT_PARAMETER_H
#define MIDGARD_CORE_OBJECT_PARAMETER_H

G_BEGIN_DECLS

MidgardObject **midgard_core_object_parameters_list(
		MidgardConnection *mgd, const gchar *class_name, const gchar *guid);

MidgardObject *midgard_core_object_parameters_create(
		MidgardConnection *mgd, const gchar *class_name, 
		const gchar *guid, guint n_params, const GParameter *parameters);

gboolean midgard_core_object_parameters_delete(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters);

gboolean midgard_core_object_parameters_purge(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters);

gboolean midgard_core_object_parameters_purge_with_blob(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters);

MidgardObject **midgard_core_object_parameters_find(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters);

gboolean midgard_core_object_has_parameters(
                MidgardConnection *mgd, const gchar *class_name, const gchar *guid);

G_END_DECLS

#endif /* MIDGARD_CORE_OBJECT_PARAMETER_H */
