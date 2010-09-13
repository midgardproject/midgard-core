
using MidgardCore;

namespace Midgard {

	public class Config : GLib.Object {

		/* private */
		private string _configname = "";
		private string _confdir = "";

		/* internal */
		internal uint dbtype_id = 0;
		internal GLib.KeyFile keyfile = null;

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

		/* constructor */
		construct {
			/* fallback to default local directories */
			this.sharedir = "/usr/local/share";
			this.blobdir = "/var/local/lib/midgard/blobs";
			this.cachedir = "/var/local/cache/midgard";
			this.vardir = "/var/local/";
			this.dbdir = "";
			/* generate path to default system-wide configuration directory */
			this._confdir = MidgardCore.Config.get_default_confdir ();
		}

		/* methods */
		public bool read_file (string name, bool user) throws GLib.KeyFileError, GLib.FileError {
			try {
				MidgardCore.Config.read_file (this, name, user);
				this._configname = name;
				return true;
			} catch (GLib.KeyFileError e) {
				throw e;
			} catch (GLib.FileError e) {
				throw e;
			} 
		}
	
		public bool read_file_at_path (string path) throws GLib.KeyFileError, GLib.FileError {
			try {
				MidgardCore.Config.read_file_at_path (this, path);
				this._configname = path; /* FIXME, do we need unique string or filename? */
				return true;
			} catch (GLib.KeyFileError e) {
				throw e;
			} catch (GLib.FileError e) {
				throw e;
			} 
		}

		public bool read_data (string data) throws GLib.KeyFileError, GLib.FileError {
			try {
				MidgardCore.Config.read_data (this, data);
				this._configname = ""; /* FIXME, do we need unique string or filename? */
				return true;
			} catch (GLib.KeyFileError e) {
				throw e;
			} 
		}

		public bool save_file (string name, bool user) throws GLib.KeyFileError, GLib.FileError {
			try {
				MidgardCore.Config.save_file (this, name, user);
				return true;
			} catch (GLib.KeyFileError e) {
				throw e;
			} catch (GLib.FileError e) {
				throw e;
			}
		}

		public static string[]? list_files (bool user) throws GLib.FileError {
			try {
				string[] names = MidgardCore.Config.list_files (user);
				return names;
			} catch (GLib.FileError e) {
				throw e;
			}
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
