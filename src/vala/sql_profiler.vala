
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
