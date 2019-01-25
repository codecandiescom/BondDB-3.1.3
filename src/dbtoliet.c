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
#include <string.h>
#include <glib.h>

#include "bc.h"
#include "dbobject.h"
#include "dbtoliet.h"
#include "dbcache.h"
#include "db.h"
#include "dbclient.h"
#include "dbwrapper.h"
#include "dbuniqueid.h"
#include "dbbureaucrat.h"
#include "dbfield.h"
#include "dbsqlparse.h"
#include "dbfilter.h"

gboolean dbtolietdebug = TRUE;

/**
 * db_toliet_flushall:
 *
 * Write all objects to the database that have changed.  
 *
 * Returns: non-zero on failure
 */
gint
db_toliet_flushall(void)
	{
	DbFlush *f;
	DbCache *c = NULL;
	GList *cachewalk;
	GList *walk, *flushlist;

	flushlist = db_bureaucrat_changedobjects();
	walk = g_list_first(flushlist);

	while (walk != NULL)
		{
		f = walk->data;
		cachewalk = g_list_first(f->cachelist);
		while (cachewalk != NULL)
			{
			c = cachewalk->data;
			/* debugwrap(f->obj, c, "db_toliet_flushall"); */
			db_toliet_flush(f->obj, c);
			c->changed = FALSE;
			cachewalk = cachewalk->next;
			}
		f->obj->changed = FALSE;
		walk = walk->next;
		}
	db_bureaucrat_freeflush(flushlist);
	return 0;
	}

/**
 * db_toliet_flush:
 *
 * Flushes an individual cache item
 *
 * Returns: non-zero on failure
 */
gint
db_toliet_flush(Object * obj, DbCache * cache)
	{
	gint cont = 0;
	gchar *query = NULL;
	DbUniqueId *id;

	/*debugwin("db_toliet_flush() called by obj\n"
	         "\tname: %s\n"
	         "\tcache value: %s\n"
	         "\tcache pg_oid: %d\n" "\tcache id: ?\n", obj->basetable, 
		 cache->value[0], cache->id->pg_oid);
	debugwin("obj->changed: %d\n"
	         "cache->changed: %d\n"
	         "db_iscachechanged(cache): %d\n", obj->changed, 
		 cache->changed, db_iscachechanged(cache)); */
	if (obj->changed == FALSE || cache->changed == FALSE)
		{
		errormsg("Your trying to flush something not chagned.?");
		g_assert(NULL);
		}
	if (db_iscachechanged(cache) == FALSE)
		return 1;

	db_toliet_checkfordefaults(obj, cache);
	/* db_toliet_debug(obj); */
	/* create the sql for adding the string to db. These statements 
	 * will create a query which can be run against sql */
	if (db_iscachenewrecord(obj, cache) == TRUE)
		{
		debugmsg("SQL Insert happened");
		cont = db_toliet_sqlinsert(obj, cache, &query);
		}
	else
		{
		/* update is done straight away. But insert is saved till
		 * later. */
		db_toliet_sqlupdate(obj, cache, NULL, NULL);
		}

	if (cont == 1)
		{
		if (db_obj_sqlwrite(query, obj->basetable, &id) == 1)
			{
			/* Re-adjust the idindex and possible obj index 
			 * cause the id just changed */
			db_cache_updateid(obj, id, cache);
			}
		else
			{
			mem_free(query);
			return -1;
			}
		mem_free(query);
		}
	/* clear cache */
	db_toliet_setwrote(cache);
	debugmsg("i've just done an insert on %s, with %d %d items there", 
			obj->basetable, db_numrecord(obj), obj->num);
	return 0;
	}

/**
 * db_toliet_flushobject:
 *
 * Write all objects to the database that have changed.  
 *
 * Returns: non-zero on failure
 */
gint
db_toliet_flushobject(Object * obj)
	{
	DbFlush *f;
	DbCache *c;
	GList *cachewalk;

	/* db_field_debug(obj); db_cache_debug(obj); */
	f = db_bureaucrat_createflush(obj);
	if (f == NULL)
		return -1;
	cachewalk = g_list_first(f->cachelist);
	while (cachewalk != NULL)
		{
		c = cachewalk->data;
		/* db_toliet_flush */
		db_toliet_flush(obj, c);
		/* debugwrap(obj, c, "db_toliet_flushobject"); */
		c->changed = FALSE;
		cachewalk = cachewalk->next;
		}
	obj->changed = FALSE;
	g_list_free(f->cachelist);
	mem_free(f);
	return 0;
	}

/**
 * db_toliet_sqlinsert:
 * @obj:
 * @c:
 * @query:
 *
 * Creates an SQL statement which is saved into @query.  Make sure you free it up after 
 * your done.  The sql statement is based on the contents of @c and @obj.
 *
 * Returns: non-zero on error
 */
gint
db_toliet_sqlinsert(Object * obj, DbCache * c, gchar ** query)
	{
	gchar *vstr = NULL, *str = NULL, *cval = NULL, *fval = NULL, *fieldparts = NULL, *vfieldparts = NULL;
	gchar *rubbish;
	gchar *fieldstr, *tablestr;
	gint cont = 0, i;

	*query = NULL;
	/* run some checks on the object */
	/* Renable this code for object fillins. It means if a value is invalid in a record set it will guess whats suppose
	   to go in there before writing it back to the database */
	/* db_makerecordvalid(c->obj); */

	/* build up insert string */
	for (i = 0; i < c->num; i++)
		{
		if (c->value[i] == NULL)
			{
			/* this is a null value so dont need to do anything */
			continue;
			}
		else
			{
			db_field_get(obj, i, &fieldstr, &tablestr);
			if (fieldstr != NULL)
				if ((tablestr == NULL || strcmp(tablestr, obj->basetable) == 0) &&
					strcmp(fieldstr,"oid") != 0)
					{
					if (cont == 0)
						{
						fval = mem_strdup_printf("%s", fieldstr);
						rubbish = db_sqlparse_dropquation(c->value[i]);
						cval = mem_strdup_printf("'%s'", rubbish);
						mem_free(rubbish);
						}
					else
						{
						fval = mem_strdup_printf(",%s", fieldstr);
						rubbish = db_sqlparse_dropquation(c->value[i]);
						cval = mem_strdup_printf(",'%s'", rubbish);
						mem_free(rubbish);
						}
					cont = 1;
					}
			}
		/* begginning part of string with the field names */
		if (fieldparts != NULL)
			{
			vfieldparts = g_strconcat(fieldparts, fval, NULL);
			g_free(fieldparts);
			}
		else
			vfieldparts = g_strdup(fval);
		mem_free(fval);
		fval = NULL;
		fieldparts = vfieldparts;
		/* second part of string with individual values */
		if (str != NULL)
			{
			vstr = g_strconcat(str, cval, NULL);
			g_free(str);
			}
		else
			vstr = g_strdup(cval);
		mem_free(cval);
		cval = NULL;
		str = vstr;
		}
	if (cont == 1)
		{
		/* join string together */
		*query = mem_strdup_printf("INSERT INTO %s (%s) VALUES (%s)", obj->basetable, vfieldparts, vstr);
		g_free(vstr);
		g_free(vfieldparts);
		}
	return cont;
	}

/**
 * db_toliet_sqlupdate:
 *
 * create an update query to play with 
 */
gint
db_toliet_sqlupdate(Object * obj, DbCache * c, gchar * tableopr, 
		GList *tablespassed)
	{
	gboolean firsttableparsed = FALSE, firstrec = FALSE;
	gchar *vstr = NULL, *str = NULL, *cval = NULL;
	gchar *rubbish, *fieldname, *tablename, *nexttable = NULL;
	gchar *query;
	gint cont = 0, i;
	DbUniqueId *id;
	GList *oidlist = NULL, *walk;
	DbField *oidfield = NULL;
	
	/* if top of recusion */
	if (tableopr == NULL)
		tableopr = obj->basetable;
	if (tablespassed == NULL)
		firstrec = TRUE;
	/* create the glist to remeber what we have done so no infinite loop */
	tablespassed = g_list_append(tablespassed,tableopr);
	/* build up update string */
	for (i = 0; i < c->num; i++)
		{
		if (c->value[i] == NULL)
			continue;
		
		rubbish = db_sqlparse_dropquation(c->value[i]);
		db_field_get(obj, i, &fieldname, &tablename);

		if (tablename != NULL && strcmp(tablename, tableopr) == 0 && 
		    fieldname != NULL)
			{
			firsttableparsed = TRUE;
			/* get the id if its important */
			if (obj->field[i]->isuniqueid == TRUE)
				{
				if (oidfield == NULL)
					oidfield = obj->field[i];
				oidlist = g_list_prepend(oidlist, c->value[i]);
				mem_free(rubbish);
				continue;
				}
			if (obj->field[i]->readonly == TRUE)
				{
				mem_free(rubbish);
				continue;
				}
			/* add the component, don't do the continue; thing. */
			if (cont == 0)
				cval = mem_strdup_printf("%s='%s'", 
						fieldname, rubbish);
			else
				cval = mem_strdup_printf(",%s='%s'", 
						fieldname, rubbish);
			mem_free(rubbish);
			}
		else if (nexttable == NULL && tablename != NULL 
			&& strcmp(tablename, tableopr) != 0
		        && firsttableparsed == TRUE)
			{
			for (walk=g_list_first(tablespassed);walk!=NULL;
				walk=walk->next)
				{
				/* break out of loop leaving walk non-null 
				 * if we have passed this table before */
				if (strcmp(walk->data,tablename) == 0)
					break;
				}
			if (walk == NULL)
				nexttable = tablename;
			mem_free(rubbish);
			continue;
			}
		else
			{
			mem_free(rubbish);
			continue;
			}

		cont = 1;
		if (str != NULL)
			{
			vstr = g_strconcat(str, cval, NULL);
			g_free(str);
			}
		else
			vstr = g_strdup(cval);
		mem_free(cval);
		str = vstr;
		}
	/* things good so far so do final step of building the query */
	if (cont == 1)
		{
		if (oidfield == NULL || oidlist == NULL)
			{
			errormsg("Unable to find a uniqueid to save field by.");
			db_obj_debug(obj);
			if (firstrec)
				g_list_free(tablespassed);
			return -2;
			}
		id = db_id_generateidfromfield(oidfield, oidlist);
		if (db_id_isvalid(id) == FALSE)
			{
			errormsg("Record uniqueid is invalid");
			if (firstrec)
				g_list_free(tablespassed);
			g_assert(NULL);
			return -1;
			}
		rubbish = db_id_createsql(obj, id);
		query = mem_strdup_printf("UPDATE %s SET %s WHERE %s", 
				tableopr, vstr, rubbish);
		mem_free(rubbish);
		g_free(vstr);
		db_id_free(id);

		db_obj_sqlwrite(query, tableopr, &id);
		db_id_free(id);
		mem_free(query);
		}
	if (oidlist != NULL)
		g_list_free(oidlist);
	/* generate queries for other components */
	if (nexttable)
		db_toliet_sqlupdate(obj, c, nexttable, tablespassed);
	
	if (firstrec)
		g_list_free(tablespassed);
	
	return cont;
	}

/* I removed some good comments here */

/**
 * db_toliet_setwrote: 
 * @cache: Database cache object
 *
 * Set the current cache object to say that it has been written to the database.
 *
 * Returns: non-zero on failure
 */
gint
db_toliet_setwrote(DbCache * cache)
	{
	if (cache->state == OBJ_NEW)
		cache->state = OBJ_EDITNEW;
	cache->changed = FALSE;

	/* Todo: return based on success or failure */
	return 1;
	}

/**
 * db_toliet_checkfordefaults:
 * @obj: database object it came from
 * @cache: cache to check
 *
 * Look to see if anything uses the name EXECFUNCTION and then excute a function
 * if it exists
 * 
 * Returns: non-zero on error
 */
gint
db_toliet_checkfordefaults(Object * obj, DbCache * cache)
	{
	gint i;
	DbField *field;

	if (obj->numfield != cache->num)
		errormsg("I'm going to segfault");

	for (i = 0; i < cache->num; i++)
		{
		/* its a function, run it */
		if (cache->value[i] != NULL)
			if (strcmp(cache->value[i], "EXECFUNCTION") == 0)
				{
				field = obj->field[i];

				if (field->fielddef->defaultvalue != NULL && strlen(field->fielddef->defaultvalue) > 0)
					{
					mem_free(cache->value[i]);
					cache->value[i] = db_default_execfunction(field->fielddef->defaultvalue);
					/* debugmsg("replacing %s with value %s",field->field,cache->value[i]); */
					}
				}
		}
	return 0;
	}
