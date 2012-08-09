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

#include <midgard_error.h>
#include "midgard_core_object.h"
#include "midgard_datatypes.h"
#include <sys/types.h>
#include <unistd.h>

/**
 * midgard_error_generic:
 *
 * GQuark for Midgard Error. It's used by Midgard Error implementation, and 
 * probably not needed to use by any application.
 *
 * Returns: MGD_GENERIC_ERROR GQuark
 */
GQuark midgard_error_generic(void)
{
	static GQuark q = 0;
	if (q == 0)
		q = g_quark_from_static_string ("midgard-generic-error-quark");
	return q;
}

/**
 * midgard_error_string:
 * @domain: GQuark which represents MidgardError domain.
 * @errcode: MidgardErrorGeneric enum value.
 *
 * Get error message for the given error code.
 *
 * Returns: error messages which is owned by midgard-core and should not be freed.
 */ 
const gchar *midgard_error_string(GQuark domain, gint errcode)
{
	switch(errcode){

		case MGD_ERR_OK:
			return _("MGD_ERR_OK");
			break;

		case MGD_ERR_ACCESS_DENIED:
			return _("Access Denied.");
			break;
		
		case MGD_ERR_NO_METADATA:
			return _("Metadata class not defined.");
			break;

		case MGD_ERR_NOT_OBJECT:
			return _("Not Midgard Object.");
			break;

		case MGD_ERR_NOT_EXISTS:
			return _("Object does not exist.");
			break;

		case MGD_ERR_INVALID_NAME:
			return _("Invalid characters in object's name.");
			break;

		case MGD_ERR_DUPLICATE:
			return _("Object already exist.");
			break;
			
		case MGD_ERR_HAS_DEPENDANTS:
			return _("Object has dependants.");
			break;
			
		case MGD_ERR_RANGE:
			return _("Date range error."); /* FIXME , replace with invalid date range or format */
			break;

		case MGD_ERR_NOT_CONNECTED:
			return _("Not connected to the Midgard database.");
			break;

		case MGD_ERR_SG_NOTFOUND:
			return _("Sitegroup not found.");
			break;

		case MGD_ERR_INVALID_OBJECT:
			return _("Object not registered as Midgard Object.");
			break;
			
		case MGD_ERR_QUOTA:
			return _("Quota limit reached.");
			break;
			
		case MGD_ERR_INTERNAL:
			return _("Critical internal error.");
			break;

		case MGD_ERR_OBJECT_NAME_EXISTS:
			return _("Object with such name exists in tree.");
			break;
			
		case MGD_ERR_OBJECT_NO_STORAGE:
			return _("Storage table not defined for object.");
			break;

		case MGD_ERR_OBJECT_NO_PARENT:
			return _("Parent object in tree not defined.");
			break;

		case MGD_ERR_INVALID_PROPERTY_VALUE:
			return _("Invalid property value.");
			break;

		case MGD_ERR_INVALID_PROPERTY:
			return _("Invalid property.");
			break;

		case MGD_ERR_USER_DATA:
			return "";
			break;

		case MGD_ERR_OBJECT_DELETED:
			return _("Object deleted.");
			break;

		case MGD_ERR_OBJECT_PURGED:
			return _("Object purged.");
			break;

		case MGD_ERR_OBJECT_EXPORTED:
			return _("Object already exported.");
			break;

		case MGD_ERR_OBJECT_IMPORTED:
			return _("Object already imported.");
			break;

		case MGD_ERR_MISSED_DEPENDENCE:
			return _("Missed dependence for object.");
			break;

		case MGD_ERR_TREE_IS_CIRCULAR:
			return _("Circular reference found in object's tree.");
			break;

		case MGD_ERR_OBJECT_IS_LOCKED:
			return _("Object is locked");
			break;

		default:
			return _("Undefined error");
					
	}		
}

static gchar* _midgard_error_format(const gchar *msg, va_list args)
{
	if(!msg)
		return g_strdup("");

	gchar *new_string = 
		g_strdup_vprintf(msg, args);

	return new_string;
}

/**
 * midgard_set_error:
 * @mgd: #MidgardConnection instance 
 * @domain: GQuark which represents MidgardError domain
 * @errcode: #MidgardErrorGeneric enum value
 * @msg: a message which should be appended to string represented by errcode
 * @...: message argument list ( if required )
 *
 * This function sets internal error constant, and creates new error message.
 * User defined message is appended to internal one.
 * Any message created by application ( and its corresponding constant ) are destroyed 
 * and reset to MGD_ERR_OK when any API function is invoked.
 * Second @domain parameter is optional , and can be safely defined as NULL for 
 * MGD_GENERIC_ERROR domain.
 *
 * <example>
 * <programlisting>
 *	
 *	void set_wrong_property(MidgardConnection *mgd, gchar *prop)
 *	{
 *		midgard_set_error(mgd, NULL, 
 *				MGD_ERR_INVALID_PROPERTY_VALUE,
 *				"My application doesn't accept %s property",
 *				prop);
 *	}
 * </programlisting>
 * </example>
 */ 
void midgard_set_error(
		MidgardConnection *mgd, GQuark domain, gint errcode, const gchar *msg, ...)
{
	g_assert(mgd != NULL);

	if(!domain) 
		domain = MGD_GENERIC_ERROR;

	static GMutex mutex;
	g_mutex_lock (&mutex);

	if(mgd->errstr)
		g_free(mgd->errstr);

	if(mgd->err)
		g_clear_error(&mgd->err);

	gchar *new_msg;
	va_list args;

	va_start(args, msg);
	new_msg =  _midgard_error_format(msg, args);
	va_end(args);	

	g_clear_error(&mgd->err);

	/* watch out! midgard 1.7 and midcom needs MGD_ERR_OK string.
	 * Keep string formatters together */
	g_set_error(&mgd->err, domain, errcode,
			"%s%s", 
			midgard_error_string(domain, errcode),
			new_msg);
	g_free(new_msg);

	mgd->errnum = errcode;
	if(!mgd->err->message)
		mgd->errstr = g_strdup("");
	else
		mgd->errstr = g_strdup(mgd->err->message);	

	g_mutex_unlock (&mutex);

	return;		
}

/**
 * midgard_error_default_log:
 * @domain: domain for the given log message
 * @level: GLogLevelFlags
 * @msg: log message
 * @ptr: pointer to structure which holds loglevel 
 *
 * @ptr pointer may be a pointer to #MidgardConnection or #MidgardTypeHolder
 * structure. This function checks pointer type using MIDGARD_IS_CONNECTION
 * convention macro. Next midgard_connection_get_loglevel is called to get loglevel.
 * If MidgardConnection check fails , a typecast to MidgardTypeHolder is made.
 * In this case, level member is used to get loglevel.
 *
 * You are responsible to correctly set MidgardConnection or MidgardTypeHolder
 * before passing ptr argument. The main approach is to follow configuration's
 * loglevel even if MidgardConnection pointer is not yet available.
 *
 * @see midgard_connection_set_loglevel() to set log level.	
 */
void midgard_error_default_log(const gchar *domain, GLogLevelFlags level,
		const gchar *msg, gpointer ptr)
{
	gchar *level_ad = NULL;
	guint mlevel;
	GFileOutputStream *output_stream = NULL;
	MidgardConnection *mgd;
	MidgardTypeHolder *holder;

	if(ptr == NULL) {

		mlevel = G_LOG_LEVEL_WARNING;
	
	} else {
		
		if(MIDGARD_IS_CONNECTION((MidgardConnection *) ptr)) {
			
			mgd = MIDGARD_CONNECTION(ptr);
			mlevel = midgard_connection_get_loglevel(mgd);
			if(mgd->priv->config != NULL && mgd->priv->config->priv != NULL)
				output_stream = mgd->priv->config->priv->output_stream;
		
		} else {
			
			holder = (MidgardTypeHolder *) ptr;
			mlevel = holder->level;
		}
	}

	switch (level) {
		case G_LOG_FLAG_RECURSION:
			level_ad =  "RECURSION";
			break;
			
		case G_LOG_FLAG_FATAL:
			level_ad = "FATAL! ";
			break;
			
		case G_LOG_LEVEL_ERROR:
			/* g_on_error_stack_trace(NULL); */
			level_ad =  "ERROR";
			break;
			
		case G_LOG_LEVEL_CRITICAL:
			level_ad = "CRITICAL ";
			break;
			
		case G_LOG_LEVEL_WARNING:
			level_ad =  "WARNING";
			break;
			
		case G_LOG_LEVEL_MESSAGE:
			level_ad = "m";
			break;
		
		case G_LOG_LEVEL_INFO:
			level_ad = "info";
			break;
			
		case G_LOG_LEVEL_DEBUG:
			level_ad = "debug";
			break;

		default:
			level_ad = "midgard-unknown-log-level";
			break;
			
	}

	if (mlevel >= level) {
	
		GString *logstr = g_string_new("");
		g_string_append_printf(logstr,
				"%s (pid:%ld):(%s):%s\n",
				domain,
				(unsigned long)getpid(),
				level_ad,
				msg);
	
		gchar *tmpstr = g_string_free(logstr, FALSE);

		if(output_stream) {
			GError *err = NULL;
			g_output_stream_write (G_OUTPUT_STREAM (output_stream), 
					(const gchar *) tmpstr, strlen (tmpstr), NULL, &err);
			g_output_stream_flush (G_OUTPUT_STREAM (output_stream), NULL, NULL);
			//g_output_stream_close_async (G_OUTPUT_STREAM (output_stream), 1, NULL, NULL, NULL);
			if (err)
				g_warning ("Logfile write error: %s", err && err->message ? err->message : err->message);
			
		} else {

			fprintf(stderr, "%s", tmpstr);
			fflush(stderr);
		}

		g_free(tmpstr);
	}
}


/**
 * midgard_error_parse_loglevel:
 * @levelstring: string which should be parsed
 *
 * This function returns level registered in GLib. 
 *
 * Returns: #GLogLevelFlags or -1 on failure
 */
gint midgard_error_parse_loglevel(const gchar *levelstring)
{
	g_assert(levelstring != NULL);

	guint level = 0, mask = 0;
	guint len = strlen(levelstring);
	gchar *newlevel = g_ascii_strdown(levelstring, len);

	if(g_str_equal(newlevel, "error")) 
		level = G_LOG_LEVEL_ERROR;
	if(g_str_equal(newlevel, "critical"))
		level = G_LOG_LEVEL_CRITICAL;
	if(g_str_equal(newlevel, "warn"))
		level = G_LOG_LEVEL_WARNING;
	if(g_str_equal(newlevel, "warning"))
		level = G_LOG_LEVEL_WARNING;
	if(g_str_equal(newlevel, "message"))	
		level = G_LOG_LEVEL_MESSAGE;
	if(g_str_equal(newlevel, "info"))
		level = G_LOG_LEVEL_INFO;
	if(g_str_equal(newlevel, "debug"))
		level = G_LOG_LEVEL_DEBUG;

	g_free(newlevel);
	
	switch(level){

		case G_LOG_LEVEL_ERROR:
		case G_LOG_LEVEL_CRITICAL:
		case G_LOG_LEVEL_WARNING:
		case G_LOG_LEVEL_MESSAGE:
		case G_LOG_LEVEL_INFO:
		case G_LOG_LEVEL_DEBUG:
			for (; level && level > G_LOG_FLAG_FATAL; level >>= 1) {
				mask |= level;
			}
			return  mask;
			break;

		default:
			return -1;
	}
	
	return -1;	
}

/* temporary, we should use g_info, but it doesn't exist */
void mgd_info(const gchar *format, ...)
{
	va_list args;
	va_start (args, format);
	g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_INFO, format, args);
	va_end (args);
}
