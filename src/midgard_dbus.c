/* 
 * Copyright (C) 2008, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_dbus.h"
#include "midgard_core_object.h"
#include "midgard_replicator.h"
#include "midgard_config_auto.h"
#include "uuid.h"

#define MIDGARD_DBUS_SERVICE	"org.midgardproject.notifier"
#define MIDGARD_DBUS_IFACE	"org.midgardproject.notifier"
#define MIDGARD_DBUS_PATH	"/org/midgardproject"

/* for auto generated interfaces */
gboolean midgard_dbus_emit_notified(MidgardDbus *self, const gchar *message, GError **error);

#ifdef MGD_HAVE_DBUS
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include "midgard_dbus_interface.h"

DBusGProxy 		*__get_bus_proxy	(MidgardConnection *mgd, DBusGConnection *bus, const gchar *path);
static DBusGConnection 	*__get_bus		(gboolean use_session);

struct _MidgardDbusPrivate{

	DBusGConnection *dbus_connection;
	gchar *path;
	gchar *fullpath;
	gchar *message;
	MidgardConnection *mgd;
	GMainLoop *mainloop;
};

static DBusGProxy *
__get_freedesktop_proxy(DBusGConnection *bus)
{
	DBusGProxy *bus_proxy = NULL;

	bus_proxy = dbus_g_proxy_new_for_name (bus,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS,
			DBUS_INTERFACE_DBUS);

	if (!bus_proxy)
		g_debug("Failed to get freedesktop bus proxy");
	
	return bus_proxy;
}

static DBusGConnection* 
__get_bus(gboolean use_session)
{
	GError *error = NULL;
	DBusGConnection *bus = dbus_g_bus_get(use_session ? DBUS_BUS_SESSION : DBUS_BUS_SYSTEM, &error);

	if (!bus) {

		g_debug("Failed to get bus (%s)", error && error->message ? error->message : "Unknown reason");
		return NULL;
	}

	g_debug("Initialize D-Bus");

	return bus;
}
#endif

/* This is signal invoked by GLib D-Bus bindings. */
gboolean 
midgard_dbus_emit_notified(MidgardDbus *self, const gchar *message, GError **error)
{
#ifdef MGD_HAVE_DBUS

	g_signal_emit(self, MIDGARD_DBUS_GET_CLASS(self)->signal_notified, 0, message);

	/* Quit the main loop. We do this *just on time* here.
	   If application is slow, we use as much time as required. 
	   We could use g_timeout, but quiting loop here ensures our signal is emitted properly */
	GMainLoop *mainloop = self->priv->mainloop;

	if (mainloop && g_main_loop_is_running(mainloop))
		g_main_loop_quit(mainloop);

	return TRUE;
#else 
	return FALSE;
#endif
}

gchar *
__modify_path(MidgardConnection *mgd, const gchar *path)
{
	g_assert(mgd != NULL);

	if(*path != '/') {
		g_warning("Invalid path. Should start with '/'.");
		return NULL;
	}

	gchar *new_path = g_strconcat(MIDGARD_DBUS_PATH, path, NULL);
	/* g_debug("New midgard path for D-Bus: (%s)", new_path); */

	return new_path;
}

#ifdef MGD_HAVE_DBUS
/* This is invoked for an object which is connected to D-Bus signal.
   Then we emit MidgardDBusClass notified signal to trigger user's callback invokation */
static void
notified_signal_handler (DBusGProxy *proxy, const char *msg, gpointer user_data)
{
	if (!user_data || !MIDGARD_IS_DBUS(user_data))
		return;

	MidgardDbus *mbus = MIDGARD_DBUS(user_data);

	if (mbus->priv->message)
		g_free(mbus->priv->message);

	mbus->priv->message = g_strdup(msg);
	
	g_signal_emit(mbus, MIDGARD_DBUS_GET_CLASS(mbus)->signal_notified, 0, mbus->priv->message);
}
#endif

/* API */

/**
 * midgard_dbus_is_enabled:
 *
 * Returns: %TRUE if Midgard library is compiled with DBUS support, %FALSE otherwise
 *
 * Since: 10.05.1
 */
gboolean
midgard_dbus_is_enabled (void)
{
#ifdef MGD_HAVE_DBUS
	return TRUE;
#endif
	return FALSE;
}

static void __unref_object(gpointer data, GClosure *closure)
{
	if (data == NULL)
		return;

	if (!G_IS_OBJECT(data))
		return;

	g_object_unref(G_OBJECT(data));
}

/**
 * midgard_dbus_new:
 * @mgd: #MidgardConnection instance
 * @path: a path at which D-Bus object exists
 * @use_session: whether to use session bus
 * 
 * @use_session, set to FALSE for session bus, TRUE for system one
 *
 * Returns: new #MidgardDbus instance, %NULL otherwise
 */
MidgardDbus *
midgard_dbus_new(MidgardConnection *mgd, const gchar *path, gboolean use_session)
{
#ifdef MGD_HAVE_DBUS

	g_assert(path != NULL);
	g_assert(mgd != NULL);

	DBusGConnection *bus = __get_bus(use_session);

	if (!bus) {

		g_debug("D-Bus bus not initialized");
		return NULL;
	}

	gchar *new_path = __modify_path(mgd, path);
	
	if(!new_path)
		return NULL;

	MidgardDbus *self = g_object_new(MIDGARD_TYPE_DBUS, NULL);

	if(self == NULL)
		return NULL;

	self->priv->path = g_strdup((gchar *)new_path);
	self->priv->mgd = mgd;	

	DBusGProxy *remote_object = __get_bus_proxy(mgd, bus, path);
	
	g_free(new_path);

	/* Increase reference count */
	g_object_ref(self);	

	/* Add signal and connect to it */
	dbus_g_proxy_add_signal(remote_object, "Notified", G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(remote_object, "Notified", G_CALLBACK (notified_signal_handler), self, __unref_object);

	return self;
#else 
	return NULL;
#endif
}

/**
 * midgard_dbus_create_dbus:
 *
 * Constructor. Invokes midgard_dbus_new().
 * This function is added for language bindings, in which, midgard_dbus_new can not be 
 * invoked explicitly.
 * 
 * Returns: (transfer full): #MidgardDbus instance 
 *
 * Since: 10.05.1
 */
MidgardDbus *
midgard_dbus_create_dbus (MidgardConnection *mgd, const gchar *path, gboolean use_session)
{
	return midgard_dbus_new (mgd, path, use_session);
}

#ifdef MGD_HAVE_DBUS
DBusGProxy*
__get_bus_proxy(MidgardConnection *mgd, DBusGConnection *bus, const gchar *path)
{
	DBusGProxy *remote_object;
	GError *error = NULL;

	g_assert(mgd != NULL);
	g_assert(path != NULL);

	gchar *new_path = __modify_path(mgd, path);

	if(!new_path)
		return NULL;

	remote_object = dbus_g_proxy_new_for_name(bus,
			MIDGARD_DBUS_SERVICE,
			new_path,
			MIDGARD_DBUS_IFACE);

	g_free(new_path);

	if(error) {

		g_error_free(error);
		return NULL;
	}

	g_debug("New DBus proxy: %s, %s, %s",
			dbus_g_proxy_get_bus_name(remote_object),
			dbus_g_proxy_get_path(remote_object),
			dbus_g_proxy_get_interface(remote_object));

	return remote_object;
}
#endif

/**
 * midgard_dbus_send:
 * @mgd: #MidgardConnection instance
 * @path: dbus path at which we expect recipients
 * @message: a message to be sent
 * @use_session: whether to use system or session bus
 *
 * Emits 'Notified' signal on objects at given @path and sends given message. 
 *
 */
void 
midgard_dbus_send(MidgardConnection *mgd, const gchar *path, const gchar *message, gboolean use_session)
{
#ifdef MGD_HAVE_DBUS

	DBusGConnection *bus = __get_bus(use_session);
	
	if (!bus) 
		return;
 
	DBusGProxy *_bus_proxy = __get_freedesktop_proxy(bus);

	GError *error = NULL;
	guint _request_name_result;

	if (!dbus_g_proxy_call (_bus_proxy, "RequestName", &error,
				G_TYPE_STRING, MIDGARD_DBUS_IFACE,
				G_TYPE_UINT, 0,
				G_TYPE_INVALID,
				G_TYPE_UINT, &_request_name_result,
				G_TYPE_INVALID)) {

		g_debug("RequestName call failed");
		return;
	}

	MidgardDbus *mbus = NULL;	

	gchar *new_path = __modify_path(mgd, path);

	/* Check if object is already registered. If it is reuse.
	   In any other case, error will be thrown. */
	mbus = MIDGARD_DBUS(dbus_g_connection_lookup_g_object(bus, new_path));
	if (!mbus) {
		
		mbus = g_object_new(MIDGARD_TYPE_DBUS, NULL);
		dbus_g_connection_register_g_object (bus, new_path, G_OBJECT (mbus));
	}

	mbus->priv->mainloop = g_main_loop_new(NULL, FALSE);

	DBusGProxy *mp = __get_bus_proxy(mgd, bus, path);

	g_free(new_path);

	/* Asynchronous call, then start running main loop to make sure signal is emited */
	dbus_g_proxy_call_no_reply (mp, "emitNotified", G_TYPE_STRING, message, G_TYPE_INVALID);
	g_main_loop_run (mbus->priv->mainloop);
	
#endif
	return;
}
 
void 
midgard_core_dbus_send_serialized_object(MidgardObject *object, const gchar *path)
{
#ifdef MGD_HAVE_DBUS
	g_assert(object != NULL);
	g_assert(path != NULL);

	gchar *xml = midgard_replicator_serialize(G_OBJECT(object));

	midgard_dbus_send(MGD_OBJECT_CNC(object), path, xml, FALSE);

	g_free(xml);
#endif
	return;
}

/**
 * midgard_dbus_get_message:
 * @self: #MidgardDBus instance
 *
 * Get message associated with givven instance.
 * 
 * Returns: pointer to object's message or %NULL
 */
const gchar*
midgard_dbus_get_message(MidgardDbus *self)
{
#ifdef MGD_HAVE_DBUS
	g_assert(self != NULL);

	return self->priv->message;
#else
	return NULL;
#endif
}

extern const gchar*
midgard_dbus_get_name(void)
{
	return NULL;
}

/* GOBJECT ROUTINES */

static void 
_midgard_dbus_finalize(GObject *object)
{
	g_assert(object != NULL);

#ifdef MGD_HAVE_DBUS
	MidgardDbus *self = (MidgardDbus *) object;

	if(self->priv->path != NULL)
		g_free(self->priv->path);
	self->priv->path = NULL;
	
	if(self->priv->fullpath != NULL)
		g_free(self->priv->fullpath);
	self->priv->fullpath = NULL;
	
	if(self->priv->message != NULL)
		g_free(self->priv->message);
	self->priv->message = NULL;

	self->priv->mgd = NULL;

        if (self->priv->mainloop && g_main_loop_is_running(self->priv->mainloop))
		g_main_loop_quit(self->priv->mainloop);

	if (self->priv->mainloop)
		g_main_loop_unref(self->priv->mainloop);

	self->priv->mainloop = NULL;

	g_free(self->priv);
	self->priv = NULL;
#endif
}

static void 
_midgard_dbus_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(g_class);
	MidgardDbusClass *klass = MIDGARD_DBUS_CLASS(g_class);
	
	gobject_class->finalize = _midgard_dbus_finalize;

	klass->signal_notified = 
		g_signal_new("notified",
				G_TYPE_FROM_CLASS(g_class),
				G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
				G_STRUCT_OFFSET (MidgardDbusClass, notified),
				NULL, /* accumulator */
				NULL, /* accu_data */
				g_cclosure_marshal_VOID__STRING,
				G_TYPE_NONE, 1, G_TYPE_STRING);

#ifdef MGD_HAVE_DBUS
	dbus_g_object_type_install_info(MIDGARD_TYPE_DBUS, &dbus_glib_midgard_dbus_object_info);
#endif
}

static void 
_midgard_dbus_instance_init(
		GTypeInstance *instance, gpointer g_class)
{
	MidgardDbus *self = MIDGARD_DBUS(instance);

#ifdef MGD_HAVE_DBUS	

	self->priv = g_new(MidgardDbusPrivate, 1);
	self->priv->path = NULL;
	self->priv->fullpath = NULL;
	self->priv->message = NULL;
	self->priv->mainloop = NULL;
#endif
}

GType 
midgard_dbus_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardDbusClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_dbus_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardDbus),
			0,              /* n_preallocs */
			(GInstanceInitFunc) _midgard_dbus_instance_init/* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT,
				"MidgardDbus",
				&info, 0);
	}
	
	return type;
}
