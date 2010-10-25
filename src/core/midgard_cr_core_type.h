/* 
 * Copyright (C) 2005, 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_CR_CORE_TYPE_H
#define MIDGARD_CR_CORE_TYPE_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include <glib.h>
#include <glib-object.h>

/* Longtext */
#define MIDGARD_CR_CORE_TYPE_LONGTEXT (midgard_cr_core_longtext_get_type())
GType midgard_cr_core_longtext_get_type (void);

/* Guid */
#define MIDGARD_CR_CORE_TYPE_GUID (midgard_cr_core_guid_get_type())
GType midgard_cr_core_guid_get_type (void);

#endif /* MIDGARD_CR_CORE_TYPE_H */
