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
 * Copyright (C) 2010, 2011 Piotr Pokora <piotrek.pokora@gmail.com>
 */

namespace MidgardCR {

	public class RDFSQLModelManager : SQLModelManager {

		/* internal properties */
		//internal unowned RDFSQLStorageManager _storage_manager = null;	
		internal RDFMapperObject[]? _mapper_objects = null;

		/* Constructor */

		/**
		 * Creates new RDFSQLModelManager for given storage manager.
		 */
		public RDFSQLModelManager (RDFSQLStorageManager manager) {
			Object (storagemanager: manager);
		}
	
		/* Model methods */
		/**
                 * Associate new model 
                 *
                 * @param model {@link Model} to add
                 *
                 * @return {@link Model} instance (self reference)
                 */
		public override Model add_model (Model model) {
			if (model is RDFMapperObject) 
				this._mapper_objects += (RDFMapperObject) model;
			else 
				base.add_model (model);
			return this;
		}
				
		/**
		 * Find {@link RDFMapperObject} model by given name. 
		 * 
                 * @param name {@link Model} name to look for
                 *
                 * @return {@link RDFMapperObject} model if found, null otherwise
                 */
                public override unowned Model? get_model_by_name (string name) {
			unowned Model model = this._find_model_by_name ((Model[])this._mapper_objects, name);
			return model;
		}

		private unowned Model? _find_model_by_name (Model[] models, string name) {
			if (models == null)
				return null;

			foreach (unowned Model model in models) {
				if (model.name == name)
					return model;
			}
			return null;
		}


		/**
		 * {@inheritDoc} 
		 */
                public override void is_valid () throws ValidationError {
			foreach (unowned Model model in ((Model[])this._mapper_objects)) {
				if (!(model is RDFMapperObject))
					throw new ValidationError.TYPE_INVALID ("Invalid '%s' class associated with model manager", model.name);
				model.is_valid ();
			}
			base.is_valid ();	
		}
	
		/**
		 * {@inheritDoc}
		 */
		public override void prepare_create () throws ValidationError {
			/* TODO, prepare rdf mappers serialization */
			base.prepare_create ();
		}

		/**
		 * {@inheritDoc}
		 */
                public override void prepare_update () throws ValidationError {
			/* TODO, prepare rdf mappers update */
			base.prepare_update ();
		}

		/**
		 * {@inheritDoc} 
		 */
                public override void prepare_save () throws ValidationError {
			/* TODO prepare rdf mappers save */
			base.prepare_save ();
		}

		/**
		 * {@inheritDoc} 
		 */
                public override void prepare_remove () throws ValidationError {
			/* TODO prepare rdf mappers remove */
		}

		/**
		 * {@inheritDoc} 
		 */
                public override void prepare_purge () throws ValidationError {
			/* TODO prepare rdf mappers purge */
			base.prepare_purge ();
		}

		/* Executable methods */
		/**
		 * {@inheritDoc}
		 */
		public override void execute () throws ExecutableError {	
			/* TODO, execute rdf mappers operations */
			base.execute ();		
		}

		/* methods */

		/**
		 * {@inheritDoc} 
		 */
		public override unowned Model[]? list_models_by_type (string type) {
			if (type == "RDFMapperObject")
				return (Model[]?) this._mapper_objects;
			return base.list_models_by_type (type);
		}
	
		/**
		 * {@inheritDoc}
		 */
		public override string[]? list_model_types () {
			string[] _types = base.list_model_types ();
			_types += "RDFMapperObject";
			return _types;
		}

		public override unowned Model? get_type_model_by_name (string type, string name) {
			if (type == "RDFMapperObject")
				return this.get_model_by_name (name);
			return base.get_type_model_by_name (type, name);
		}

		public override unowned Model[]? list_models () {
			return null;
		}
	}
}
