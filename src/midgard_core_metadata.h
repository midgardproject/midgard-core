/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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
 *   */

#ifndef MIDGARD_CORE_METADATA_H
#define MIDGARD_CORE_METADATA_H

#include "midgard_core_object.h"
#include "midgard_metadata.h"

/* Read only properties setters */
void	midgard_core_metadata_set_creator		(MidgardMetadata *self, const gchar *creator);
void	midgard_core_metadata_set_created		(MidgardMetadata *self, const GValue *value);
void	midgard_core_metadata_set_revisor		(MidgardMetadata *self, const gchar *revisor);
void	midgard_core_metadata_set_revised		(MidgardMetadata *self, const GValue *revised);
void	midgard_core_metadata_set_revision		(MidgardMetadata *self, guint revision);
void	midgard_core_metadata_set_locker		(MidgardMetadata *self, const gchar *locker);
void	midgard_core_metadata_set_locked		(MidgardMetadata *self, const GValue *locked);
void	midgard_core_metadata_set_approver		(MidgardMetadata *self, const gchar *approver);
void	midgard_core_metadata_set_approved		(MidgardMetadata *self, const GValue *approved);
void	midgard_core_metadata_set_size			(MidgardMetadata *self, guint size);
void	midgard_core_metadata_set_exported		(MidgardMetadata *self, const GValue *exported);
void	midgard_core_metadata_set_imported		(MidgardMetadata *self, const GValue *imported);
void	midgard_core_metadata_set_deleted		(MidgardMetadata *self, gboolean deleted);
void	midgard_core_metadata_set_isapproved		(MidgardMetadata *self, gboolean isapproved);
void	midgard_core_metadata_set_islocked		(MidgardMetadata *self, gboolean islocked);
void	midgard_core_metadata_set_schedule_start	(MidgardMetadata *self, const GValue *schedule);
void	midgard_core_metadata_set_schedule_end		(MidgardMetadata *self, const GValue *schedule);
void	midgard_core_metadata_set_published		(MidgardMetadata *self, const GValue *published);
void	midgard_core_metadata_set_authors		(MidgardMetadata *self, const gchar *authors);
void	midgard_core_metadata_set_owner			(MidgardMetadata *self, const gchar *owner);

/* routines */
void		midgard_core_metadata_increase_revision		(MidgardMetadata *self);
MidgardMetadata *midgard_core_metadata_copy			(MidgardMetadata *src);
guint 		midgard_core_metadata_get_size			(MidgardMetadata *self);

#endif /* MIDGARD_CORE_METADATA_H */
