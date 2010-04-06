/* 
 * Copyright (C) 2005 Piotr Pokora <piotr.pokora@infoglob.pl>
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

#ifndef MIDGARD_TYPE_H
#define MIDGARD_TYPE_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include <glib.h>
#include <glib-object.h>

/* Longtext */
#define MGD_TYPE_LONGTEXT (midgard_longtext_get_type())

GType midgard_longtext_get_type(void);


#define MGD_TYPE_PARAM_LONGTEXT (midgard_param_longtext_get_type())
#define MGD_IS_PARAM_SPEC_LONGTEXT(pspec)    (G_TYPE_CHECK_INSTANCE_TYPE ((pspec), MGD_TYPE_PARAM_LONGTEXT))
#define MGD_PARAM_SPEC_LONGTEXT(pspec)       (G_TYPE_CHECK_INSTANCE_CAST ((pspec), MGD_TYPE_PARAM_LONGTEXT, MidgardParamSpecLongtext))

typedef struct _MidgardParamSpecLongtext MidgardParamSpecLongtext;

GType midgard_param_longtext_get_type(void);

/* Timestamp */
#define MGD_TYPE_TIMESTAMP MIDGARD_TYPE_TIMESTAMP

/* Guid */
#define MGD_TYPE_GUID (midgard_guid_get_type())

GType midgard_guid_get_type(void);

/* Routines */
#define MGD_TYPE_NONE G_TYPE_NONE
#define MGD_TYPE_STRING G_TYPE_STRING
#define MGD_TYPE_UINT G_TYPE_UINT
#define MGD_TYPE_FLOAT G_TYPE_FLOAT
#define MGD_TYPE_BOOLEAN G_TYPE_BOOLEAN
#define MGD_TYPE_INT G_TYPE_INT

#endif /* MIDGARD_TYPE_H */
