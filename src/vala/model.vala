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

	public errordomain ValidationError {
		NAME_INVALID,
		NAME_DUPLICATED,
		TYPE_INVALID,
		VALUE_INVALID,
		REFERENCE_INVALID,
		PARENT_INVALID,
		LOCATION_INVALID,
		INTERNAL
	}	

	public interface Model : GLib.Object {
		
		/* properties */
		public abstract Model?       parent    { get; set; }
		public abstract string      @namespace       { get; set;  }
		public abstract string      name            { get; set;  }

		/* methods */
		
		/**
		 * Associate new model
		 */
		public abstract Model add_model (Model model);

		/**
		 * Get model by given name.
		 */
		public abstract unowned Model? get_model_by_name (string name);
		
		/**
		 * List all associated models.
		 */
		public abstract unowned Model[]? list_models ();

		/**
		 * Check whether model is valid
		 */
		public abstract void is_valid () throws ValidationError;
	}

	public interface ModelProperty : GLib.Object, Model {
		
		/* properties */
		
		/**
		 * The {@link GLib.Type} of the value.
		 */
		public abstract GLib.Type   valuegtype     { get; }

		/**
		 * Name of the {@link GLib.Type} of the value
		 */
		public abstract string      valuetypename  { get; set; }

		/**
		 * Default of the value
		 */
		public abstract string	    valuedefault   { get; set; }

		/**
		 * Description of the property.
		 */
		public abstract string      description     { get; set; }

		/**
		 * Tells whether property model is private. 
		 */
		public abstract bool        @private      { get; set; }

		/**
		 * Tells whether ModelProperty is reference
		 */
		public abstract bool	isref { get; }

		/**
  		 * The name of reference.
		 */
		public abstract string 	refname { get; }

		/**
		 * The name of reference's target
		 */
		public abstract string 	reftarget { get; }
	}
}
