
using MidgardCore;

namespace Midgard {

	public class Config : GLib.Object {

		/* private */
		private string _authtype;
		private string _dbname;
		private string _dbpass;
		private string _dbtype;
		private string _dbuser;
		private string _host;
		private string _logfilename;
		private string _loglevel;
		private string _midgardpassword;
		private string _midgardusername;	
		private uint _port;
		private string _sharedir;
		private string _vardir;
		private string _blobdir;
		private string _cachedir;
		private string _dbdir;                
		private bool _tablecreate;
		private bool _tableupdate;
		private bool _testunit;

		/* public properties */
	  	public string authtype {
			get { return this._authtype; }
			set { this._authtype = value; }
		}

		public string dbname {
			get { return this._dbname; }
			set { this._dbname = value; }
		}

		public string dbpass {
			get { return this._dbpass; }
			set { this._dbpass = value; }
		}
	
		public string dbtype {
			get { return this._dbtype; }
			set { this._dbtype = value; }
		}	

		public string dbuser {
			get { return this._dbuser; }
			set { this._dbuser = value; }
		}	

		public string host {
			get { return this._host; }
			set { this._host = value; }
		}	

		public string logfilename {
			get { return this._logfilename; }
			set { this._logfilename = value; }
		}	

		public string loglevel {
			get { return this._loglevel; }
			set { this._loglevel = value; }
		}	

		public string midgardpassword {
			get { return this._midgardpassword; }
			set { this._midgardpassword = value; }
		}	

		public string midgardusername {
			get { return this._midgardusername; }
			set { this._midgardusername = value; }
		}	

		public uint port {
			get { return this._port; }
			set { this._port = value; }
		}	

		public string sharedir {
			get { return this._sharedir; }
			set { this._sharedir = value; }
		}	

		public string vardir {
			get { return this._vardir; }
			set { this._vardir = value; }
		}	

		public string blobdir {
			get { return this._blobdir; }
			set { this._blobdir = value; }
		}	

		public string cachedir {
			get { return this._cachedir; }
			set { this._cachedir = value; }
		}	

		public string dbdir {
			get { return this._dbdir; }
			set { this._dbdir = value; }
		}	

		public bool tablecreate {
			get { return this._tablecreate; }
			set { this._tablecreate = value; }
		}	

		public bool tableupdate {
			get { return this._tableupdate; }
			set { this._tableupdate = value; }
		}	

		public bool testunit {
			get { return this._testunit; }
			set { this._testunit = value; }
		}

		/* methods */
		public bool read_file (string name, bool user) throws GLib.KeyFileError, GLib.FileError {
			
			return MidgardCore.Config.read_file (this, name, user);
		}
	
		/*public bool read_file_at_path (string path, bool user) throws GLib.KeyFileError, GLib.FileError {
			
			return MidgardCore.Config.read_file_at_path (name, user);
		}*/
	}

	static void main () {}
}
