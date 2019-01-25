#include <stdlib.h>
#include <glib.h>

#include "dbheader.h"

#include "db.h"
#include "bc.h"
#include "dbobject.h"
#include "dbobjectnav.h"
#include "dbmapme.h"
#include "dbuniqueid.h"
#include "dbtoliet.h"
#include "dbcache.h"

/* Record Navigation Component */

/**
 * db_numrecord:
 * @obj: database object
 *
 * Get the number of rows in a record set.
 *
 * Returns: non-zero on error else number of rows.
 */
gint
db_numrecord(Object * obj)
	{
	g_assert(obj);
	if (obj->filtered == TRUE)
		return db_filter_num(obj);
	return obj->num;
	}

/**
 * db_moveto:
 * @obj: database object
 * @row: new row to move to.
 *
 * Move to a new row position in the database object recordset.  This will set 
 * the obj->row as well as update everything else in the object to point to 
 * the new row.  You can move to records yet to be written as well as back to 
 * prevously added records.  If the @obj has been filtered previously this 
 * function will look up the correct corresponding row for its filtered 
 * position and move to that row.
 *
 * Returns: non-zero on failure.
 */
gint
db_moveto(Object * obj, gint row)
	{
	g_assert(obj);
	if (obj->filtered == TRUE)
		row = db_filter_moveto(obj, row);

	/* check if everything is ok for moving */
	if (row >= obj->num)
		{
		warningmsg("Trying to request a record outside the recordpos. "
			"Row %d > %d", row, obj->num);
		return -1;
		}
	if (row < 0)
		{
		warningmsg("Object: %s, pos is less than 0, like %d, oops", 
				obj->basetable, row);
		return -1;
		}
	/* set the position */
	obj->row = row;
	/* update the cache and id stuff. this is like important. But its 
	 * sshjit slow. I'll think of something later to do about this in 
	 * regard to speeed. It does do the all important obj->id setting */
	/* I finally did something about it being slow. YESS ITS NOT SLOW 
	 * ANYMORE!. disregard comments about it been slow */
	db_cache_moveto(obj);
	obj->mapoutofsync = TRUE;
	return 0;
	}

gint
db_movelast(Object * obj)
	{
	g_assert(obj);
	if (obj->num > 0)
		return db_moveto(obj, obj->num - 1);
	return -1;
	}

gint
db_movenext(Object * obj)
	{
	g_assert(obj);
	if (obj->row + 1 <= obj->num - 1)
		return db_moveto(obj, obj->row + 1);
	else
		return -2;
	}

/**
 * db_copyobject:
 * @obj: database object
 * @row: row to copy
 *
 * A function to copy objects.  If row is -1 then copy all objects,
 * else just copy the requested row.
 *
 * Returns: newly created database object or NULL on error.
 */
Object *
db_copyobject(Object * obj, gint row)
	{
	Object *retobj = NULL;

	g_assert(obj);
	/* return all objects, a direct copy */
	if (row == -1)
		{
		/* Need to flush objects cause of stale objects problem */
		db_toliet_flushall();
		/* run the same query on a new object */
		retobj = db_sql(NULL, obj->query);
		/* move to the record that it is suppose to */
		db_moveto(retobj, obj->row);
		/* update all the other mapped objects */
		db_copymapobject(retobj, obj);
		}
	/* makea copy of just one record */
	else
		{
		if (row >= obj->num)
			{
			db_freeobject(retobj);
			errormsg("Invalid Row Selected");
			g_assert(NULL);
			return NULL;
			}

		/* Need to flush objects cause of stale objects problem */
		db_toliet_flushall();
#ifdef __PORTED
		/* Add in where clause into SQL statement. Man, this is where sql parser is used. yes. */
#endif
		/* run the query */
		retobj = db_sql(NULL, obj->query);
		/* grab the associate objects */
		errormsg("db_copymapobject's hasn't been finished yet, please code this");
		db_copymapobject(retobj, obj);
		}
	return retobj;
	}

gint
db_moverecordbyoid(Object * obj, DbUniqueId * id)
	{
	return db_moveto(obj, db_id_moveto(obj, id));
	}

/**
 * db_currentrow:
 * 
 * Get what row in a table your currently on
 * 
 * Returns: row index
 */
gint
db_currentrow(Object * obj)
	{
	return obj->row;
	}

/**
 * db_moveforce:
 *
 * Like db_moveto() but a forced version of it. You shouldn't normally have 
 * to use this function. It forces the dataobject to move to a specific row 
 * ignoring all filters that maybe inplace as well as ignoring any tests to 
 * see if its physically possible to move here. This will normally have 
 * undiseriable results.
 */
gint
db_moveforce(Object * obj, gint row)
	{
	obj->row = row;
	return 0;
	}
