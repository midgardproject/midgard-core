/*
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_executable.h"

static guint execution_start_signal = 0;
static guint execution_end_signal = 0;

/**
 * midgard_executable_error_quark: (skip)
 *
 * Returns: MIDGARD_EXECUTION_ERROR quark
 */
GQuark
midgard_execution_error_quark (void)
{
	static GQuark q = 0;
	if (q == 0)
		q = g_quark_from_static_string ("midgard-execution-error-quark");
	return q;
}

/**
 * midgard_executable_execute:
 * @self: #MidgardExecutable instance
 * @error (error-domains MIDGARD_EXECUTION_ERROR): location to store error
 *
 * Execute command or perform operation.
 * Implementation shall check if given instance is valid.
 * If it's not, shall invoke validation method, if given instance is #MidgardValidable derived.
 *
 * Before execution, implementation should emit 'execution-start' signal, and 'execution-end' when 
 * execution operation is completed.
 *
 * Since: 10.05.5
 */
void
midgard_executable_execute (MidgardExecutable *self, GError **error)
{
	if (MIDGARD_EXECUTABLE_GET_INTERFACE (self)->execute == NULL) {
		g_set_error (error, MIDGARD_EXECUTION_ERROR, MIDGARD_EXECUTION_ERROR_INTERNAL,
				"%s class doesn't implement execute() method", G_OBJECT_TYPE_NAME (self));
	}

	MIDGARD_EXECUTABLE_GET_INTERFACE (self)->execute (self, error);
}

/**
 * midgard_executable_execute_async:
 * @self: #MidgardExecutable instance
 * @error (error-domains MIDGARD_EXECUTION_ERROR): location to store error
 *
 * Execute asynchronous command or perform operation.
 * Implementation shall check if given instance is valid.
 * If it's not, shall invoke validation method, if given instance is #MidgardValidable derived.
 *
 * Implementation shall not emit any signal inside asynchronous method. 
 * Instead, signal emission should be added to GLib's default main loop(e.g. g_idle_add).
 * 
 * Since: 12.09
 */
void
midgard_executable_execute_async (MidgardExecutable *self, GError **error)
{
	if (MIDGARD_EXECUTABLE_GET_INTERFACE (self)->execute_async == NULL) {
		g_set_error (error, MIDGARD_EXECUTION_ERROR, MIDGARD_EXECUTION_ERROR_INTERNAL,
				"%s class doesn't implement execute_async() method", G_OBJECT_TYPE_NAME (self));
	}

	MIDGARD_EXECUTABLE_GET_INTERFACE (self)->execute_async (self, error);
}
/**
 * midgard_executable_execution_start:
 * @self: #MidgardExecutable instance
 *
 * Emits 'execution-start' signal on given executable.
 *
 * Since: 12.09
 */
void
midgard_executable_execution_start (MidgardExecutable *self)
{
	g_return_if_fail (MIDGARD_IS_EXECUTABLE (self));
	g_signal_emit (self, execution_start_signal, 0);
}

static gboolean
execution_end_func (gpointer data)
{
	MidgardExecutable *executable = (MidgardExecutable *) data;
	g_signal_emit (executable, execution_end_signal, 0);	
	return FALSE;
}

/**
 * midgard_executable_execution_end:
 * @self: #MidgardExecutable instance
 *
 * Emits 'execution-end' signal on given executable.
 *
 * Since: 12.09
 */
void
midgard_executable_execution_end (MidgardExecutable *self)
{
	g_return_if_fail (MIDGARD_IS_EXECUTABLE (self));
	g_signal_emit (self, execution_end_signal, 0);
	//g_idle_add ((GSourceFunc) execution_end_func, self);
}

static void
midgard_executable_class_init (gpointer g_class)
{
	static gboolean initialized = FALSE;

	static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
	g_static_mutex_lock (&mutex);

	if (initialized == TRUE) {
		g_static_mutex_unlock (&mutex);
		return;
	}

	/**
	 * MidgardExecutable::execution-start:
	 * @executor: the #MidgardExecutable
	 *
	 * Gets emitted before @executor's operation is executed
	 **/
	execution_start_signal = g_signal_new("execution-start",
			MIDGARD_TYPE_EXECUTABLE,
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (MidgardExecutableIFace, execution_start),
			NULL, /* accumulator */
			NULL, /* accu_data */
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE,
			0);
	/**
	 * MidgardExecutable::execution-end:
	 * @executor: the #MidgardExecutable
	 *
	 * Gets emitted after @executor's operation is executed
	 **/
	execution_end_signal = g_signal_new("execution-end",
			MIDGARD_TYPE_EXECUTABLE,
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (MidgardExecutableIFace, execution_end),
			NULL, /* accumulator */
			NULL, /* accu_data */
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE,
			0);

	initialized = TRUE;

	g_static_mutex_unlock (&mutex);
}

GType
midgard_executable_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardExecutableIFace),
			(GBaseInitFunc) midgard_executable_class_init,
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardExecutable", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}
	return type;
}
