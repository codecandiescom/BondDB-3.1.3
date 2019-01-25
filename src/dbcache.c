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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bc.h"

#include "dbcache.h"
#include "dbtoliet.h"
#include "dbuniqueid.h"
#include "dbobject.h"
#include "dbuniqueid.h"
#include "dbdefault.h"
#include "dbbureaucrat.h"
#include "dbfield.h"
#include "dbsqlparse.h"
#include "dbfilter.h"
#include "dbobjectnav.h"
#include "dbverifysql.h"
#include "db.h"

/*static void db_toliet_freecache_element(DbCache * c);
static void db_toliet_blank_element(DbCache * c, gint num); */

static gint
db_cache_createall(Object * obj)
	{
	g_assert(obj);
	if (obj->cache != NULL)
		db_cache_cleanup(obj);
	obj->cache = mem_alloc(sizeof(DbCache *) * (obj->num + 1));
	memset(obj->cache, 0, sizeof(DbCache *) * (obj->num + 1));
	obj->numcache = obj->num;

	return 0;
	}

static gint
db_cache_expand(Object * obj)
	{
	gint ptrlen;
	char *ptr;

	if (obj->num < obj->numcache)
		return -1;
	obj->cache = mem_realloc(obj->cache, sizeof(DbCache *) * (obj->num + 1));
	ptr = (void*)obj->cache;
	ptr += obj->numcache * sizeof(DbCache *);
	ptrlen = (sizeof(DbCache *) * (obj->num)) - (sizeof(DbCache *) * 
		(obj->numcache));
	memset(ptr, 0, ptrlen);
	obj->numcache = obj->num;

	return 0;
	}

void
db_cache_freecache_element(DbCache * c)
	{
	gint i;

	for (i = 0; i < c->num; i++)
		{
		if (c->value[i] != NULL)
			mem_free(c->value[i]);
		}
	db_filter_free(c->filter);
	db_id_free(c->id);

	mem_free(c->value);
	mem_free(c->flags);
	mem_free(c);
	return ;
	}

static void
db_cache_blank_element(DbCache * c, gint num)
	{
	g_assert(c);

	c->num = num;

	if (num <= 0)
		num = 1;

	c->value = (void **)mem_calloc(sizeof(void *) * num);

	c->origrow = -1;
	c->id = NULL;
	c->state = OBJ_NEW;
	c->changed = FALSE;
	}

/**
 * db_cache_addtocache:
 * @obj: Database object
 *
 * Add to the cache index file a new entry, and return the newly created cache.  Do not 
 * free the variable returned.  You need to make sure you add an id and update the idindex
 * file after using this function. 
 *
 * Returns: Null on error else the newly created cache object.
 */
static DbCache *
db_cache_create(Object * obj)
	{
	DbCache *c;

	/* add an item to it */
	c = (DbCache *) mem_calloc(sizeof(DbCache));
	db_cache_blank_element(c, obj->numfield);
	/* create filter flags */
	c->flags = (gchar *) mem_calloc(sizeof(gchar) * (obj->numfield + 1));
	return c;
	}

/* =============================================================================
 * External Code.
 */

/**
 * db_cache_moveto:
 * @obj: Database object
 * 
 * Replaces remberid() function.
 * Moves to the current row in the @obj, and if there isn't a cache item 
 * existing it will create one.  If its a read source it will extract the 
 * UniqueIds (primary keys or oid's).  Referencing is done on a row basis.
 * 
 * Returns: DbCache element for your current row.
 */
DbCache *
db_cache_moveto(Object * obj)
	{
	DbUniqueId *id;

	if (obj->cache == NULL)
		db_cache_createall(obj);
	else if (obj->numcache < obj->num)
		db_cache_expand(obj);
	if (obj->cache[obj->row] == NULL)
		{
		obj->cache[obj->row] = db_cache_create(obj);
		obj->cache[obj->row]->origrow = obj->row;
		id = db_id_remeberbyrow(obj, obj->row);
		obj->cache[obj->row]->filter = db_filter_create(TRUE, obj->row);
		obj->cache[obj->row]->id = id;
		if (db_id_isnewrecord(id) == FALSE)
			if (obj->cache[obj->row]->state == OBJ_NEW)
				obj->cache[obj->row]->state = OBJ_READ;
		}
	obj->currentcache = obj->cache[obj->row];
	/* Align the id's */
	db_id_syncid(obj);
	return obj->currentcache;
	}

/**
 * db_cache_isincache:
 * @obj: Database object
 *
 * Works out if the record you have currently selected is in cache or not.  This will take
 * the current UniqueId and row and see if there is data for this in cache.  
 *
 * Returns: %NULL if not in cache, else returns cached record.
 */
DbCache *
db_cache_isincache(Object * obj)
	{
	/* i suspect a bug lies here within */
	g_assert(obj);
	if (obj->cache == NULL)
		return NULL;
	/* the quick way... */
	if (obj->row >= obj->numcache)
		{
		errormsg("Referce to %s is %d outside of range of %d", 
				obj->basetable, obj->row, obj->numcache);
		return NULL;
		}
	return obj->cache[obj->row];
	}

/**
 * db_cache_getvalue:
 * @obj: Database object
 * @cache: Single cache element
 * @field: Field name
 * @value: Pointer to a string that will have the returned string assigned to it.
 *
 * Get the value of a value in the cache.
 *
 * Returns: non-zero on failure
 */
gint
db_cache_getvalue(Object * obj, DbCache * cache, gchar * field, gchar * table, 
		gchar ** value)
	{
	gint fieldpos;

	g_assert(cache);
	g_assert(field);
	g_assert(obj);
	*value = NULL;
	if (cache->value == NULL)
		{
		warningmsg("Nothing in here worth returning");
		return -1;
		}

	fieldpos = db_field_getpos(obj, field, table);
	if (fieldpos >= obj->numfield || fieldpos < 0)
		{
		errormsg("Invalid field %s, can not find a postiion for this.",
			field);
		return -1;
		}
	*value = cache->value[fieldpos];
	/* m.essage("got %s from cache. pos %d",*value,fieldpos); */

	return 0;
	}

/* Function for testing cache courption. */

/**
 * db_cache_setvalue:
 * @obj: Database object
 * @cache: Single cache element
 * @field: Field name
 * @value: Value to set it to
 *
 * Set the value in the cache.
 *
 * Returns: negitive on error else the postion in the string that it saved it to.
 */

/* Hello.!
	This will clean up cache, well first of all flush it */
gint
db_cache_setvalue(Object * obj, DbCache * cache, gchar * field, gchar * table, gchar * value)
	{
	gint fieldpos;

	g_assert(cache);
	/* m.essage("Write: %s = %s in row %d id %d",field,value,cache->origrow,cache->id->pg_oid); */
	fieldpos = db_field_getpos(obj, field, table);
	if (fieldpos >= cache->num || fieldpos < 0)
		{
		errormsg("Invalid field %s, can not find a position for this. "
			"Field pos is set to %d of %d items.", field,
			fieldpos, cache->num);
		db_obj_debug(obj);
		return -1;
		}
	/* set to null or a value automatically */
	if (cache->value[fieldpos] != NULL && value != cache->value[fieldpos])
		mem_free(cache->value[fieldpos]);

	if (value != NULL)
		cache->value[fieldpos] = mem_strdup(value);
	else
		{
		cache->value[fieldpos] = NULL;
		return fieldpos;
		}
	
	db_bureaucrat_checkmisticconnections(obj, cache);

	return fieldpos;
	}

/**
 * db_cache_debug:
 *
 * Display all the records that are in the cache and other such information.
 */
void
db_cache_debug(Object * obj)
	{
	gint i, j;

	debug_minorheader();
	debug_output("Contents for %s.  %d rows, %d columns\n", 
			obj->basetable, obj->num, obj->numfield);
	for (i = 0; i < obj->numfield; i++)
		debug_output("%s\t", obj->field[i]->field);
	debug_output("\n");

	/* for (i = 0; i < obj->numcache; i++) if (obj->cache[i]!=NULL) { for (j = 0; j < obj->cache[i]->num; j++)
	   debug_output("%s\t", obj->cache[i]->value[j]); debug_output("\n"); } */
	for (i = 0; i < obj->numcache; i++)
		{
		if (obj->cache[i] != NULL)
			{
			if (obj->cache[i]->filter)
				if (obj->cache[i]->filter->active == FALSE)
					{
					printf("Row not visible.\n");
					continue;
					}

			for (j = 0; j < obj->cache[i]->num; j++)
				{
				printf("%s: %s\n", obj->field[j]->field, 
					(gchar *) obj->cache[i]->value[j]);
				}
			}
		printf("[%d]-------------------------\n", i);
		}

	debug_minorfooter();
	}

/**
 * db_cache_updateid:
 * @obj: database object
 * @id: database cache object.
 * @cache: Object cache item
 *
 * This function is used internally. It takes the @cache write and and updates any ids 
 * that refer to the same row position and update there id's to the new id. This is designed
 * for inserts or other times when uniqueid's may change in an object. 
 *
 * Returns: non-zero on failure
 */
gint
db_cache_updateid(Object * obj, DbUniqueId * id, DbCache * cache)
	{
	g_assert(obj);
	g_assert(cache);
	if (obj->row == cache->origrow)
		obj->id = id;
	/* may have needed to free this. will find out later on */
	if (cache->id != NULL)
		mem_free(cache->id);
	cache->id = id;
	return 0;
	}

/**
 * db_cache_cleanup:
 * @obj: Database object
 *
 * Free cache array for an object.
 * 
 * Returns: non-zero on failure
 */
gint
db_cache_cleanup(Object * obj)
	{
	gint i;

	g_assert(obj);
	for (i = 0; i < obj->numcache; i++)
		if (obj->cache[i] != NULL)
			db_cache_freecache_element(obj->cache[i]);
	mem_free(obj->cache);
	obj->cache = NULL;
	obj->numcache = 0;
	obj->currentcache = NULL;
	return 0;
	}

/**
 * db_cache_delete:
 * @obj: Database object.
 *
 * Deletes an item from cache and realigns the object cache.  It will delete
 * based upon your current row.
 *
 * Returns: Negitive on error.
 */
gint
db_cache_delete(Object * obj)
	{
	gint i;

	/* Error checking */
	g_assert(obj);
	if (obj->row >= obj->numcache || obj->row < 0)
		return -1;
	/* Free cache element */
	db_cache_freecache_element(obj->cache[obj->row]);
	for (i = obj->row; i < obj->numcache - 1; i++)
		{
		obj->cache[i] = obj->cache[i + 1];
		if (obj->cache[i] != NULL)
			obj->cache[i]->origrow--;
		}
	/* Realign stuff */
	obj->cache[obj->numcache - 1] = NULL;
	obj->numcache--;
	if (obj->numcache == 0)
		db_cache_cleanup(obj);
	else 
	if (obj->row >= obj->numcache && obj->numcache > 0)
		obj->row--;
	if (obj->row >= 0 && obj->cache)
		obj->currentcache = obj->cache[obj->row]; 
	return 0;
	}

/**
* db_cache_hascache:
* Francis: Check whether the given object has a cache or not.
* The true motivation of writing this function was due to the perplexing failure
* of simple checking (obj->numcache > 0). Andrew exaplained that it sets size of
* numcache ahead before assigning the numcache. So use this function.
*/
gboolean
db_cache_hascache(Object * obj, int i)
	{
	return (obj->numcache > 0 && i < obj->numcache && obj->cache && obj->cache[i]);
	}

/**
 * db_cache_expandfield:
 * @obj: Database object.
 * @numfield: New field number length
 * 
 * Expand the number of fields stored in the cache.
 * 
 * Returns: non-zero on error.
 */
gint
db_cache_expandfield(Object * obj, gint numfield)
	{
	DbCache *c;
	gint i, j;

	g_assert(obj);
	if (obj->cache == NULL)
		return -1;
	for (i = 0; i < obj->numcache; i++)
		{
		c = obj->cache[i];
		if (c == NULL)
			continue;
		if (numfield < c->num)
			{
			errormsg("Can not strink cache");
			continue;
			}
		c->value = (void **)mem_realloc(c->value, sizeof(void *) * (numfield + 1));
		c->flags = (gchar *) mem_realloc(c->flags, sizeof(gchar) * (numfield + 1));
		for (j = c->num; j < numfield; j++)
			{
			c->value[j] = NULL;
			c->flags[j] = 0;
			}
		c->num = numfield;
		/* m.essage("cache expanded to %d items",numfield); */
		}
	return 0;
	}

/**
 * db_cache_extract:
 * @obj: database object
 * @fields: A glist array of all the field names you want, teh next 
 * @rows: returned number of rows in the list.
 * @cols: returned number of columns, should be 
 *
 * Creates an array of dimisons @rows x @cols containing the values in cache
 * of each field and table requested in @fields. @fields must be a glist
 * of strings containting both the field and corresponding table names. Create
 * it with every second entry being a tablename.  It ignores sorting so if you
 * require the array returned sorted use the traditional db_getvalue etc. Also
 * it assumes all the values have already been obtained from the database.
 *
 * Returns: Freshly created array which you must free using mem_free, do not
 * free the individual elements in this array.  
 */
void **
db_cache_extract(Object * obj, GList * fields, gint * rows, gint * cols)
	{
	void **retarr;
	int *fieldposarr;
	long pg_oid;
	gint numcol, colcount = 0, rowcount = 0, i, retcol;
	gchar *tmpvalue;
	GList *walk;
	DbFieldDef *fielddef = NULL;
	gchar *query;
	
	g_assert(obj);
	*rows = db_numrecord(obj);
	*cols = 0;
	if (*rows == 0)
	     {
	     debuglog(80,"Empty record set %s, can't do cache extraction.", obj->basetable);
	     return NULL;
	     }
	   
	if (fields == NULL)
	     {
	     errormsg("No fields specified on %s",obj->basetable);
	     return NULL;
	     }
	numcol = g_list_length(fields) / 2;
	if (numcol < 1)
		{
		errormsg("No fields requested, please parse in a field then tablename format");
		return NULL;
		}

	fieldposarr = mem_calloc(sizeof(void *) * numcol);
	for (walk = g_list_first(fields); walk != NULL; walk = walk->next)
		{
		g_assert(walk);
		g_assert(walk->next);
		fieldposarr[colcount] = db_field_getpos(obj, walk->data, (gchar *) ((GList *) (walk->next)->data));
		if (fieldposarr[colcount] != -1)
			colcount++;
		else
		     {
		     /* try and append a table.field to the sql statement */
		     if (db_verifysql_addfieldtable(obj, walk->data, (gchar *) ((GList *) (walk->next)->data)) == 0)
			{
			gboolean readd=FALSE;
			gint oldrow;
			
			db_toliet_flushobject(obj);
			readd = db_isnewrecord(obj);
			oldrow = obj->row;
			debuglog(99,"Re-running SQL Statement because a"
				      "field was added");
			query = mem_strdup(obj->query);
			obj = db_sql(obj,query);
			mem_free(query);
			if (readd)
			     db_add(obj);
			db_moveto(obj,oldrow);
			/* db_obj_sqlread(obj, obj->query); */
			/* db_cache_debug(obj); */
/*			for (i=0;i<obj->numcache;i++)
			     if (obj->cache[i])
			       	obj->cache[i]->state = OBJ_READ; */
			mem_free(fieldposarr);
			return db_cache_extract(obj,fields,rows,cols);
			}
		     else
			errormsg("Failed to extract field position for %s.%s in query %s", 
				 (gchar *) ((GList *) (walk->next)->data),
			         (gchar *)walk->data, obj->query);			
		     }
		   
		walk = walk->next;
		}
	retcol = colcount;
	*cols = retcol;
	for (i = 0; i < obj->numfield; i++)
		if (obj->field[i]->isuniqueid)
			{
			fieldposarr = mem_realloc(fieldposarr, sizeof(void *) * (colcount + 1));
			fieldposarr[colcount] = i;
			colcount++;
			}

	if (colcount == 0)
	     {
		mem_free(fieldposarr);
		return NULL;
	     }
	
	/* Warning: code ignores sort positions */
	retarr = mem_calloc(sizeof(void *) * ((*rows) * colcount));
	for (i = 0; i < obj->numcache; i++)
		{
		/* create a new cache element */
		if (!obj->cache[i])
			{
			obj->cache[i] = db_cache_create(obj);
			obj->cache[i]->origrow = i;
			obj->cache[i]->id = db_id_remeberbyrow(obj, i);
			obj->cache[i]->filter = db_filter_create(TRUE, i);
			if (db_id_isnewrecord(obj->cache[i]->id) == FALSE)
				if (obj->cache[i]->state == OBJ_NEW)
					obj->cache[i]->state = OBJ_READ;
			}
		if (obj->cache[i]->filter->active == FALSE)
			continue;

		for (colcount = 0; colcount < (*cols); colcount++)
			{
			if (fieldposarr[colcount] < 0)
				continue;
			if (obj->cache[i]->value[fieldposarr[colcount]] && colcount < retcol)
				retarr[(rowcount * (*cols)) + colcount] = obj->cache[i]->value[fieldposarr[colcount]];
			else if (obj->res)
				{
				/* Get field defination */
				fielddef = obj->field[fieldposarr[colcount]]->fielddef;
				/* Only extract value if its a read value cache item */
				/* if (obj->cache[i]->value == NULL) */
				if (obj->cache[i]->state != OBJ_NEW && 
				    obj->cache[i]->state != OBJ_EDITNEW)
				     {
				     tmpvalue = db_dbgetvalue(obj->res, i, 
					       fieldposarr[colcount]);
				     }
				else
				     continue;
				
				if (fielddef->datatype == DBPGOID)
					{
					pg_oid = atoi(tmpvalue);
					obj->cache[i]->value[fieldposarr[colcount]] = mem_alloc(sizeof(long));
					memcpy(obj->cache[i]->value[fieldposarr[colcount]], &pg_oid, sizeof(long));
					if (colcount < retcol)
						retarr[(rowcount * retcol) + colcount] = obj->cache[i]->value[fieldposarr[colcount]];
					}
				else if (colcount < retcol)
					{
					if (tmpvalue)
						obj->cache[i]->value[fieldposarr[colcount]] = mem_strdup(g_strstrip(tmpvalue));
					retarr[(rowcount * retcol) + colcount] =
					    obj->cache[i]->value[fieldposarr[colcount]];
					}
				continue;
				}
			if (obj->cache[i]->value[fieldposarr[colcount]])
				{
				/* Check if it is a function wanting to be exectued */
				if (strcmp(obj->cache[i]->value[fieldposarr[colcount]], "EXECFUNCTION") == 0)
					{
					/* Get field defination */
					fielddef = obj->field[fieldposarr[colcount]]->fielddef;
					if (fielddef->defaultvalue != NULL)
						{
						tmpvalue = db_default_execfunction(fielddef->defaultvalue);
						mem_free(obj->cache[i]->value[fieldposarr[colcount]]);
						if (!tmpvalue)
							{
							obj->cache[i]->value[fieldposarr[colcount]] = NULL;
							continue;
							}
						retarr[(rowcount * retcol) + colcount] =
							 obj->cache[i]->value[fieldposarr[colcount]] =
								  mem_strdup(g_strstrip(tmpvalue));
						mem_free(tmpvalue);
						}
					}
				}
			}
		rowcount++;
		}
	mem_free(fieldposarr);
	return retarr;
	}
