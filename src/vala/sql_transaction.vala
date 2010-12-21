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

	public class SQLTransaction : GLib.Object, Executable, Transaction {

		/* private properties */
		private GLib.Object _core_transaction = null;

		/* constructor */
		public SQLTransaction (SQLStorageManager manager) {
			Object ();
			this._core_transaction = new MidgardCRCore.Transaction (manager);
		}

		public void begin () throws ExecutableError {
			try {
				(this._core_transaction as MidgardCRCore.Transaction).begin ((MidgardCRCore.Transaction)this._core_transaction);	
			} catch (GLib.Error e) {
				throw new ExecutableError.INTERNAL (e.message);
			}
		}

		public void rollback () throws ExecutableError {
			try {
				(this._core_transaction as MidgardCRCore.Transaction).rollback ((MidgardCRCore.Transaction)this._core_transaction);	
			} catch (GLib.Error e) {
				throw new ExecutableError.INTERNAL (e.message);
			}
		}

		public bool get_status () {
			return (this._core_transaction as MidgardCRCore.Transaction).get_status ((MidgardCRCore.Transaction)this._core_transaction);
		}

		public unowned string get_name () { 
			return (this._core_transaction as MidgardCRCore.Transaction).get_name ((MidgardCRCore.Transaction)this._core_transaction);
		}

		public void execute () throws ExecutableError {
			try {
				(this._core_transaction as MidgardCRCore.Transaction).commit ((MidgardCRCore.Transaction)this._core_transaction);	
			} catch (GLib.Error e) {
				throw new ExecutableError.INTERNAL (e.message);
			}
		}
	}
}
