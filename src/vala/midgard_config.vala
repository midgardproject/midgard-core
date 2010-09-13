
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

		public uint dbport { get; set; }

		public string host { get; set; }

		public string logfilename { get; set; }

		public string loglevel { get; set; }

		public string midgardpassword { get; set; }

		public string midgardusername { get; set; }

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
	
		public bool read_file_at_path (string path) throws GLib.KeyFileError, GLib.FileError {
			GLib.warning ("FIXME");
			return false;
			//return MidgardCore.Config.read_file_at_path (name, user);
		}

		public bool save_file (string name, bool user) throws GLib.KeyFileError, GLib.FileError {
			GLib.warning ("FIXME");
			return false;
		}

		public static string[]? list_files (bool user) {
			GLib.warning ("FIXME");
			return null;
		}

		public Midgard.Config copy () {

			Midgard.Config dest = new Midgard.Config ();
			dest.authtype = this.authtype;
			dest.dbname = this.dbname;
			dest.dbpass = this.dbpass;
			dest.dbtype = this.dbtype;
			dest.dbuser = this.dbuser;
			dest.host = this.host;
			dest.logfilename = this.logfilename;
			dest.loglevel = this.loglevel;
			dest.midgardpassword = this.midgardpassword;
			dest.midgardusername = this.midgardusername;
			dest.dbport = this.dbport;
			dest.sharedir = this.sharedir;
			dest.vardir = this.vardir;
			dest.blobdir = this.blobdir;
			dest.cachedir = this.cachedir;
			dest.dbdir = this.dbdir;
			dest.tablecreate = this.tablecreate;
			dest.tableupdate = this.tableupdate;
			dest.testunit = this.testunit;

			dest._configname = this._configname;

			/* FIXME, copy log file GIOChannel */

			return dest;
		}
	}

	static void main () {}
}
