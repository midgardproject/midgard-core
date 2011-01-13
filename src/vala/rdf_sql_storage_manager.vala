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

	/**
	 * SQL {@link StorageManager} with RDF support
	 *
	 * RDFSQLStorageManager is designed to store objects without {@link SQLTableModel} models.
	 * It provides managers to RDF triples, which might be objects or their properties.
	 */
	public class RDFSQLStorageManager : SQLStorageManager {
	
		/* internal properties */
		internal ObjectModel _rdf_object_model = null;
		internal SQLTableModel _rdf_table_model = null;
		private RDFSQLModelManager _model_manager = null;
		private RDFSQLObjectManager _object_manager = null;

		/* public properties */

		/**
		 * Content Manager of RDFSQLContentManager type 
		 */
		public new StorageContentManager content_manager {
			get {
				if (this._content_manager == null)
					this._content_manager = new RDFSQLContentManager(this);
				return this._content_manager;
			}
		}

		/**
		 * {@link RDFSQLModelManager} model manager
		 */
		public new unowned StorageModelManager model_manager {
			get { 
				if (this._model_manager == null)
					this._model_manager = new RDFSQLModelManager (this);
				return (StorageModelManager) this._model_manager;
			}
		}

		/**
		 * {@link RDFSQLObjectManager} object manager
		 */
		public new unowned ObjectManager object_manager {
			get {
				if (this._object_manager == null)
					this._object_manager = new RDFSQLObjectManager (this);
				return (ObjectManager) this._object_manager;
			}
		} 

		/* Constructor */
		public RDFSQLStorageManager (string name, Config config) throws StorageManagerError {
			/* chain up to parent constructor */
			base (name, config);
			
			/* initialize generic RDF object and table models */ 
			this._rdf_object_model = new ObjectModel ("RDFTripleObject");
			this._rdf_object_model	
				.add_model (new ObjectPropertyModel ("classname", "string", ""))
				.add_model (new ObjectPropertyModel ("objectguid", "guid", ""))
				.add_model (new ObjectPropertyModel ("property", "string", ""))
/*				.add_model (new ObjectPropertyModel ("alias", "string", ""))*/
				.add_model (new ObjectPropertyModel ("literal", "string", ""))
				.add_model (new ObjectPropertyModel ("value", "string", ""));

			this._rdf_table_model = new SQLTableModel ((SQLStorageManager) this, "RDFTripleObject", "rdf_triple_object");
			this._rdf_table_model
				.add_model (new SQLColumnModel ((SQLStorageManager) this, "classname", "class_name", "string"))
				.add_model (new SQLColumnModel ((SQLStorageManager) this, "objectguid", "object_guid", "guid"))
				.add_model (new SQLColumnModel ((SQLStorageManager) this, "property", "property_name", "string"))
/*				.add_model (new SQLColumnModel ((SQLStorageManager) this, "alias", "property_alias", "string"))*/
				.add_model (new SQLColumnModel ((SQLStorageManager) this, "literal", "property_literal", "string"))
				.add_model (new SQLColumnModel ((SQLStorageManager) this, "value", "property_value", "string"));
		}

		public override bool initialize_storage () throws StorageManagerError {
			base.initialize_storage ();

			if (this._rdf_table_model.exists ()) 
				return true;

			this.model_manager
				.add_model (this._rdf_object_model)
				.add_model (this._rdf_table_model);
			try {
				this.model_manager.prepare_create ();
			} catch (ValidationError e) {
				if (!(e is ValidationError.NAME_DUPLICATED))
					throw new StorageManagerError.INTERNAL (e.message);
			}
			
			try {
				this.model_manager.execute ();	
			} catch (ExecutableError e) {
				throw new StorageManagerError.INTERNAL (e.message);
			}
			
			return true;	
		}

		/* methods */
	}
}
