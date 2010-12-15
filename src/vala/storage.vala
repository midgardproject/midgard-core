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

using GLib;

namespace MidgardCR {

	public interface QueryManager : GLib.Object {

	}

	/* FIXME, improve it so we can catch exception in one step */
	public interface Transaction : GLib.Object, Executable {

		public abstract string name { get; construct; }

		public abstract bool begin ();
		public abstract bool rollback ();
		public abstract bool get_status ();
	}

	public interface StorageManagerPool : GLib.Object {
		
		public abstract void register_manager_type (string classname);
		public abstract StorageManager create_manager (string classname, string name);
		public abstract string[]? list_managers();
		public abstract StorageManager? get_manager_by_name (string name);
	}

	public errordomain StorageManagerError {
		ACCESS_DENIED,
		NAME_INVALID,
		NOT_OPENED,
		INTERNAL
	}

	public interface StorageManager : GLib.Object {

		/* properties */
		public abstract StorageContentManager 	content_manager 	{ get; }
		public abstract StorageModelManager   	model_manager   	{ get; }
		public abstract Profiler              	profiler        	{ get; }
		public abstract Transaction           	transaction     	{ get; }
		public abstract StorageWorkspaceManager	workspace_manager	{ get; }	

		/* signals */
		public abstract signal void opened ();
		public abstract signal void closed ();
		public abstract signal void operation_start ();
		public abstract signal void operation_end ();	
		//public abstract signal void lost-provider (); 

		/* connection methods */
		public abstract bool open () throws StorageManagerError;
		public abstract bool close () throws StorageManagerError; 
		
		public abstract bool initialize_storage () throws StorageManagerError;

		/* FIXME, add Clonable interface ? */
		public abstract StorageManager? fork ();
		public abstract StorageManager? clone ();
	}

	public interface StorageWorkspaceManager : StorageManager {

		public abstract WorkspaceStorage workspace { get; set; }
	}

	public interface StorageExecutor : Executable {
		
		/* methods */
		public abstract bool exists ();
	 	public abstract void prepare_create () throws ValidationError;
		public abstract void prepare_update () throws ValidationError;
		public abstract void prepare_save () throws ValidationError;
		public abstract void prepare_remove () throws ValidationError;
		public abstract void prepare_purge () throws ValidationError;
	}

	public interface StorageModelManager : Model, StorageExecutor {

		/* properties */
		public abstract unowned StorageManager   storagemanager   { get; } 

		/* methods */
		public abstract StorageModel create_storage_model (ObjectModel object_model, string location);
		public abstract unowned StorageModel[]? list_storage_models ();	
		public abstract unowned ObjectModel[]? list_object_models ();
	}

	public errordomain StorageContentManagerError {

		OBJECT_INVALID,
		OBJECT_DUPLICATE,
		OBJECT_NOT_EXISTS,
		MODEL_INVALID,
		INTERNAL
	}

	public interface StorageContentManager : GLib.Object {

		/* properties */
		public abstract unowned NamespaceManager namespace_manager { get; }

		/* per object methods */
		public abstract bool exists (Storable object);
		public abstract void create (Storable object) throws StorageContentManagerError;
		public abstract void update (Storable object) throws StorageContentManagerError;
		public abstract void save (Storable object) throws StorageContentManagerError; 
		public abstract void remove (Storable object) throws StorageContentManagerError;
		public abstract void purge (Storable object) throws StorageContentManagerError;

		public abstract unowned StorageManager get_storage_manager ();
	} 

	public errordomain StorageModelError {
		STORAGE_INVALID,
		STORAGE_EXISTS,
		INTERNAL
	}

	/* Initialized for every given class name */
	public interface StorageModel : Model, StorageExecutor {
	
		/* properties */
		public abstract string location { get; set; }		

		/* methods */
		public abstract unowned StorageManager get_storage_manager ();
	}

	/* Initialized for every given property name */
	public interface StorageModelProperty : StorageExecutor, StorageModel, ModelProperty {

		/* properties */
		public abstract bool index { get; set; }
		public abstract bool primary { get; set; }	
	}	
}
