/* 
 * Copyright (C) 2006 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_ERROR_H
#define MIDGARD_ERROR_H

#include "midgard_connection.h"

G_BEGIN_DECLS

#define MIDGARD_GENERIC_ERROR midgard_error_generic()
#define MGD_GENERIC_ERROR MIDGARD_GENERIC_ERROR

typedef enum
{
	MGD_ERR_OK = -0, /**< MD_ERR_OK  <br /> */ 
	MGD_ERR_ERROR = -1, /**<  <br />*/ 
	MGD_ERR_ACCESS_DENIED = -2, /**< Access denied *  <br /> */
	MGD_ERR_NO_METADATA = -3,
	MGD_ERR_NOT_OBJECT = -4, /**< Not Midgard Object   <br /> */
	MGD_ERR_NOT_EXISTS = -5, /**< Object does not exist   <br /> */
	MGD_ERR_INVALID_NAME = -6 , /**<   <br /> */
	MGD_ERR_DUPLICATE = -7 , /**< Object already exist   <br /> */
	MGD_ERR_HAS_DEPENDANTS = -8 , /**< Object has dependants   <br /> */
	MGD_ERR_RANGE = -9, /**<   <br /> */
	MGD_ERR_NOT_CONNECTED = -10, /**< Not connected to the Midgard database   <br /> */
	MGD_ERR_SG_NOTFOUND = -11, /**< Sitegroup not found   <br /> */
	MGD_ERR_INVALID_OBJECT = -12, /**< Object not registered as Midgard Object   <br /> */ 
	MGD_ERR_QUOTA = -13, /**< Quota limit reached   <br /> */
	MGD_ERR_INTERNAL = -14, /**< Critical internal error   <br /> */
	MGD_ERR_OBJECT_NAME_EXISTS = -15, /**< Object with such name exists in tree   <br /> */
	MGD_ERR_OBJECT_NO_STORAGE = -16, /**< Storage table not defined for object   <br /> */
	MGD_ERR_OBJECT_NO_PARENT= -17, /**< Parent object in tree not defined   <br /> */
	MGD_ERR_INVALID_PROPERTY_VALUE = -18, /**< Invalid property value   <br /> */
	MGD_ERR_INVALID_PROPERTY = -19, /**<Invalid property <br />*/
	MGD_ERR_USER_DATA = -20, /**< Empty error message reserved for application's developers   <br /> */
	MGD_ERR_OBJECT_DELETED = -21, /**< Object deleted   <br /> */
	MGD_ERR_OBJECT_PURGED = -22, /**< Object purged   <br /> */
	MGD_ERR_OBJECT_EXPORTED = -23, /**< Object already exported   <br /> */
	MGD_ERR_OBJECT_IMPORTED = -24, /**< Object already imported   <br /> */
	MGD_ERR_MISSED_DEPENDENCE = -25, /**< Missed dependence for object   <br /> */
	MGD_ERR_TREE_IS_CIRCULAR = -26, /**< Circular reference found in object's tree <br />*/
	MGD_ERR_OBJECT_IS_LOCKED = -27 /**< Object is locked */
}MidgardErrorGeneric;

GQuark 		midgard_error_generic		(void);
const gchar 	*midgard_error_string		(GQuark domain, gint errcode);
void 		midgard_set_error		(MidgardConnection *mgd, GQuark domain, gint errcode, const gchar *msg, ...);
void 		midgard_error_default_log	(const gchar *domain, GLogLevelFlags level, const gchar *msg, gpointer ptr);
gint 		midgard_error_parse_loglevel	(const gchar *levelstring);

#define MIDGARD_ERRNO_SET(str, errcode)  \
	str->errnum = errcode;  \
	midgard_set_error(str, \
	MGD_GENERIC_ERROR, \
	errcode, \
	NULL); \
	g_signal_emit_by_name(str, "error"); 	

#define MIDGARD_ERRNO_SET_STRING(__mgd, __errcode, __format...) G_STMT_START { \
	__mgd->errnum = __errcode; \
	midgard_set_error(__mgd, \
			MGD_GENERIC_ERROR, \
                        __errcode, __format); \
        g_clear_error(&__mgd->err); \
	g_signal_emit_by_name (__mgd, "error"); \
	} G_STMT_END

void mgd_info(const gchar *format, ...);

G_END_DECLS

#endif /* MIDGARD_ERROR_H */
