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

namespace MidgardCR {

	public class RDFGenericObject : RepositoryObject {

		/* internal properties */
		private string _rdf_identifier = null;
		internal string _classname = null;
		internal RepositoryObject[]? _triples = null;
		internal RepositoryObject? _decorated_object = null;

		/* properties */

		public RepositoryObject? repository_object {
			get { return this._decorated_object; }
			construct { this._decorated_object = value; }
		}	
	
		public new string identifier {
			get {
				if (this._rdf_identifier == null) {
					this._rdf_identifier = "midgard:guid:" + this.guid;
				}

				return this._rdf_identifier;
			}
			set { this._identifier = value; } /* TODO: do we need to "denamespace" this? */
		}

		public string classname {
			get { return this._classname; }
			construct { this._classname = value; }
		}

		/* constructor */
		/** 
		 * Create new RDFGenericObject instance
		 *
		 * @param classname Valid RDF statement. For example, 'foaf:Person' or uri which identifies statement. 
		 * @param object, optional {@link RepositoryObject} object 
		 * @param guid, optional guid identifying object
		 * 
		 * If object argument is not null, created instance is a decorator for given one.
		 * Application is responsible to set correct decorated object. 
		 * If unsure, {@link RDFSQLObjectManager} should be involved in object creation.
		 * If given object argument's value is null, rdf object will hold set of triples
		 * objects to manage its properties.
		 * 
		 * If given guid is not null, object's data will be fetch from SQL database.
		 * Otherwise, guid will be set internally, so there's no need to pass guid argument, 
		 * if object doesn't have to be fetched from storage.
		 */ 
		public RDFGenericObject (string classname, RepositoryObject? object = null, string? guid = null) {
			Object (classname: classname, repository_object: object, guid: guid);	
		}
		
		/* private methods */
		/* Return array of triples. This is done to allow arrays as returned values */
		private RepositoryObject[]? _find_triples_by_name (string name) {
			if (this._triples ==null)
				return null;
			RepositoryObject[]? found_triples = null;
			foreach (unowned RepositoryObject triple in this._triples) {
				string pname;
				triple.get ("property", out pname);
				if (pname == name)
					found_triples += triple;
			}
			return found_triples;
		}

		/* public methods */
		public new void set_property_value (string name, GLib.Value value) {
			RepositoryObject[]? triples = this._find_triples_by_name (name);
			RepositoryObject triple = null;
			/* FIXME, handle returned array */
			if (triples == null) {
				var builder = new ObjectManager ();
				try {
	                       		triple = builder.factory ("RDFTripleObject") as RepositoryObject; 
				} catch ( ValidationError e) {
					/* Do nothing */
				} catch (ObjectManagerError e) {
					/* Do nothing */
				}
				this._triples += triple;
			} else {
				triple = triples[0];
			}
                        triple.set (
                               	"objectguid", this.guid,
                               	"identifier", this.identifier,
                               	"classname",  this.classname,
                               	"property",   name,
                               	"literal",    "",
                               	"value",      (string) value
                       	);
		}

		public virtual void set_property_literal (string name, string value) {	
			RepositoryObject[]? triples = this._find_triples_by_name (name);
			RepositoryObject triple = null;
			/* FIXME, handle returned array */
			if (triples == null) {
				var builder = new ObjectManager ();
	                       	try {
					triple = builder.factory ("RDFTripleObject") as RepositoryObject; 
				} catch ( ValidationError e) {
					/* Do nothing */
				} catch (ObjectManagerError e) {
					/* Do nothing */
				}
				this._triples += triple;
			} else {
				triple = triples[0];
			}
                        triple.set (
                               	"objectguid", this.guid,
                               	"identifier", this.identifier,
                               	"classname",  this.classname,
                               	"property",   name,
                               	"literal",    value,
                               	"value",      ""
                       	);
		}

		public new GLib.Value? get_property_value (string name) {
			/* TODO, if required throw exception */
			if (name == null || name == "") 
				return null;
			RepositoryObject[]? triples = this._find_triples_by_name (name);
			if (triples == null) {
				return null;
			}
			GLib.Value val = "";
			triples[0].get_property ("value", ref val);
			return val;
		}

		public virtual string? get_property_literal (string name) {
			/* TODO, if required throw exception */
			if (name == null || name == "") 
				return null;
			RepositoryObject[]? triples = this._find_triples_by_name (name);
			if (triples == null)
				return null;
			string val;
			triples[0].get ("literal", out val);
			return val;	
		}

		public override string[]? list_all_properties () {
			if (this._triples == null)
				return null;
			string[] propnames = null;
			foreach (unowned RepositoryObject triple in this._triples) {
				string name;
				triple.get ("property", out name);
				propnames += name;
			}
			return propnames;
		}

		public unowned RepositoryObject[]? list_triples () {
			return this._triples;
		}

		public void add_triple (RepositoryObject triple) {
			this._triples += triple;
		}
	}
}
