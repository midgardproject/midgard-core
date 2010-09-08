
using MidgardCore;

namespace Midgard {

	public class Config : GLib.Object {

		/* private */
		private string _configname = "";
	
		/* public properties */
	  	public string authtype { get; set; }

		public string dbname { get; set; }

		public string dbpass { get; set; }
	
		public string dbtype { get; set; }

		public string dbuser { get; set; }

		public string host { get; set; }

		public string logfilename { get; set; }

		public string loglevel { get; set; }

		public string midgardpassword { get; set; }

		public string midgardusername { get; set; }

		public uint port { get; set; }

		public string sharedir { get; set; }

		public string vardir { get; set; }

		public string blobdir { get; set; }

		public string cachedir { get; set; }

		public string dbdir { get; set; }

		public bool tablecreate { get; set; }

		public bool tableupdate { get; set; }

		public bool testunit { get; set; }

		/* methods */
		public bool read_file (string name, bool user) throws GLib.KeyFileError, GLib.FileError {
			
			return MidgardCore.Config.read_file (this, name, user);
		}
	
		/*public bool read_file_at_path (string path, bool user) throws GLib.KeyFileError, GLib.FileError {
			
			return MidgardCore.Config.read_file_at_path (name, user);
		}*/

		public Midgard.Config copy () {

			Midgard.Config dest = new Midgard.Config ();
			dest._authtype = this._authtype;
			dest._dbname = this._dbname;
			dest._dbpass = this._dbpass;
			dest._dbtype = this._dbtype;
			dest._dbuser = this._dbuser;
			dest._host = this._host;
			dest._logfilename = this._logfilename;
			dest._loglevel = this._loglevel;
			dest._midgardpassword = this._midgardpassword;
			dest._midgardusername = this._midgardusername;
			dest._port = this._port;
			dest._sharedir = this._sharedir;
			dest._vardir = this._vardir;
			dest._blobdir = this._blobdir;
			dest._cachedir = this._cachedir;
			dest._dbdir = this._dbdir;
			dest._tablecreate = this._tablecreate;
			dest._tableupdate = this._tableupdate;
			dest._testunit = this._testunit;

			dest._configname = this._configname;

			/* FIXME, copy log file GIOChannel */

			return dest;
		}
	}

	static void main () {}
}
