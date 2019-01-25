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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <glib.h>

#include "bc.h"
#include "db.h"
#include "dbobject.h"
#include "dbwrapper.h"
#include "dbgather.h"
#include "dbconstraint.h"
#include "dbmapme.h"
#include "dbsqlparse.h"
#include "dbobjectvalid.h"
#include "dbbureaucrat.h"
#include "dbtoliet.h"
#include "dbuniqueid.h"
#include "dbobjectnav.h"
#include "dbfield.h"
#include "dbbirth.h"
#include "dbcache.h"
#include "dbverifysql.h"
#include "dbmasteroftable.h"

/* Not part of hte main db_API.  But a side function
   If a record is new, and hasn't been in database then this function
   will go and blonk it into the database 
*/

/* Write ahead cache
   flush out all writes to db and free cache. 
	This function has been re-written way to much and is such a fundemental one to the whole
	project. Recode with caution, many a dead programme scatter this lines of code.... . */

/**
 * db_obj_addwrite:
 * @obj: database object
 * @field: fieldname to write to
 * @value: value to write to that field
 * @mark: whether or not to leave a mark on the object that you wrote.  Set 
 * to TRUE normally.
 *
 * For the given @obj add a write to the @field at the current row the object 
 * is on.  The record must have a table name supplied to it on creation in 
 * order to write back.  This is a low level function, you should generally 
 * use db_setvalue().  Also note this wont write back straight away but
 * at some time when it thinks it appropriate.  The @mark is for things like 
 * default values where you want to save a value but you want to wait till 
 * you get some real data before writing back and this is just one of those 
 * optional extra values to have.
 *
 * Returns: non-zero on failure.
 */
gint
db_obj_addwrite(DbObject * obj, gchar * field, gchar * table, gchar * value,
                gboolean mark)
	{
	gint fieldpos;
	DbCache *cache;

	g_assert(obj);
	g_assert(field);

	/*	if (value != NULL && strlen(value) == 0)
			value = NULL; */

	/* debugmsg("Write to cache of %s.%s of value %s", obj->basetable,
	 field, value); */
	cache = db_cache_isincache(obj);
	if (cache == NULL)
		{
		/* This is the first write to this object so create a cache 
		 * for it. */
		cache = db_cache_moveto(obj);
		/* m.essage("%s.%s: Adding writeid of
		   %d",obj->basetable,field,obj->id->pg_oid); */
		/* State the object is new, or edited or whatever */
		if (db_id_isnewrecord(obj->id) == TRUE)
			cache->state = OBJ_NEW;
		else
			cache->state = OBJ_EDITREAD;
		}
	/* hmm mark. If its set to mark it then mark it. */
	if (mark == TRUE)
		{
		/* record that you did some write... so if you have an object
		 * previously in cache you dump it. */
		db_bureaucrat_recordwrite(obj->objectinstance);
		/* debugmsg("Marking object %s with a write", obj->basetable); */
		obj->changed = TRUE;
		cache->changed = TRUE;
		}
	/* now we actually going to do it.. */
	fieldpos = db_cache_setvalue(obj, cache, field, table, value);
	/* check to see if this change effects anything else
	 * which values need changing */
	if (mark)
		{
		/* Free the previous list of changed variables */
		if (obj->fieldschanged)
			g_list_free(obj->fieldschanged);
		/* In obj->fieldschanged a list fields that are effected by 
		 * the change */
		obj->fieldschanged = db_masteroftable_actionchange(
			obj,obj->field[fieldpos],value);
		}
	
	return fieldpos;
	}

/**
 * db_obj_doread:
 * @obj: Database object
 * @field: name of field to read
 * @value: value to return
 *
 * This shoulnd't be called directly, but what it does is check to see if a 
 * value is in cache, and when and if its not then it will read it from the 
 * database.  This is done on your currently selected row in the database, 
 * use db_moveto() to navigate the recordset.  Don't free @value when
 * your done.
 *
 * Returns: non-zero on failure
 */
gint
db_obj_doread(DbObject * obj, gchar * field, gchar * table, gchar ** value)
	{
	gint retval, fieldpos;
	DbCache *cache;
	DbField *fielddef;

	g_assert(obj);
	g_assert(field);
	obj->dead = FALSE;
	*value = NULL;
	/* check cache */
	cache = db_cache_isincache(obj);
	/* db_obj_debug(obj); */
	/* check if we have the record cached */
	if (cache != NULL)
		{
		/* m.essage("checking in cache. hmm. row is %d, orig row is %d",
		   obj->row, cache->origrow); */
		/* Read the item from the cache */
		retval = db_cache_getvalue(obj, cache, field, table, value);

		/* m.essage("returning value. hmm. %d, %s", retval, *value); */
 		if (*value == NULL)
			{
			/* If the record is in fact blank, return that as the 
			 * value */
			if (cache->state == OBJ_NEW || 
				cache->state == OBJ_EDITNEW)
				return retval;
			/* Continue on with reading from the record set to 
			 * load the cache */
			}
		else
			{
			/* Its a function, run the function to get the value */
			if (strcmp(*value, "EXECFUNCTION") == 0)
				{
				debugmsg("This is a default value field (%s) "
					"you are getting the value from", 
					field);
				fielddef = db_field_dbfind(obj, field, table);
				if (fielddef->fielddef->defaultvalue != NULL)
					{
					*value = db_default_execfunction(
						fielddef->fielddef->defaultvalue);
					db_cache_setvalue(obj, cache, field, 
							table, *value);
					mem_free(*value);
					db_cache_getvalue(obj, cache, field, 
							table, value);
					}
				debugmsg("returning %s for default value", 
						*value);
				}
			return retval;
			}
		}
	/* no data in there */
	if (db_isnewrecord(obj) == TRUE)
		{
		*value = NULL;
		return 1;
		}

	/* do a normal read */
	retval = db_field_read(obj, field, table, value);

	/* m.essage("read %s from %s.%s",*value,table,field); */
	/* if your in a filtered object, then save everything you read to 
	 * cache */
	if (cache == NULL) /* && obj->filtered == TRUE) */
		cache = db_cache_moveto(obj);

	if (cache != NULL)
		{
		/* assumes the state is defaulted to a OBJ_NEW */
		if (cache->state == OBJ_NEW)
			cache->state = OBJ_READ;
		if ((fieldpos = db_cache_setvalue(obj, cache, 
			field, table, *value)) >= 0)
			* value = cache->value[fieldpos];
		}
	return retval;
	}

/**
 * db_obj_sqlread:
 *
 * Awaiting documentation patrol.
 *
 * Returns: Salvation
 */
gint
db_obj_sqlread(DbObject * obj, gchar * sql)
	{
	g_assert(obj);
	g_assert(sql);

	if (obj->res == NULL && obj->statement == NULL)
		{
		/* run query */
		db_verifysql_parse(obj, sql);
		if (obj->query == NULL)
			{
			errormsg("Failed to modify SQL statement to include "
				"keys, %s", sql);
			return -2;
			}
		}
	/* if your running object in several parts. */
	if (obj->sqlready == FALSE)
		return 2;

	if (db_bureaucrat_checkpendingwrites(obj) > 0)
		debugmsg("had to write some stuff before i could run "
			"this sql.");

	/* check number of records loaded, as well as record number loaded */
	if (obj->res)
		db_dbclear(obj->res);
	obj->res = db_dbexec(globaldbconn, obj->query);
	debugmsg("%s", obj->query);
	/* check for errors */
	if (db_dbcheckresult(obj->res) != 0)
		{
		errormsg("error in running query %s", obj->query);
		obj->res = NULL;
		return -2;
		}
	/* obj->base = obj->res->basetable; */
	if ((obj->num = db_dbnumrows(obj->res)) <= 0)
		{
		/* This needs to be relooked at so that an empty object is 
		 * created when you have zero results */
		debugmsg("No records found for loading %s", obj->query);
		/* db_obj_clear(obj); */
		if (obj->res != NULL)
			{
			/* if (obj->freeresult == TRUE) */
			db_dbclear(obj->res);
			obj->res = NULL;
			}
		db_obj_handle_empty_recordset(obj);
		return 1;
		}
	/* Assign record defaults to a read, data unchanged */
	obj->newrecord = FALSE;
	obj->freeresult = TRUE;
	obj->changed = FALSE;
	obj->unknownid = FALSE;
	obj->filtered = FALSE;
	/* I suspect this code is buggy so i'm putting a reminder that I need 
	 * to get a hair cut because i'll visit here often */
	obj->readonly = FALSE;
	/* populate that object, Yeaaah! */
	db_moveto(obj, 0);
	/* Run some tests */
	/* commented out 2005-01, causes paymaster to fail on displaying
	 * journals. */
	if (db_isnewrecord(obj) == TRUE)
		g_assert(NULL); 
	return 0;
	}

/**
 * db_obj_sqlwrite:
 * @query: An insert or update query string
 * @targettable: The table your running the query on
 * @id: If its an insert then you want to record the new id for the object.
 *
 * Execture a insert or a update, this is used in internally by the flushing.
 * You shoulnd't need to call this function.
 *
 * Returns: negitive on failure, 1 on insert, 0 on update.
 */
gint
db_obj_sqlwrite(gchar * query, gchar * targettable, DbUniqueId ** id)
	{
	gint retval = 0;
	DbRecordSet *res;

	g_assert(query);

	*id = NULL;
	/* execute the write query */
	debugmsg("%s", query);
	res = db_dbexec(globaldbconn, query);
	/* check result */
	if (db_dbcheckresult(res) != 0)
		{
		/* clean up the odd ends for the object */
		db_dbclear(res);
		return -1;
		}
	/* If you ran an insert re-get the uniqueid and record it in the 
	 * cache for future reference. */
	if ((*id = db_uniqueid(globaldbconn, res)) != NULL)
		retval = 1;

	db_dbclear(res);
	return retval;
	}

/**
 * db_obj_test:
 * @obj: Database object
 *
 * Run a series of tests on an object to make sure it is not courpt.
 *
 * Returns: non-zero on failure.
 */
gint
db_obj_test(DbObject * obj)
	{
	gint f = 0, i;

	/* Disabled to improve speed */
	return 0;

	if (obj == NULL)
		errormsg("%d, object is null", f++);
	if (obj->basetable == NULL)
		errormsg("%d,object can not be written back to db.", f++);

	/* if (obj->res == NULL && obj->query != NULL) 
	  	errormsg("%d,object has no
	   recordset", f++); */
	if (obj->id == NULL)
		errormsg("%d,id is null", f++);
	if (obj->row < 0)
		errormsg("%d,row is invalid", f++);
	if (obj->cache == NULL && obj->changed == TRUE)
		warningmsg("%d,cache is null, but the object has changed", f++);

	for (i = 0; i < obj->numcache; i++)
		{
		if (obj->cache[i]->origrow == obj->row)
			if (db_id_compare(obj->cache[i]->id, obj->id) != 0)
				errormsg("%d, cache is courpt.", f++);
		}
	f += db_field_test(obj);
	return (0 - f);
	}

/**
 * db_obj_create:
 * @name: Optional name of a table
 *
 * Creates a blank empty object which can then used for reading or writing 
 * into.  This is internal code.
 *
 * Returns: Newly created database object
 */

DbObject *
db_obj_create(gchar * tablename)
	{
	DbObject *obj;

	obj = (DbObject *) mem_alloc(sizeof(DbObject));
	memset(obj, 0, sizeof(DbObject));

	obj->objectinstance = -1;

	if (tablename == NULL)
		{
		obj->readonly = TRUE;
		obj->basetable = NULL;
		}
	else
		{
		obj->readonly = FALSE;
		obj->basetable = mem_strdup(tablename);
		}

	obj->res = NULL;
	obj->row = 0;
	obj->num = 0;
	obj->numfield = 0;

	obj->freeresult = FALSE;
	obj->newrecord = TRUE;
	obj->unknownid = TRUE;
	obj->sqlready = TRUE;
	obj->changed = FALSE;
	obj->filtered = FALSE;

	obj->sqlgood = TRUE;
	obj->fulldbsupport = TRUE;

	obj->query = NULL;
	obj->mapobject = NULL;
	obj->field = NULL;
	obj->birth = NULL;
	obj->fieldschanged = NULL;

	obj->cache = NULL;
	obj->currentcache = NULL;
	obj->id = NULL;
	obj->numcache = 0;

	/* Just to keep the proper authorities happy */
	db_bureaucrat_add(obj);

	return obj;
	}

/**
 * db_obj_free:
 *
 * Lets do some cleanup. always a good idea you know.
 *
 * Returns: non-zero on failure
 */
gint
db_obj_free(DbObject * obj)
	{
	if (obj == NULL)
		return 1;

	db_obj_clear(obj);

	if (db_bureaucrat_deleteinstance_byobject(obj) != 0)
		{
		errormsg("You are attempting a double free here. Instance "
			"id %d. I have to abort.", obj->objectinstance);
		g_assert(NULL);
		return -1;
		}

	/* to see if everything is ok with the object */
#ifdef _SELFTEST
	db_obj_test(obj);
#endif

	/* Clean up record */
	if (obj->res != NULL)
		{
		if (obj->freeresult == TRUE)
			db_dbclear(obj->res);
		obj->res = NULL;
		}

	if (obj->query != NULL)
		mem_free(obj->query);
	if (obj->basetable != NULL)
		mem_free(obj->basetable);
	db_freemapobject(obj);
	memset(obj, 0, sizeof(DbObject));

	mem_free(obj);
	obj = NULL;
	return 0;
	}

/**
 * db_obj_clear:
 *
 * Frees up an object nearly, As in blanks it out so that its a fresh empty 
 * object, ready to be used again.
 */

/* blanks an object out, not freeing it but making it quivilant to a new blank 
 * record */
gint
db_obj_clear(DbObject * obj)
	{
	gint i;

	g_assert(obj);

	for (i = 0; i < obj->numfield; i++)
		{
		if (obj->field[i]->birthvalue)
			mem_free(obj->field[i]->birthvalue);
		obj->field[i]->birthvalue = NULL;
		}

	db_toliet_flushobject(obj);
	if (obj->res != NULL)
		{
		if (obj->freeresult == TRUE)
			db_dbclear(obj->res);
		obj->res = NULL;
		}
	if (obj->query != NULL)
		mem_free(obj->query);
	if (obj->mapobject != NULL)
		db_freemapobject(obj);
	if (obj->statement != NULL)
		sql_destroy(obj->statement);
	if (obj->fieldschanged)
		g_list_free(obj->fieldschanged);

	db_filter_freeapplied(obj);
	db_birth_free(obj->birth);
	db_cache_cleanup(obj);
	db_field_freeall(obj);

	obj->query = NULL;
	obj->newrecord = TRUE;
	obj->freeresult = FALSE;
	obj->changed = FALSE;
	obj->unknownid = TRUE;
	obj->filtered = FALSE;
	obj->field = NULL;
	obj->row = -1;
	obj->num = 0;
	obj->id = NULL;
	obj->numfield = 0;
	obj->mapobject = NULL;
	obj->statement = NULL;
	obj->fieldschanged = NULL;

	obj->cache = NULL;
	obj->currentcache = NULL;
	obj->birth = NULL;

	return 0;
	}

/**
 * db_obj_applydefaults:
 * @obj: Database object to apply defaults to.
 *
 * This will apply a bunch of default and start up values to an object.  The 
 * object will be recorded as not changed so it wont save back the changes 
 * unless you change something yourself. This is handy if you want to create 
 * a object with a bunch of values but not save it until you really need to. 
 * The birth is also used to correctly allocate values for reference variables,
 * ie make sure specific ids are set. This function is called by db_clear_obj.  
 *
 * Returns: non-zero on error
 */
gint
db_obj_applydefaults(DbObject * obj)
	{
	return db_default_populateobject(obj);
	}

/**
 * db_obj_handle_empty_recordset:
 * @obj: Database object with a empty recordset
 *
 * If you fail to load any items (ie no items in the database) this function 
 * can be used to create a blank record.
 *
 * Returns: non-zero on failure
 */
gint
db_obj_handle_empty_recordset(DbObject * obj)
	{
	gint retval = 0;

	g_assert(obj);
	if (db_isrecordset(obj) == TRUE)
		{
		warningmsg("DbObject %s isn't  empty.", obj->basetable);
		return -1;
		}
	obj->num = 0;
	obj->row = 0;
	obj->newrecord = FALSE;
	/* Remeber to move to postion zero when calling this function */
	/* Populate it in with field names. */

	/* this code is commented out so that it has a empty record set with 
	 * 0 items */
	/* retval -= db_field_populate_singletable(obj); */

	/* retval -= db_moveto(obj, 0); retval -= db_obj_applydefaults(obj); */
	return retval;
	}

/**
 * db_obj_handle_new_recordset:
 * @obj: Database class object to make a new record
 *
 * Assuming your starting a record set and you want one blank item
 * to begin with.  Default values are added.  A db_moveto(obj,0)
 * is called to realign the object correctly.
 */
gint
db_obj_handle_new_recordset(DbObject * obj)
	{
	gint retval = 0;

	g_assert(obj);

	if (db_isrecordset(obj) == TRUE)
		{
		warningmsg("DbObject isn't %s empty.", obj->basetable);
		return -1;
		}

	obj->num = 1;
	obj->newrecord = TRUE;
	/* Remeber to move to postion zero when calling this function */
	/* Populate it in with field names. */
	retval -= db_field_populate_singletable(obj);
	retval -= db_moveto(obj, 0);
	retval -= db_obj_applydefaults(obj);
	return retval;
	}

/**
 * db_obj_dodelete:
 * @obj: database object
 *
 * This function is used primarly by db_deleteobject().  
 * Yes i know these functions are similar, this is a lower level one supposedly.  
 *
 * Returns: non-zero on error
 */
gint
db_obj_dodelete(DbObject * obj)
	{
	DbRecordSet *res;
	gint newpos;
	gchar *query, *idsql;

	g_assert(obj);
	if (obj->id == NULL)
		return -1;
	if (db_id_isnewrecord(obj->id) == FALSE)
		{
		/* m.essage("Id is %d, row %d/%d, id in cache %d",
		 obj->id->pg_oid,obj->row,obj->numcache,
		 obj->currentcache->id->pg_oid); */
		/* Get me an sql statement */
		idsql = db_id_createsql(obj, obj->id);
		query = mem_strdup_printf("DELETE FROM %s WHERE %s", 
				obj->basetable, idsql);
		mem_free(idsql);
		debugmsg("Deleting record.\n%s",query);
		/* run that sql statement */
		res = db_dbexec(globaldbconn, query);
		/* Tidy up */
		if (db_checkpgresult(res) != 0)
			{
			errormsg("error in deleting DbObject*");
			db_dbclear(res);
			mem_free(query);
			return -1;
			}
		mem_free(query);
		db_dbclear(res);
		}
	/* Delete item from the cache */
	/* m.essage("DELETEING CACHE, it was %d",obj->numrow); */
	db_filter_deletedrow(obj, obj->row);
	db_cache_delete(obj); 
	db_obj_refresh(obj);
	
	/* message("Done refresh");
	if (obj->num > 0)
	{
	gchar *value;
	value = db_dbgetvalue(obj->res,obj->row,db_dbnumrows(obj->res));
	message("got id %s, %d/%d-%d num",value,obj->row,obj->num,db_dbnumrows(obj->res));
	} */
	
	/* m.essage("DELETEING CACHE, its now %d",obj->numrow); */
	/* Move everything along and tidy up */

	/* Francis: Replace back debug: line with next following if-else block 
	 * if it cause any problems. If Francis's logic is correct, if it was 
	 * indicating the last row and it was deleted, then it moves the one 
	 * before. Otherwise just stay there. */
	/* debug: newpos = obj->row - 1; */
	if (obj->row == obj->num - 1)
		{
		newpos = obj->row - 1;
		}
	else
		{
		newpos = obj->row;
		}

	if (newpos < 0)
		newpos = 0;
	/* obj->num--; */
	if (obj->row < 0)
		obj->row = 0;
	if (obj->num <= 0)
		{
		/* db_boj_setdead(obj,TRUE); */
		/* m.essage("failed on moveto"); */
		/* this was uncommented 2004-10, but, its needed
		 * for being able to add to a list after you've deleted 
		 * from it */
		/* db_obj_clear(obj); */ /* Uncommented 2004-10 */ 
		db_obj_handle_empty_recordset(obj);
		}
	else
		{
		db_moveto(obj, newpos);
		/* m.essage("moveto succesful to position 0"); */
		}
	/* if (obj->id && obj->currentcache && obj->currentcache->id)
message("After delete id is %d, row %d/%d-%d, id in cache %d",obj->id->pg_oid,obj->row,obj->numcache,obj->num, obj->currentcache->id->pg_oid);
	else message("After delete no currentcache"); */
	return 0;
	}

gint
db_obj_debug(DbObject * obj)
	{
	debug_output("Num rows: %d Current Row: %d Unfiltered Num %d", obj->num,
	             obj->row, db_numrecord(obj));
	db_cache_debug(obj);
	return 0;
	}

/**
 * db_obj_refresh:
 *
 * Rerun any queries on @obj because we think things have changed in the 
 * backend sql database and you need to take a fresh perspective on life. 
 * oh no i mean the data souce.
 */
gint
db_obj_refresh(DbObject * obj)
	{
	gint oldpos;

	g_assert(obj);

	db_toliet_flushall();
	/* warningmsg("this code may not work properly"); */

	/* if (db_isnewrecord(obj) == TRUE || obj->query == NULL)
	   db_obj_loadself(obj); */
	if (!obj->query)
		return -1;

	oldpos = obj->row;
	if (db_obj_sqlread(obj,obj->query) < 0)
		return -2;

	if (oldpos >= 0 && oldpos < obj->num)
		;
	if (obj->num > 0)
		db_moveto(obj, oldpos);
	return 0;
	}

/**
 * db_obj_compareid:
 * @scan: 
 * @mainobj: 
 *
 * Figure out if 2 database objects are pointing to the same field/table/row.
 */
gint
db_obj_compareid(DbObject * search, DbObject * mainobj, gchar * field,
                 gchar * table, gboolean compareall, gint * row)
	{
	gint num, i, found = -1;
	DbUniqueId *id;

	if (compareall == FALSE)
		{
		if (db_id_compare(search->id, mainobj->id) == 0)
			return TRUE;
		else
			return FALSE;
		}
	num = db_numrecord(mainobj);

	for (i = 0; i < num; i++)
		{
		id = db_id_getbyrow(mainobj, i);
		if (db_id_compare(id, search->id) == 0)
			{
			found = i;
			break;
			}
		}
	*row = found;
	if (found == -1)
		return FALSE;
	else
		return TRUE;
	}

/** 
 * db_obj_addloadby:
 * @destobj: Object to load results into.
 * @srcobj: Where you load by.
 * 
 * Returns: 1 if you need to free ret_value else 0 
 */
gint
db_obj_addloadby(DbObject * destobj, DbObject * srcobj,
                 sql_statement * statement, gchar ** ret_ltable,
                 gchar ** ret_lfield, gchar ** ret_value)
	{
	gint retval = 0;
	gboolean cont = TRUE, selfref = FALSE;
	gchar *tmpstr, *value = NULL;
	gchar *ltable = NULL, *rtable = NULL, *lfield = NULL, *rfield = NULL;
	GList *desttablelist, *srctablelist, *walk, *srcwalk, *creflist;
	DbConstraintDef *cref;
	DbDatabaseDef *dbdef;
	DbTableDef *dbtabledef;

	*ret_ltable = NULL;
	*ret_lfield = NULL;
	g_assert(statement);
	mem_verify(srcobj);
	if (srcobj->statement == NULL || srcobj->query == NULL)
		return -1;
	dbdef = db_get_current_database();
	creflist = dbdef->constraints;
	/* get all the tables in sql statemenet */
	desttablelist = sql_statement_get_tables(statement);
	srctablelist = sql_statement_get_tables(srcobj->statement);
	/* get destination table and fieldname */
	for (walk = g_list_first(desttablelist); walk != NULL; 
			walk = walk->next)
		{
		for (srcwalk = g_list_first(srctablelist); srcwalk != NULL;
		        srcwalk = srcwalk->next)
			{
			/* check if tables overlap each other. Ie a 
			 * subselection of self. */
			if (((gchar *) walk->data)[0] != '*'
			        && strcasecmp(walk->data, srcwalk->data) == 0)
				{
				dbtabledef = db_findtable(srcwalk->data);
				g_assert(dbtabledef);
				rtable = dbtabledef->name;
				value = db_id_getuniqueid_asstring(
					srcobj, rtable, &rfield);
				selfref = TRUE;
				cont = FALSE;
				retval = 1;
				break;
				}
			cref = db_constraint_getonmatch(creflist, 
					walk->data, srcwalk->data, 0);
			if (cref)
				{
				if (strcasecmp((gchar *) walk->data, 
					cref->table[0]) == 0)
					{
					ltable = cref->table[0];
					rtable = cref->table[1];
					lfield = cref->column[0];
					rfield = cref->column[1];
					}
				else if (strcasecmp((gchar *) walk->data, 
					cref->table[1]) == 0)
					{
					ltable = cref->table[1];
					rtable = cref->table[0];
					lfield = cref->column[1];
					rfield = cref->column[0];
					}
				else
					errormsg("Invalid cref. Cant not do "
						"this");

				cont = FALSE;
				break;
				}
			}
		if (cont == FALSE)
			break;
		}
	if (!rtable)
		{
		warningmsg("Unable to load object by object due to no "
			"relationship between them");
		return -1;
		}
	/* check if the table already exists */
	cont = TRUE;
	for (walk = g_list_first(desttablelist); walk != NULL; 
		walk = walk->next)
		{
		if (strcasecmp(walk->data, rtable) == 0)
			{
			cont = FALSE;
			break;
			}
		}
	/* free table lists */
	sql_statement_free_tables(desttablelist);
	sql_statement_free_tables(srctablelist);
	if (cont == TRUE)
		{
		/* modify the sql statement */
		sql_statement_append_tablejoin(statement, ltable, rtable, 
				lfield, rfield);
		}
	/* add the where field=value bit at the end. */
	if (selfref == FALSE)
		if (db_getvalue(srcobj, rfield, rtable, &value) != 0)
			{
			errormsg("Unable to extract %s.%s from a source table",
				rtable, rfield);
			return -2;
			}
	if (db_isnewrecord(srcobj) == TRUE)
		{
		db_setvalue(srcobj, rfield, rtable, value);
		}

	tmpstr = mem_strdup_printf("%s.%s", rtable, rfield);
	/* message("got %s.%s = %s for appendng.",rtable,rfield,value);
	   db_cache_debug(srcobj); */
	sql_statement_append_where(statement, tmpstr, value, SQL_and, SQL_eq);
	mem_free(tmpstr);
	*ret_ltable = ltable;
	*ret_lfield = lfield;
	*ret_value = value;
	/* done. */
	return retval;
	}

DbObject *
db_obj_loadself(DbObject * obj)
	{
	gchar *id, *query;
	DbObject *retobj = NULL;

	g_assert(obj->basetable);
	/* Clear any old record set that might be there. normally this not 
	 * here */
	if (db_id_isnewrecord(obj->id))
		db_toliet_flushobject(obj);
	if (db_id_isnewrecord(obj->id))
		{
		errormsg("Massive problem with object with basetable %s here",
		         obj->basetable);
		errormsg("I have failed to place this object back into the db");
		return NULL;
		}
	/* Create SQL statement */
	id = db_id_createsql(obj, obj->id);

	query = mem_strdup_printf("SELECT * FROM %s WHERE %s", 
			obj->basetable, id);
	mem_free(id);
	retobj = db_sql(NULL, query);
	mem_free(query);
	return retobj;
	} 

/**
 * db_obj_setdead:
 * @obj: Database object.
 * @state: %TRUE/%FALSE state to set of the object.
 * 
 * Sets a database @obj to be dead. Which means nothing much
 * will happen to it. Its just an empty clear object not containing
 * any data and wont let you change anything.
 */
void
db_obj_dead(DbObject * obj, gboolean state)
	{
	g_assert(obj);
	obj->dead = state;
	}
