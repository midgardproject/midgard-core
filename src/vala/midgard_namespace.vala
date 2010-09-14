using GLib;

namespace Midgard {

	errordomain NamespaceManagerError {
		URI_INVALID,
		URI_EXISTS,
		ALIAS_INVALID,
		ALIAS_EXISTS
	}

	public interface NamespaceManager : GLib.Object {

		/* methods */
		public abstract bool create_uri (string uri, string name) throws NamespaceManagerError;
		public abstract string[]? list_names ();
		public abstract string get_uri_by_name (string name);
		public abstract string get_name_by_uri (string uri);
		public abstract bool name_exists ();
		public abstract bool uri_exists ();
	}
}
