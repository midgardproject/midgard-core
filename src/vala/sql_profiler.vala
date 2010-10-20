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

	public class SQLProfiler : GLib.Object, Profiler {

		/* internal properties */

		internal string _command = null;
		internal bool _enabled = false;
		internal bool _running = false;
		internal GLib.Timer _timer = null;

		/* public properties */

		public double time { 
			get { return this._timer.elapsed (null); }
		}

		public string command { 
			get { return this._command; }
		}

		/* methods */

		construct {
			this._timer = new GLib.Timer ();
		}

		public void enable (bool toggle) {
			this._enabled = toggle;
		}

		public void start () {
			if (this._enabled == false)
				return;
			if (this._running == true)
				return;
			this._running = true;
			this._timer.start ();
		}

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
