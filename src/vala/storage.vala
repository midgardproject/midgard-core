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

using GLib;

namespace MidgardCR {

	/**
	 * Basic interface for all query related classes.
	 *
	 * QueryManager implementation shall provide access to query tools, able to 
	 * fetch from storage multiple objects or particular data using specific constraints.
	 *
	 * Check {@link ObjectManager} for single objects construction 
	 */
	public interface QueryManager : GLib.Object {

		/**
		 * Create new instance which is {@link QuerySelect} implementation.
		 */ 
		public abstract QuerySelect create_query_select (string classname);

		/**
		 * Get {@link StorageManager} associated with QueryManager.
		 */
		public abstract unowned StorageManager get_storage_manager ();
	}

	/* FIXME, improve it so we can catch exception in one step */
	/**
	 * Basic interface for storage transactions.
	 */
	public interface Transaction : GLib.Object, Executable {

		/**
		 * Begins transaction.
		 */
		public abstract void begin () throws ExecutableError;

		/**
		 * Transaction's rollback
		 */
		public abstract void rollback () throws ExecutableError;

		/**
		 * Get information about transaction's status.
		 */
		public abstract bool get_status ();

		/**
		 * Get transaction's name.
		 */
		public abstract unowned string get_name ();
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

	/**
	 * Basic interface to access underlying storage.
	 *
	 * StorageManager's implementation shall be responsible for handling underlying storage
	 * connection. Opening and closing it. A connection might be SQL database connection,
	 * file or directory handler, or any other specific for storage represented by implemetation.
	 *
	 * StorageManager, als provides access to frequently used (in repository) managers, including
	 * content, query or workspace one. The main approach is to access such managers quickly and 
	 * encapsulate their construction details. 
	 */ 
	public interface StorageManager : GLib.Object {

		/* properties */
		
		/**
		 * Get {@link StorageContentManager} object
		 */
		public abstract unowned StorageContentManager 	content_manager 	{ get; }

		/**
		 * Get {@link StorageModelManager} object
		 */
		public abstract unowned StorageModelManager   	model_manager   	{ get; }

		/**
		 * Get {@link ObjectManager} object
		 *
		 * Any StorageManager implementation shall provide own ObjectManager class.
		 * The purpose of such implementation is to fetch single object from storage easily and quickly.
		 * Returned object manager shall be associated with storage manager to optimize object creation.
		 */
		public abstract unowned ObjectManager object_manager			{ get; }

		/**
		 * Get {@link ReplicationManager} object
		 */
		//public abstract unowned ReplicationManager? replication_manager		{ get; }

		/**
		 * Get {@link Profiler} object
		 */
		public abstract unowned Profiler              	profiler        	{ get; }

		/**
		 * Get {@link Transaction} object.
		 */
		public abstract unowned Transaction           	transaction     	{ get; }

		/**
		 * Get {@link StorageWorkspaceManager} object
		 */
		public abstract unowned StorageWorkspaceManager	workspace_manager	{ get; }	

		/* signals */

		/**
		 * The signal which shall be emitted when underlying storage is opened.
		 */
		public abstract signal void opened ();

		/**
		 * The signal which shall be emitted when underlying storage is closed.
		 */
		public abstract signal void closed ();

		/**
		 * The signal which shall be emitted when any operation in storage is going to happen.
		 */
		public abstract signal void operation_start ();

		/**
		 * The signal which shall be emitted when any operation in storage is finished.
		 */
		public abstract signal void operation_end ();	

		/* connection methods */
	
		/**
		 * Opens connection to underlying storage.
		 */
		public abstract bool open () throws StorageManagerError;

		/**
		 * Close connection to underlying storage.
		 */
		public abstract bool close () throws StorageManagerError; 
		
		/**
		 * Initialize minimal storage layout required for manager to work.
		 */
		public abstract bool initialize_storage () throws StorageManagerError;

		/* FIXME, add Clonable interface ? */
		public abstract StorageManager? fork ();
		public abstract StorageManager? clone ();
	}

	public interface StorageWorkspaceManager : StorageManager {

		public abstract WorkspaceStorage workspace { get; set; }
	}

	/**
	 * Basic interface for storage operations.
	 *
	 * StorageExecutor interface provides methods to prepare operations in storage.
	 * Validate operations and associated data, and finally execute. 
	 *
	 * Any implementation shall follow three main rules:
	 * 
	 *  1. Gather and collect data
	 *  1. Prepare execution and validate data
	 *  1. Execute (commit) operation
	 */
	public interface StorageExecutor : Executable {
		
		/* methods */
	
		/**
		 * Check if data of current instance exists in storage.
		 */
		public abstract bool exists ();

		/**
		 * Prepares create operation.
		 */
	 	public abstract void prepare_create () throws ValidationError;

		/**
		 * Prepares update operation.
		 */
		public abstract void prepare_update () throws ValidationError;

		/**
		 * Prepares save operation.
		 */
		public abstract void prepare_save () throws ValidationError;

		/**
		 * Prepares remove operation.
		 */
		public abstract void prepare_remove () throws ValidationError;

		/**
		 * Prepares purge operation.
		 */
		public abstract void prepare_purge () throws ValidationError;
	}

	/** 
	 * StorageModelManager is responsible for all operations of associated models.
	 */
	public interface StorageModelManager : Model, StorageExecutor {

		/* methods */
		
		/**
		 * List class names of all models associated with model manager.
		 */
		public abstract string[]? list_model_types ();

		/**
		 * List all available models, associated with model manager which are instances of given type. 
		 * There's no strict rule for model manager to hold only models with unique names. 
		 * There might be different models initialized for particular class, e.g. {@link ObjectModel} and 
		 * {@link StorageModel}.
		 *
		 * @see list_model_types
		 *
		 * @param type, {@link Model} class name 
		 *
		 * @return array of models or null
		 */  
		public abstract unowned Model[]? list_models_by_type (string type);

		/**
		 * Get {@link Model} instance of a named type identified by given name.
		 *
		 * @param type, name of the model to get
		 * @param name, class name of the model
		 *
		 * @return model or null if not found
		 */
		public abstract unowned Model? get_type_model_by_name (string type, string name); 

		/**
		 * Get {@link StorageManager} associated with given model manager.
		 */
		public abstract unowned StorageManager get_storage_manager ();
	}

	public errordomain StorageContentManagerError {

		OBJECT_INVALID,
		OBJECT_DUPLICATE,
		OBJECT_NOT_EXISTS,
		MODEL_INVALID,
		INTERNAL
	}

	/**
	 * Basic interface which provides mathods for {@link Storable} objects' operations.
	 *
	 * Any Storable derived object is volatile (on application level) data presentation and holder.
	 * Content manager is responsible to make such object persistant in underlying storage.
	 * For example create method can perform SQL INSERT query using object's properties as values, 
	 * or write serialized object to a file.
	 *
	 * Any content manager implementation shall not associate given Storable object with particular 
	 * storage, as any object could be stored with any content manager implementation.
	 * 
	 */
	public interface StorageContentManager : GLib.Object {

		/* properties */

		/**
		 * Get {@link NamespaceManager} object.
		 */
		public abstract unowned NamespaceManager namespace_manager { get; }

		/**
		 * Get {@link QueryManager} object.
		 */
		public abstract unowned QueryManager query_manager { get; }

		/* per object methods */

		/**
		 * Check if given Storable object exists.
		 */
		public abstract bool exists (Storable object);

		/**
		 * Create object in storage.
		 */
		public abstract void create (Storable object) throws StorageContentManagerError;

		/**
		 * Update object in storage.
		 */
		public abstract void update (Storable object) throws StorageContentManagerError;

		/**
		 * Save object in storage.
		 *
		 * This method shall create object if doesn't exist or update if it does.
		 * The complexity (and performance) of this method depend on implementation.
		 */
		public abstract void save (Storable object) throws StorageContentManagerError; 

		/**
		 * Mark object as deleted. 
		 */
		public abstract void remove (Storable object) throws StorageContentManagerError;

		/**
		 * Removes object from storage.
		 */
		public abstract void purge (Storable object) throws StorageContentManagerError;

		/**
		 * Get {@link StorageManager} object.
		 */
		public abstract unowned StorageManager get_storage_manager ();
	} 

	public errordomain StorageModelError {
		STORAGE_INVALID,
		STORAGE_EXISTS,
		INTERNAL
	}

	/* Initialized for every given class name */
	
	/**
	 * Describes storage location (and additional attributes if required) for given class.
	 */
	public interface StorageModel : Model, StorageExecutor {
	
		/* properties */

		/** 
		 * Location where data of any instance of the class should be stored.
		 */
		public abstract string location { get; set; }		

		/* methods */

		/**
		 * Get {@link StorageManager} object.
		 */
		public abstract unowned StorageManager get_storage_manager ();
	}

	/* Initialized for every given property name */

	/**
	 * Describes storage location for property.
	 *
	 * The types described by this model shall depend on StorageModel. For example, it may describe 
	 * column in table of SQL database, file in directory, etc. 
	 */
	public interface StorageModelProperty : StorageExecutor, StorageModel, ModelProperty {

		/* properties */

		/**
		 * Tells whether given property is indexed.
		 */
		public abstract bool index { get; set; }

		/**
		 * Tells whether given property is primary one.
		 */
		public abstract bool primary { get; set; }	
	}	
}
