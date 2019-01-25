/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef dbcache_h
#define dbcache_h

#include "dbheader.h"

/**
 * Caching module.
 *
 * All this code resided in both dbtoliet and dbuniqueid but i brought the code into
 * here so dbuniqueid and dbtoliet can concentrate with what they do best.
 * DbCache attempts to act as both reading and writing cache for records.
 * It is also used for filtering purposes.
 */

struct DbCache
	{
	/* gchar* list of fields, NULL's will be present for those not retrieved or set etc */
	void **value;
	/* flags for data */
	gchar *flags;
	/* width of data */
	gint num;
	/* this is the row it refers to */
	gint origrow;
	/* UniqueId of this row */
	DbUniqueId *id;
	/* Filtering and sorting */
	DbFilter *filter;
	/* state of the row, whether its for reading or writing */
	enum
		{
	    OBJ_READ,
	    OBJ_EDITREAD,
	    OBJ_NEW,
	    OBJ_EDITNEW
		} state;
	/* has the row been changed in anyway */
	gboolean changed;
	};

/** State defintion:
 *
 * OBJ_READ is a space filler. hmmm . 
 * OBJ_EDITREAD is that the object was read in and then edited. 
 * OBJ_NEW means that its a completely new object.
 * OBJ_EDITNEW means you just edited a freshly new object that was written back to db.
 */
gint db_cache_cleanup(Object * obj);
DbCache *db_cache_moveto(Object * obj);
DbCache *db_cache_isincache(Object * obj);
gint db_cache_delete(Object * obj);

gint db_cache_getvalue(Object * obj, DbCache * cache, gchar * field, gchar * table, gchar ** value);
gint db_cache_setvalue(Object * obj, DbCache * cache, gchar * field, gchar * table, gchar * value);
gint db_cache_updateid(Object * obj, DbUniqueId * id, DbCache * cache);
gint db_cache_expandfield(Object *obj, gint numfield);

void db_cache_debug(Object * obj);

gboolean db_cache_hascache(Object * obj, int i);
void **db_cache_extract(Object *obj, GList *fields, gint *rows, gint *cols);

#endif
