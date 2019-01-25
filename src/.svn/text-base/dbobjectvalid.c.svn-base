#include <glib.h>
#include <string.h>

#include "dbobjectvalid.h"
#include "db.h"
#include "dbgather.h"
#include "bc.h"
#include "dbtoliet.h"
#include "dbuniqueid.h"

/* This is important file for doing the boolean logic on objects.
	Using these functions can tell you as well as set the conditions of objects. Yahh! 
*/

/* get the cache storing structure */

/**
 * :
 * obj: database object to check
 * 
 *
 *
 * Returns: %TRUE or %FALSE depending on conditions
 */

/* ==========================================================================================
 * State checking code
 */

/**
 * db_isnewrecord:
 * obj: database object to check
 * 
 * Check if you have selected a new record that hasn't been written to the database yet
 * 
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_isnewrecord(Object * obj)
	{
	g_assert(obj);

	if (obj->id == NULL)
		{
		/* warningmsg("obj->id is NULL. This isn't hugly bad but could be if i was god"); */
		return TRUE;
		}
	if (db_id_isnewrecord(obj->id) == TRUE)
		{
		if (obj->currentcache != NULL && obj->currentcache->state == OBJ_EDITNEW)
			return FALSE;
		return TRUE;
		}
	return FALSE;
	}

/**
 * db_isabletogetid:
 * @obj: database object
 *
 * Tell if you the record your on is just freshly added and you have no hope of getting
 * an id from it. 
 *
 * Returns: %TRUE if the id is able to be extracted, or %FALSE if no id can be got
 */

gboolean
db_isabletogetid(Object * obj)
	{
	if (obj->unknownid == TRUE)
		return FALSE;
	return TRUE;
	}

/**
 * db_isappend:
 * obj: database object to check
 * 
 * Function is kinda obsolete now, will check if your appending to an existing object.
 * You shouldn't need to call this code much anymore.
 *
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_isappend(Object * obj)
	{
	if (db_isnewrecord(obj) == TRUE && obj->num > 1)
		return TRUE;
	/* if (obj->append == TRUE) return TRUE; */
	return FALSE;
	}

/**
 * db_isfiltered:
 * obj: database object to check
 * 
 * Is the current record you are on a filtered record set?
 * 
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_isfiltered(Object * obj)
	{
	if (obj->filtered == TRUE)
		return TRUE;
	return FALSE;
	}

/**
 * db_isrecordchanged:
 * obj: database object to check
 * 
 * Is the current record you are on changed in any way?  Like you know you wrote something to it.
 * It will ignore the fact if a different row had been changed.
 * 
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_isrecordchanged(Object * obj)
	{
	DbCache *cache;

	g_assert(obj);
	cache = db_cache_isincache(obj);
	if (cache == NULL)
		return FALSE;
	if (db_iscachechanged(cache) == TRUE)
		return TRUE;
	return FALSE;
	}

/**
 * db_isobjectchanged:
 * obj: database object to check
 * 
 * Is the current object changed in any way?  This will check to see if any writes have been done.
 *
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_isobjectchanged(Object * obj)
	{
	g_assert(obj);

	if (obj->changed == TRUE)
		return TRUE;
	return FALSE;
	}

/**
 * db_isrecordset:
 * obj: database object to check
 * 
 * Does object have data in it, was an sqlread carried out on it?
 *
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_isrecordset(Object * obj)
	{
	if (obj->res == NULL)
		return FALSE;
	if (obj->query == NULL)
		return FALSE;
	return TRUE;
	}

/**
 * db_isfieldposok:
 * @obj: object to check
 * @pos: position to check for
 *
 * Looks at a field postion and see if its an ok reference.  This will check
 * to see if @pos is inside the limits of the fields defined in obj->field.
 *
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_isfieldposok(Object * obj, gint pos)
	{
	g_assert(obj);
	if (pos < 0)
		return FALSE;
	if (pos >= obj->numfield)
		return FALSE;
	return TRUE;
	}

gboolean
db_isrecordposok(Object * obj, gint pos)
	{
	g_assert(obj);
	if (pos < 0 || pos >= obj->num)
		return FALSE;
	if (db_isappend(obj) == TRUE)
		{
		if (pos == obj->num - 1)
			return FALSE;
		}
	return TRUE;
	}

gboolean
db_isvalidtable(Object * obj)
	{
	g_assert(obj);

	if (obj->basetable == NULL)
		return FALSE;
	if (obj->numfield <= 0)
		return FALSE;

	return TRUE;
	}

/**
 * @obj: database object
 * 
 * Same as db_isnewrecord() at the moment but would be changed later on.
 * 
 * Returns: %TRUE or %FALSE
 */
gboolean
db_iscachenewrecord(DbObject *obj, DbCache *cache)
	{
	if (cache)
		{
		if (cache->state == OBJ_NEW)
			return TRUE;
		else
			return FALSE;
		}
	if (obj->newrecord == FALSE)
		return FALSE;
	return TRUE;
	}

/**
 * db_iscachechanged:
 * @cache: database cache object
 * 
 * Is this item of cache been changed in any way?
 *
 * Returns: %TRUE or %FALSE
 */
gboolean
db_iscachechanged(DbCache * cache)
	{
	if (cache->changed == TRUE)
		return TRUE;
	return FALSE;
	}

/**
 * db_iscachechanged:
 * @cache: database cache object
 * 
 * Has the query in @obj been written back to the database. This function may not
 * function perfectly.
 *
 * Returns: %TRUE or %FALSE
 */
gboolean
db_isindatabase(Object * obj)
	{
	g_assert(obj);
	return obj->newrecord;
	}

/**
 * db_isabletoadd:
 * @obj: database object
 *
 * Work out if your allowed to add another record. You wont be allowed to add another record if you
 * just added one and havn't typed anything. This is to stop you hitting add lots in the UI.  See 
 * you dont really want a couple of hundred blank records.
 *
 * Returns: %TRUE or %FALSE
 */
gboolean
db_isabletoadd(Object * obj)
	{
	gint i;

	/* yes g_assert before you start using it, not after */
	g_assert(obj);
	/* m.essage("%d num cache, %d",obj->numcache,obj->num); */
	/* check all items in the cache to see if something is new but havn't been finished yet */
	if (obj->cache != NULL)
		for (i = 0; i < obj->numcache; i++)
			{
			if (obj->cache[i] != NULL)
				if (obj->cache[i]->changed == FALSE && obj->cache[i]->state == OBJ_NEW)
					return FALSE;
			}
	return TRUE;
	}

/* ==========================================================================================
 * Setting objects for different conditions code */
void
db_setforread(Object * obj)
	{
	g_assert(obj);

	obj->row = 0;
	obj->newrecord = FALSE;
	obj->freeresult = TRUE;
	obj->changed = FALSE;
	}

/**
 * db_setforinsert:
 * @obj: database object
 *
 * Appends a record on the end of an existing object.  
 */

void
db_setforinsert(Object * obj)
	{
	obj->row = obj->num;
	obj->newrecord = TRUE;
	obj->unknownid = TRUE;
	obj->num++;
	if (!strcmp(obj->basetable, "programme") || !strcmp(obj->basetable, "product"))
	if (obj->filtered == TRUE)
		{
		db_filter_resort(obj, 0);
		}
	}

void
db_setforwrite(Object * obj)
	{
	g_assert(obj);

	obj->row = 0;
	obj->newrecord = FALSE;
	obj->freeresult = TRUE;
	obj->changed = FALSE;
	}

void
db_setnotchanged(Object * obj)
	{
	g_assert(obj);

	obj->changed = FALSE;
	}

gint
db_numfields(Object * obj)
	{
	g_assert(obj);
	return obj->numfield;
	}

/* CODE GRAVE YARD */

/*
gboolean
db_isincache(Object * obj)
   {
   g_assert(obj);
   if (globaldbwritecache != NULL)
      if (globaldbwritecache->obj != NULL)
         { */

/* check cache for a local copy */

/*         if (obj == globaldbwritecache->obj && obj->row == globaldbwritecache->row &&
               (obj->pg_oid == globaldbwritecache->pg_oid || obj->pg_oid == -1))
            { */

/* debugmsg("Getting value from the write cache"); */

/* debugmsg("looking in cache, row is %d, cache row is %d",obj->row,globaldbwritecache->row); */

/*            return TRUE;
            }
         }
   return FALSE;
   }
 
gboolean
db_isflushneeded(Object * obj)
   {
   g_assert(obj);
   if (globaldbwritecache != NULL)
      if (globaldbwritecache->obj != NULL)
         {
         if (obj != NULL && obj->query != NULL && globaldbwritecache->obj->query != NULL
               && globaldbwritecache->obj->changed == TRUE)
            {
            if (strcmp(obj->query, globaldbwritecache->obj->query) == 0)
               { */

/* Query is the same so empty the cache */

/*               return TRUE;  */

/* db_refreshobject(obj); */ /* Flush does own object refreshes */

/* } */

/* else debugmsg("really dont have to flush cache yet"); */

/*            }
			if (obj != globaldbwritecache->obj)
				{
				debugmsg("obj != globaldbwritecache so i'm flushing");
				return TRUE;
				} */

/* else debugmsg("dont need to flush cache yet for %s",obj->table->basetable); */

/*         }
   return FALSE;
   } */

/**
 * db_isfulldbsupport:
 * obj: database object to check
 * 
 * It checks to see if the backend database has full support in bonddb.  A postgresql
 * database would return %TRUE but a database not fully supported by bonddb, gnome-db,
 * or sql92 standards may only be able do insert and selects on, and information like
 * constraints, relationships, default values etc maybe lost.  This function allows
 * parts of bonddb that rely on these trickier sql92 features to shutdown and not seg
 * fault.
 *
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_isfulldbsupport(Object * obj)
	{
	g_assert(obj);
	return obj->fulldbsupport;
	}

/**
 * db_issqlreadable:
 * obj: database object to check
 * 
 * Checks to see if the SQL statement be parsed though the lex parser.  This may not
 * be the case on more complex sql statements, and they may not be valid anyway in
 * the database your using.  This is set to false if parsing the sql statement fails,
 * and auxillery means are used to obtain the data needed.
 *
 * Returns: %TRUE or %FALSE depending on conditions
 */
gboolean
db_issqlreadable(Object * obj)
	{
	g_assert(obj);
	return obj->sqlgood;
	}

gboolean
db_iscacheloaded(Object * obj)
	{
	if (obj->numcache < obj->num)
		return FALSE;
	return TRUE;
	}

/**
 * db_isasodeadobject:
 * 
 * This is for search box, then you dont want to disable the controls.  
 * It will check if a query is present, and if there isn't a query return %TRUE.
 */
gboolean
db_isasodeadobject(Object * obj)
	{
	mem_verify(obj);
	return obj->dead;
	}

gboolean
db_isdeadobject(Object * obj)
	{
	mem_verify(obj);
	return obj->dead;
	}
