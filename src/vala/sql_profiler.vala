/* 
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
 * 
 * Copyright (C) 2010, 2011 Piotr Pokora <piotrek.pokora@gmail.com>
 */

using GLib;

namespace MidgardCR {

	/** 
	 * {@link Profiler} implementation for SQL database.
	 * 
	 * SQLProfiler provides information about SQL queries being executed and
	 * the time spent on execution.
	 *
	 * The example below demonstrates simple profiler's usage. In callback function, 
	 * on execution start, profiler starts it's timer, and stops it on execition end.
	 * 
	 * {{{
	 *   void profiler_callback_start (SQLProfiler profiler)
	 *   {
	 *      profiler.start ();
	 *   }
	 *
	 *   void profiler_callback_end (SQLProfiler profiler)
	 *   {
	 *      profiler.end ();
	 *      GLib.print ("SQL QUERY: %s (%.04f) \n", profiler.command, profiler.time);
	 *   }
	 *
	 * 
	 *   SQLProfiler profiler = (SQLProfiler) storage_manager.profiler;
	 *   profiler.enable (true);
	 *   storage_manager.operation_start.connect (() => {
	 *      profiler_callback_start (profiler);
	 *   });
	 * 
	 *   storage_manager.operation_end.connect (() => {
	 *      profiler_callback_end (profiler);
	 *   }); 
	 * }}}
	 *
	 */
	public class SQLProfiler : GLib.Object, Profiler {

		/* internal properties */

		internal string _command = null;
		internal bool _enabled = false;
		internal bool _running = false;
		internal GLib.Timer _timer = null;

		/* public properties */

		/**
		 * Returns the time elapsed from the last start() invocation.
		 */
		public double time { 
			get { return this._timer.elapsed (null); }
		}

		/**
		 * Returns the SQL query command executed recently.
		 *
		 * The command is set internally by {@link SQLStorageManager}
		 */
		public string command { 
			get { return this._command; }
		}

		/* methods */

		construct {
			this._timer = new GLib.Timer ();
		}

		/**
		 * Enable or disable profiler.
		 */
		public void enable (bool toggle) {
			this._enabled = toggle;
		}

		/**
		 * Starts profiler.
		 *
		 * Starts profiler's timer and mark profiler as running.
		 *
		 * This method silently returns, if profiler is running or disabled.
		 * @see enable
		 */
		public void start () {
			if (this._enabled == false)
				return;
			if (this._running == true)
				return;
			this._running = true;
			this._timer.start ();
		}

		/**
		 * Stops profiler.
		 *
		 * Stops profiler's timer and mark it as not running.
		 * 
		 * This method silently returns, if profiler is not running or disabled.
		 * @see enable
		 * @see start
		 */
		public void end () {
			if (this._enabled == false)
				return;
			if (this._running == false)
				return;
			this._running = false;
			this._timer.stop ();

		}
	}
}
