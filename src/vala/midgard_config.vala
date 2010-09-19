
using MidgardCRCore;

namespace MidgardCR {

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
			this._confdir = MidgardCRCore.Config.get_default_confdir ();
		}

		/* methods */
		public bool read_configuration (string name, bool user) throws GLib.KeyFileError, GLib.FileError {
			if (!MidgardCRCore.Config.read_file (this, name, user))
				return false;

			this._configname = name;
			return true;
		}
	
		public bool read_configuration_at_path (string path) throws GLib.KeyFileError, GLib.FileError {
			if (MidgardCRCore.Config.read_file_at_path (this, path))
				return true;
			
			return false;
		}

		public bool read_data (string data) throws GLib.KeyFileError, GLib.FileError {
			if (!MidgardCRCore.Config.read_data (this, data))
				return false;

			this._configname = ""; /* FIXME, do we need unique string or filename? */
			return true;
		}

		public bool save_configuration (string name, bool user) throws GLib.KeyFileError, GLib.FileError {
			bool saved = MidgardCRCore.Config.save_file (this, name, user);
			return saved;
		}

		public bool save_configuration_at_path (string path) throws GLib.KeyFileError, GLib.FileError {
			if (MidgardCRCore.Config.save_file_at_path (this, path))
				return true;

			return false;
		}

		public static string[]? list_files (bool user) throws GLib.FileError {
			string[] names = MidgardCRCore.Config.list_files (user);	
			return names;
		}

		public MidgardCR.Config copy () {

			MidgardCR.Config dest = new MidgardCR.Config ();
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
