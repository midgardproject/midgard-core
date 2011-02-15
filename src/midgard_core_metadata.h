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

#define METADATA_PROPERTY_FIELD_PREFIX	"metadata_"

#define METADATA_PROPERTY_CREATOR	"creator"
#define METADATA_PROPERTY_CREATED	"created"
#define METADATA_PROPERTY_REVISOR	"revisor"
#define METADATA_PROPERTY_REVISED	"revised"
#define METADATA_PROPERTY_REVISION	"revision"
#define METADATA_PROPERTY_LOCKER	"locker"
#define METADATA_PROPERTY_LOCKED	"locked"
#define METADATA_PROPERTY_APPROVER	"approver"
#define METADATA_PROPERTY_APPROVED	"approved"
#define METADATA_PROPERTY_AUTHORS	"authors"
#define METADATA_PROPERTY_OWNER		"owner"
#define METADATA_PROPERTY_SIZE		"size"
#define METADATA_PROPERTY_EXPORTED	"exported"
#define METADATA_PROPERTY_IMPORTED	"imported"
#define METADATA_PROPERTY_DELETED	"deleted"
#define METADATA_PROPERTY_ISAPPROVED	"isapproved"
#define METADATA_PROPERTY_ISLOCKED	"islocked"
#define METADATA_PROPERTY_SCHEDULESTART	"schedule_start"
#define METADATA_PROPERTY_SCHEDULEEND	"schedule_end"
#define METADATA_PROPERTY_PUBLISHED	"published"

#define METADATA_FIELD_DELETED		"metadata_deleted"

/* Properties setters */
void	midgard_core_metadata_set_creator		(MidgardMetadata *self, const GValue *creator) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_created		(MidgardMetadata *self, const GValue *value) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_revisor		(MidgardMetadata *self, const GValue *revisor) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_revised		(MidgardMetadata *self, const GValue *revised) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_revision		(MidgardMetadata *self, const GValue *revision) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_locker		(MidgardMetadata *self, const GValue *locker) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_locked		(MidgardMetadata *self, const GValue *locked) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_approver		(MidgardMetadata *self, const GValue *approver) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_approved		(MidgardMetadata *self, const GValue *approved) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_size			(MidgardMetadata *self, const GValue *size) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_exported		(MidgardMetadata *self, const GValue *exported) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_imported		(MidgardMetadata *self, const GValue *imported) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_deleted		(MidgardMetadata *self, const GValue *deleted) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_isapproved		(MidgardMetadata *self, const GValue *isapproved) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_islocked		(MidgardMetadata *self, const GValue *islocked) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_schedule_start	(MidgardMetadata *self, const GValue *schedule) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_schedule_end		(MidgardMetadata *self, const GValue *schedule) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_published		(MidgardMetadata *self, const GValue *published) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_authors		(MidgardMetadata *self, const GValue *authors) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_owner			(MidgardMetadata *self, const GValue *owner) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_score			(MidgardMetadata *self, const GValue *score) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_hidden		(MidgardMetadata *self, const GValue *hidden) __attribute__((nonnull(1,2)));
void	midgard_core_metadata_set_navnoentry		(MidgardMetadata *self, const GValue *navnoentry) __attribute__((nonnull(1,2)));

/* MidgardDBObject */
gboolean	midgard_core_metadata_dbpriv_set_property 	(MidgardDBObject *self, const gchar *name, GValue *value);

/* routines */
void		midgard_core_metadata_increase_revision		(MidgardMetadata *self);
MidgardMetadata *midgard_core_metadata_copy			(MidgardMetadata *src);
guint 		midgard_core_metadata_get_size			(MidgardMetadata *self);

#endif /* MIDGARD_CORE_METADATA_H */
