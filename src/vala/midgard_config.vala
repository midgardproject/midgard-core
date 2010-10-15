
using MidgardCRCore;

namespace MidgardCR {

	public class Config : GLib.Object {

		/* private */
		private string _configname = "";
		private string _confdir = "";
		private string _dbtype = "SQLite";

		/* internal */
		internal uint dbtype_id = 0;
		internal GLib.KeyFile keyfile = null;

		/* public properties */
		[Description(nick = "Authtype", blurb = "The type of authentication used")]
	  	public string authtype { get; set; }
		[Description(nick = "Name", blurb = "The name of database to connect to")]
		public string dbname { get; set; }
		[Description(nick = "Password", blurb = "Password used in database connection")]
		public string dbpass { get; set; }
		[Description(nick = "Type", blurb = "The type of database to use")]
		public string dbtype { 
			get { return this._dbtype; }
			set { 
				this._dbtype = value;
				MidgardCRCore.Config.set_dbtype_id (this, this._dbtype);
			}
		}
		[Description(nick = "Username", blurb = "Username used in database connection")]
		public string dbuser { get; set; }
		[Description(nick = "Port", blurb = "Port used in database connection")]
		public uint dbport { get; set; }
		[Description(nick = "Host", blurb = "Database's host ")]
		public string host { get; set; }
		[Description(nick = "Logfile", blurb = "Location of the logfile")]
		public string logfilename { get; set; }
		[Description(nick = "Loglevel", blurb = "Log level")]
		public string loglevel { get; set; }
		[Description(nick = "MidgardPassword", blurb = "Midgard's user password")]
		public string midgardpassword { get; set; }
		[Description(nick = "MidgardUsername", blurb = "Midgard's user username")]		
		public string midgardusername { get; set; }
		[Description(nick = "ShareDir", blurb = "Directory for shared, architecture independent files")]
		public string sharedir { get; set; }
		[Description(nick = "VarDir", blurb = "Application specific directories (cache, storage files, etc)")]
		public string vardir { get; set; }
		[Description(nick = "BlobDir", blurb = "Location of the blobs directory")]
		public string blobdir { get; set; }
		[Description(nick = "CacheDir", blurb = "Directory to keep cache files")]
		public string cachedir { get; set; }
		[Description(nick = "DatabaseDir", blurb = "Database directory (e.g. for SQLite provider)")]
		public string dbdir { get; set; }
		[Description(nick = "TableCreate", blurb = "??")]
		public bool tablecreate { get; set; }
		[Description(nick = "TableUpdate", blurb = "??")]
		public bool tableupdate { get; set; }
		[Description(nick = "TestUnit", blurb = "??")]
		public bool testunit { get; set; }

		/* constructor */
		construct {
			/* fallback to default local directories */
			this.sharedir = "/usr/local/share";
			this.blobdir = "/var/local/lib/midgard/blobs";
			this.cachedir = "/var/local/cache/midgard";
			this.vardir = "/var/local/";
			this.dbdir = "";
			this.dbtype_id = 0; /* Satisfy valac */
			this.keyfile = null;
			/* generate internal dbtype id */
			MidgardCRCore.Config.set_dbtype_id (this, this._dbtype);
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

		public static string[]? list_configurations (bool user) throws GLib.FileError {
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
