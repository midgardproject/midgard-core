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
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
 */

using GLib;

namespace MidgardCR {

	/**
	 * Provides information about executed commands.
	 *
	 * Minimal Profiler implementation shall provide information about executed command:
	 * the time spent on execution and the command itself. Command might be SQL query, 
	 * directory, file or xml node creation.
	 *
	 * Profiler initialization shall be done with {@link StorageManager}.
	 * The easiest way to get information about commands is to connect profiler's 
	 * callback to {@link Executable} signals.
	 */
	public interface Profiler : GLib.Object {

		/* properties */

		/**
		 * The spent to execute any command.
		 */
		public abstract double time { get; }

		/**
		 * The command being executed.
		 */
		public abstract string command { get; }

		/**
		 * Enable or disable profiler.
		 */
		public abstract void enable (bool toggle);

		/**
		 * Start profiler.
		 * 
		 * Implementation shall (at least) reset profiler's time and start timer . 
		 */
		public abstract void start ();

		/**
		 * Stop profiler.
		 *
		 * Implementation shall (at least) keep profiler's times unchanged and stop timer.
		 */
		public abstract void end ();
	}
}
