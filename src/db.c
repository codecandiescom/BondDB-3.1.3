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

#include "bcdebug.h"
#include "dbwrapper.h"

#include "db.h"
#include "dbclient.h"
#include "dbgather.h"
#include "dbobject.h"
#include "dbconstraint.h"
#include "dbtoliet.h"
#include "dbuniqueid.h"
#include "dbbureaucrat.h"
#include "dbsuggest.h"
#include "dbbirth.h"
#include "dbobjectnav.h"
#include "dbobjectdel.h"
#include "dbsqlparse.h"
#include "dbverifysql.h"
#include "dbmasteroftable.h"
#include "dbsuggest.h"

static gchar *
trimstring(gchar * str)
	{
	gchar *retstr;
	gint i, k = 0, slen;
	retstr = mem_strdup(str);
	slen = strlen(str);
	g_assert(str);
	for (i = 0; i < slen; i++)
		{
		if (k == 0 && str[i] < 65)
			continue;
		retstr[k] = str[i];
		k++;
		}
	retstr[k] = 0;
	return retstr;
	}

/**
 * db_getvalue:
 * @obj: Database object
 * @field: Field name
 * @table: a class to look up in, or NULL if the current class.
 * @retval: Where to set the string after i've got the value.
 *
 * Toplevel function.  Retreives information from the database. 
 *
 * Returns: non-zero on failure.
 */
gint
db_getvalue(Object * obj, gchar * fieldname, gchar * tablename, gchar ** retval)
	{
	gint rval, pos;

	mem_verify(obj);
	*retval = NULL;
	if (obj->dead)
		return -3;
	if (obj->row >= obj->num || obj->row < 0)
		{
		debugmsg("Unable to read, your outside of range for your "
			"current row. (%d of %d)", obj->row, obj->num);
		return -2;
		}
	/* Establish if field is a remote location. */
	pos = db_field_getpos(obj, fieldname, tablename);
	if (pos == -1)
		{
		if (db_verifysql_addfieldtable(obj, fieldname, tablename) == 0)
			{
			if (obj->res != NULL)
				db_obj_sqlread(obj, obj->query);
			}
		else
			return -1;
		}

	rval = db_obj_doread(obj, fieldname, tablename, retval);
	if (rval < 0)
		warningmsg("error was encounted");

	return rval;
	}

/* create an empty object ready for stuff to be done to it */

/**
 * db_createobject:
 * @tablename: Name of a table where the record will be inserted
 *
 * Will create a fresh new database object ready for use.  If %NULL is passed 
 * into @tablename then this record isn't used for updating back to the 
 * database, just reading.
 *
 * Returns: A newly created database object, or %NULL on error.
 */
Object *
db_createobject(gchar * tablename)
	{
	Object *obj;
	gchar *sql;

	obj = db_obj_create(tablename);
	if (tablename)
		{
		obj->numfield = db_numfields(obj);
		db_obj_handle_new_recordset(obj);
		sql = mem_strdup_printf(
		    "SELECT %s.* FROM %s ORDER BY %s.oid",
		    tablename,tablename,tablename);
		db_verifysql_parse(obj,sql);
		mem_free(sql);
		}

	/* Arr ok this should do stuff now.. . */
	return obj;
	}

/**
 * db_freeobject:
 * @obj: Database object
 *
 * Free up a database object, both clean up recordsets and all allocated memory. It will also
 * be removed from the bureaucrats records.
 */
void
db_freeobject(Object * obj)
	{
	db_obj_free(obj);
	}

/**
 * db_sql:
 * @obj:
 * @sql:
 *
 * Run a SQL statement on the backend database and save the results into a Object
 *
 * Returns: returns @obj or a newly created object if @obj is %NULL
 */

/* note, request the oid field last in the list, ie select * from ... */
Object *
db_sql(Object * obj, gchar * sql)
	{
	gint typeofsql;
	gchar *retquery;
	gchar *targettable;
	DbRecordSet *res;
	DbUniqueId *id;
	extern DbConnection *globaldbconn;

	g_assert(sql);

	retquery = trimstring(sql);
	typeofsql = db_sqlparse_typeofsql(retquery);
	/* run the query */
	if (typeofsql == 0)
		{
		/* if you pass in NULL it'll create a brand new spanking object and 
		 * then do some fancy stuff on it to make it safe */
		if (obj == NULL)
			obj = db_obj_create(NULL);
		else 
		        db_obj_clear(obj);
		db_obj_sqlread(obj, retquery);
		}
	else
		{
		/* do a write */
		if (obj == NULL)
			{
			res = db_dbexec(globaldbconn, retquery);
			db_dbclear(res);
			mem_free(retquery);
			return NULL;
			}
		targettable = db_sqlparse_getfirsttable(retquery);
		db_obj_sqlwrite(retquery, targettable, &id);
		/* db_id_updateidindex(obj, id, cache->origrow); */
		}
	mem_free(retquery);
	/* return object */
	return obj;
	}

/* load everything in a table. oh joy */

/**
 * db_loadall:
 * @obj: Database object to load results into
 * 
 * Load all fields in a table into an object
 * 
 * Returns: non-zero on failure
 */
gint
db_loadall(Object * obj)
	{
	gchar *query;
	gint retval;

	mem_verify(obj);
	query = mem_strdup_printf("SELECT * FROM %s", obj->basetable);
	db_obj_clear(obj);
	retval = db_obj_sqlread(obj, query);
	mem_free(query);
	return retval;
	}

/* marika needs to get ... supermarket, i from from supermarket, ...
   bazmarty price.
   tomota sauce
   pipped prunes
   potato pomps - frozen ones
   sascuages
   deviled sausages sauce in a can
   chicken stock. - real liquid stock.
*/

/**
 * db_loadobjectbyobjects:
 * @destobj:
 * @srcobjs:
 * 
 * Modify destobj sql statement to be subset of @srcobjs. So a where
 * clause will be added for each srcobjs statement and a birth will be generated.
 */
gint
db_loadobjectbyobjects(Object * destobj, GList * srcobjs)
	{
	Object *obj, *oldobj;
	GList *walk, *tofree = NULL, *tofreestr = NULL;
	gchar *value, *ltable, *lfield, *sql;
	GList *ltablelist = NULL, *lfieldlist = NULL, *valuelist = NULL;
	gint retval = 0, i, check;
	sql_statement *statement;

	mem_verify(destobj);
	g_assert(srcobjs);
	if (!destobj->query)
		{
		errormsg("No sql query is associated with the destination "
			"object %s", destobj->basetable);
		return -1;
		}
	
	statement = sql_parse(destobj->query);
	/* Go though src objects. Save any record that needs it. Add them to 
	 * the scheme of things */
	for (walk = g_list_first(srcobjs); walk != NULL; walk = walk->next)
		{
		obj = walk->data;
		mem_verify(obj);
		/* You can't display a object from a non-saved object. */
		if (db_isnewrecord(obj) == TRUE)
			{
			if (db_isrecordchanged(obj) == FALSE 
				|| db_isdeadobject(obj) == TRUE)
				{
				warningmsg("Record %s is a new empty one and "
					"has not changed so i will abort", 
					obj->basetable);
				db_obj_clear(destobj); /* Code switched 8/04. 
				Dont want to mark it as dead just empty */
				/*db_obj_setdead(destobj, TRUE); */
				return -2;
				}
			/* flush the buffer if its waiting to be writtened */
			db_toliet_flushobject(obj);
			debugmsg("%s is a new record, I am flushing it so i "
				"can use it properly.", obj->basetable);
			oldobj = obj;
			obj = db_obj_loadself(oldobj);
			tofree = g_list_append(tofree, obj);
			retval = 1;
			}
		if (obj == NULL)
		     {
		     warningmsg("Obj is null so aborting.");
		     return -3;
		     }
		   
		check = db_obj_addloadby(destobj, obj, statement, &ltable, 
				&lfield, &value);
		if (check < 0)
			{
			warningmsg("Record %s is not quite how it should be "
				"so I will abort", obj->basetable);
				db_obj_clear(destobj); 		
				return -4;
			}
		if (lfield && ltable)
			{
			ltablelist = g_list_append(ltablelist, ltable);
			lfieldlist = g_list_append(lfieldlist, lfield);
			valuelist = g_list_append(valuelist, value);
			}
		/* m.essage("doing loadby %s %s.%s got value %s",obj->basetable,ltable,lfield,value); */
		/* if db_obj_addloadby returns 1 you need to free value */
		if (check == 1)
		     tofreestr = g_list_append(tofreestr,value);
		}
	/* Regenerate the SQL statement */
	sql = sql_stringify(statement);
	sql_destroy(statement);
	if (destobj->sqlready == FALSE)
		{
		mem_free(destobj->query);
		destobj->query = mem_strdup(sql);
		   /* Commented out 2005-02-21 to fix searching in bond */
/*		db_setready(destobj, TRUE); */
		}
	else
		{
		db_obj_clear(destobj);
		db_obj_sqlread(destobj, sql);
		}
	/* set birth values */
	/* Code is broken for birth values. or at least never appears to have
	 * worked */
	/* destobj->birth = db_birth_createfromsql(sql); */
	g_free(sql);
	for (i = 0; i < destobj->numfield; i++)
		{
		message("Where spring chicken been set?");
		while (ltablelist != NULL)
			{
			if (strcasecmp((gchar *) lfieldlist->data, destobj->field[i]->field) == 0 &&
			        strcasecmp((gchar *) ltablelist->data, destobj->field[i]->table) == 0)
				destobj->field[i]->birthvalue = mem_strdup(valuelist->data);

			if (ltablelist->next != NULL)
				{
				ltablelist = ltablelist->next;
				lfieldlist = lfieldlist->next;
				valuelist = valuelist->next;
				}
			else
				break;
			}
		ltablelist = g_list_first(ltablelist);
		lfieldlist = g_list_first(lfieldlist);
		valuelist = g_list_first(valuelist);
		}
	g_list_free(ltablelist);
	g_list_free(lfieldlist);
	g_list_free(valuelist);
	if (tofree)
		{
		for (walk = g_list_first(tofree); walk != NULL; walk = walk->next)
			db_freeobject(walk->data);
		g_list_free(tofree);
		}
	if (tofreestr)
		{
		for (walk = g_list_first(tofreestr); walk != NULL; walk = walk->next)
			mem_free(walk->data);
		g_list_free(tofreestr);
		}

	return retval;
	}

/**
 * db_loadobjectbyobject:
 * @obj: database object, source object
 * @destobj: database object, the returned object list
 * @desttable: destination table to load by
 * 
 * Destination object needs to be already created on your part.  What this will
 * do is run the nessary sql statements on the @destobj to make it a subset
 * of the @obj database object.
 *
 * Returns: non-zero on failure
 */
gint
db_loadobjectbyobject(Object * obj, Object * destobj, gchar * desttable)
	{
	gint retval = 0;
	gchar *query;
	DbConstraintDef *cref;
	DbTableDef *desttabledef;
	DbBirth *birth;

	/* You can't display a object from a non-saved object. */
	if (db_isnewrecord(obj) == TRUE)
		{
		if (db_isrecordchanged(obj) == FALSE)
			{
			debugmsg("Record has not changed so i will abort");
			return retval;
			}
		/* flush the buffer if its waiting to be writtened */
		db_toliet_flushobject(obj);
		debugmsg("%s is a new record, I am flushing it so i can use "
			"it properly.", obj->basetable);
		retval = 1;
		}

	/* loading yourself by yourself. hmmm */
	if (strcmp(desttable, obj->basetable) == 0)
		{
		birth = db_birth_create(obj->id, desttable);
		query = db_birth_createsql(birth);
		db_birth_save(destobj, birth);
		retval = db_obj_sqlread(destobj, query);
		mem_free(query);
		return retval;
		}
	desttabledef = db_findtable(desttable);
	g_assert(desttabledef);
	/* TODO: expand support to have more than one table source */
	cref = db_constraint_getonmatch(desttabledef->cref, obj->basetable, 
			desttable, 0);
	if (cref == NULL)
		cref = db_constraint_getonmatch(desttabledef->cref, desttable, 
				obj->basetable, 0);
	if (cref == NULL)
		{
		errormsg("Unable to find a relationship between %s and %s, "
			"make sure a cref exists", obj->basetable, desttable);
		return -3;
		}

	/* now the sensible stuff */
	db_obj_clear(destobj);
	birth = db_birth_create(NULL, desttable);
	if (db_birth_applycref(obj, birth, cref) != 0)
		{
		db_birth_free(birth);
		return -1;
		}
	query = db_birth_createsql(birth);
	g_assert(query);
	db_birth_save(destobj, birth);
	debugmsg("Added spring chicken stuff");
	retval = db_obj_sqlread(destobj, query);
	mem_free(query);
	/* Was there no results from this load? */
	/* Uncommented 2005-05-24 - dru */
	if (retval == 1) { debugmsg("Spring Chickens!"); 
	  db_birth_springchicken(destobj); } 
	debugmsg("Finishing running loadobjectbyobject, %d records",obj->num); 
	return retval;
	}

/* Why is there a shopping list in this code??? */

/**
 * db_refreshobject:
 * @obj: database object
 *
 * Re-run the query on an object. This will refresh all its data and dump 
 * anything sitting in the cache waiting to be flushed.
 *
 * Returns: non-zero on error
 */
gint
db_refreshobject(Object * obj)
	{
	return db_obj_refresh(obj);
	}

/* Prepare for random comment unattached to any specific code */

/* a bit more of the tricky stuff. load an object based on what
someone has selected in the first object. hmmm, yes the row does
play a part in this */

/**
 * db_debugobject:
 * @obj: database object to show information for
 * 
 * Show lots of intersting things about this object. Actually no, no it just 
 * prints a list of the values for every field. boring huh?.
 * db_obj_debug() differs from thsi function in that it prints out the cache 
 * values.
 */
void
db_debugobject(Object * obj)
	{
	gchar *tmpstr;
	gint x, y;

	fprintf(debug_stream, "====== OBJECT CONTENTS =======\n");
	for (y = 0; y < obj->num; y++)

		{
		for (x = 0; x < obj->numfield; x++)
			{
			tmpstr = db_dbgetvalue(obj->res, y, x);
			fprintf(debug_stream, "%s \t", tmpstr);
			}
		fprintf(debug_stream, "\n");
		}
	fprintf(debug_stream, "==============================\n");
	}

/**
 * db_setvalue:
 * @obj: database object
 * @field:
 * @table:
 * @value:
 *
 * Majical function that writes stuff to a database.  You can edit an existing 
 * recordset, create a new record, and modifed a new record fields with this 
 * function.
 *
 * Returns: non-zero on failure
 */
gint
db_setvalue(Object * obj, gchar * field, gchar * table, gchar * value)
	{
	DbSuggest *suggest;

	if (obj->dead)
		return -3;
	if (obj->row < 0 || obj->row >= obj->num)
		{
		debugmsg("Out of range for row, will not add write");
		return -1;
		}

	suggest = db_suggest_all(obj, field, table);

	/* Establish if field is a remote location. Null will be returned 
	 * if it can't be done */
	if (suggest->err != 0)
		{
		errormsg("Unable to find values for %s.%s. Aborting.", 
				table, field);
		return -1;
		}

	/* run man, just do it */
	db_obj_addwrite(suggest->obj, suggest->fieldname, suggest->tablename, value, TRUE);
	
	db_suggest_free(suggest);
	return 0;
	}

/**
 * db_setvalue_nomark:
 * @obj: database object
 * @field: field to set value into
 * @table: table to set this into, leave as NULL if main table
 * @value: string to set the field to
 *
 * Majical function that writes stuff to a database.  You can edit an existing 
 * recordset, create a new record, and modifed a new record fields with this 
 * function.  The difference between this function doesn't record the change 
 * as perment in the object, so if nothing else changes in teh object then 
 * the changes wont be saved, else they will be.
 *
 * Returns: non-zero on failure
 */
gint
db_setvalue_nomark(Object * obj, gchar * field, gchar * table, gchar * value)
	{
	gint retval = 0;
	DbSuggest *suggest;

	if (obj->row < 0 || obj->row >= obj->num)
		{
		debugmsg("Out of range for row, will not add write");
		return -1;
		}
	suggest = db_suggest_all(obj, field, table);

	/* Establish if field is a remote location. Null will be returned if 
	 * it cna't be done */
	if (suggest->err != 0)
		{
		errormsg("Unable to find values for %s.%s. Aborting.", table, field);
		return -1;
		}

	/* run man, just do it */
	debugmsg("adding in some default values of %s to %s.%s", value, suggest->tablename, suggest->fieldname);
	retval = db_obj_addwrite(suggest->obj, suggest->fieldname, suggest->tablename, value, FALSE);
	db_suggest_free(suggest);
	return retval;
	}

/**
 * db_getobjectbyfield:
 * @obj: database object
 * @field: field to load by
 * @value: value you want field to equal
 *
 * This is equivilant to a SELECT * FROM obj->name WHERE field=value.  
 * All records that match the required results will be returned. whiippi.
 *
 * Returns: non-zero on error
 */
gint
db_getobjectbyfield(Object * obj, gchar * field, gchar * value)
	{
	gint retval = 0;
	gchar *query;
	DbBirth *birth;

	mem_verify(obj);

	/* clear the contents of the object before creating a new instance of 
	 * it */
	db_obj_clear(obj);
	if (obj->basetable == NULL)
		{
		errormsg("I have no table associated with this object, I "
			"can not proceeed as i do not know what to filter on");
		return -1;
		}
	birth = db_birth_create(NULL, obj->basetable);
	db_birth_addvalue(birth, field, value);
	query = db_birth_createsql(birth);
	db_birth_save(obj, birth);
	g_assert(obj->birth);
	retval = db_obj_sqlread(obj, query);
	mem_free(query);
	return retval;
	}

/**
 * db_getobjectid:
 * @obj: database object
 *
 * Get uniqueid for a object, this just returns the obj->id value. 
 *
 * Returns: %NULL on error else the unique id
 */
DbUniqueId *
db_getobjectid(Object * obj)
	{
	mem_verify(obj);
	if (obj->id == NULL)
		{
		errormsg("Returning NULL objectid. This is bad");
		g_assert(obj->id);
		}
	return obj->id;
	}

/**
 * db_setrowbyfield:
 * @obj: database object
 * @fieldname: fieldname to set by
 * @value: value to equate by
 *
 * Assumes object list has already been loaded. Go though records and find one that
 * matchs criteria specified and sets row to that field. 
 *
 * Returns: non-zero on error
 */
gint
db_setrowbyfield(Object * obj, gchar * fieldname, gchar * tablename, gchar * value)
	{
	gchar *tmpstr;
	gint row = -1;
	gint i, pos = -1;
	gint oldrow;
	
	mem_verify(obj);
	g_assert(value);
	if (obj->num == 0)
		{
		errormsg("Empty list, can't set row");
		return -2;
		}
	oldrow = obj->row;
	pos = db_field_getpos(obj, fieldname, tablename);
	if (pos < 0)
		{
		errormsg("We have troubles, can't find %s", fieldname);
		return -3;
		}
	for (i = 0; i < obj->num; i++)
		{
		/* move along and get the value */
		db_moveto(obj, i);
		db_obj_doread(obj, fieldname, tablename, &tmpstr);

		/* do the compare */
		if (strcmp(value, tmpstr) == 0)
			{
			row = i;
			break;
			}
		}
	if (row == -1)
		{
		db_moveto(obj, oldrow);
		debugmsg("Unable to find a record with the value selected");
		return -3;
		}

	/* set to the new postion like */
	db_moveto(obj, row);
	return 0;
	}

/**
 * db_deleteobject:
 * @obj: database object
 *
 * Deletes the record in obj, and all records which follow this one.  It 
 * only deletes the row your in, not the entire record set. Needs to be 
 * tested still, as it has the potential to cause huge damage, especially 
 * with things like circular relationships.
 *
 * Returns: non-zero on error
 */
gint
db_deleteobject(Object * obj)
	{

	mem_verify(obj);

	/* Run recusive delete, and move that code below into something 
	 * else like db_obj_dodelete() */

	db_del_objectrecinit();
	db_del_objectrec(obj, db_id_get(obj));
	db_del_objectreccleanup();
	debugmsg("This is going to delete recusively subrelationships.\n"
		"TEST WELL BEFORE USE!");
	return 0;
	}

/**
 * db_clearobject:
 * @obj: database object
 *
 * Clear the contents of an object making everything blank.  This will free up any
 * record sets associated with @obj and setup the object as a new record.  The
 * table it points to will be the same as its existing table.
 *
 * Returns: non-zero on error
 */
gint
db_clearobject(Object * obj)
	{
	if (obj == NULL)
		return -3;
	return db_obj_clear(obj);
	}

/* CODE GRAVE YARD .... from db_getvalue*/

/* check to see if num is out of range. If so return "" string */

/*	if (obj->append == TRUE)
		{
		if (obj->row == obj->num-1)
			{
			warningmsg("Trying to extract a value from a not added object (in process of append");
			*retval = emptystring;
			return 0;
			}
		} */

/* data has changed so reload query. I disabled this cause i dont know why i have it */

/*   if (obj->changed == TRUE)// && obj->append != TRUE)
	db_refreshobject(obj);  */

/*   if ((fieldpos = db_getfieldpos(obj, field)) == -1)
      {
      errormsg("field %s not found in %s", field, obj->table->name);
      g_assert(NULL);	
      return -1;
      } */

/*	printf(":%d:%d  %s\n",obj->row,fieldpos,*retval); */

/* m.essage("Got value %s for row %d, %d items in total, This is all strange and lovely and i think we ar egoing to be hit by a giant rock from outta space",*retval,obj->row,obj->num); */

/* new record, get what you just wrote */

/* if (obj->pg_oid > 0 && obj->res == NULL) { warningmsg("This is untested code. I dont know why i am here. HELP");
   db_refreshwrittenobject(obj); } 
10 PRINT "HELLO WORLD"
20 GOTO 10
	*/

/* check the local pointer */

/*   if (db_isnewrecord(objlocal) == TRUE && db_ischanged(objlocal) == FALSE)
      {
      errormsg("%s is a new record, I can't load values from it into the other object.\n"
					"You need to add some text",objlocal->name);
      return -1;
      } */

/*   if (db_isvalidforread(objlocal) == FALSE)
      {
      errormsg("Invalid oid for current record");
      return -4;
      } */

/*   desttable = objdest->table;
   g_assert(desttable);
   if (desttable->cref == NULL)
      {
      errormsg("Unable to find any relationships for this destionation table, can not proceed");
      return -5;
      }
 
   walk = g_list_first(desttable->cref);
   while (walk != NULL)
      {
      c = walk->data;
      g_assert(c);
      localfield = NULL;
      destfield = NULL;
      if (strcasecmp(c->table[0], objlocal->table->name) == 0 && c->casual == 0)
         {
         localfield = c->column[0];
         destfield = c->column[1];
         }
      else if (strcasecmp(c->table[1], objlocal->table->name) == 0 && c->casual == 0)
         {
         localfield = c->column[1];
         destfield = c->column[0];
         }
      if (localfield != NULL && destfield != NULL)
         {
         db_getfieldstr(objlocal, localfield, &value);
         db_addwrite(objdest, destfield, value);
         objdest->changed = FALSE;
         }
      walk = walk->next;
      }
   objdest->changed = FALSE;
 
   return 0;
   }
*/

/* Check to work if refreshnwrittenrecord worked. */

/* if (obj->res == NULL) { errormsg("Object result is null, meaning you trying to read stuff not yet written.\n"
   "This occured for %s.%s", obj->table->name, field); g_assert(NULL); } */

/* this code died somewhere else but we bury him here just the same */

/* if (c->obj != NULL) if (obj != c->obj || (obj == c->obj && c->pg_oid != obj->pg_oid)) db_flushobjects(); */

/* gchar *str; gint fieldpos; extern DbWriteCache *globaldbwritecache;
   errormsg("Code is obsolete. Do not use"); g_assert(NULL);
   g_assert(obj); if (globaldbwritecache != NULL) if (globaldbwritecache->obj != NULL) if (obj ==
   globaldbwritecache->obj) db_flushobjects();
   g_assert(obj->res); g_assert(obj->table); if (obj->res == NULL || obj->row < 0) { errormsg("obj->res is null or
   obj->row is < 0, i can't do this. arr"); return -3; }
   if (obj->append == TRUE) { if (obj->row == obj->num - 1) { (*retval) = 0; return 0; } }
   if ((fieldpos = db_getfieldpos(obj, field)) == -1) { errormsg("field not found"); return -1; } str = (gchar *)
   db_dbgetvalue(obj->res, obj->row, fieldpos); if (str == NULL) { errormsg("returned null"); return -2; } *retval =
   atoi(str); return 0; */

/**
  * db_getfieldint:
  * @obj: Database object
  * @field: database field name
  * @retval: int to return
  * Retreives an int from the database based on a object and field.
  * db_getfieldstr().
  * Returns: <b>error code</B> on how successful it was. 
  */

/*
gint
db_getfieldint(Object * obj, gchar * field, gint * retval)
   {
	
	
   gchar *str = NULL;
	*retval = 0;
   db_getfieldstr(obj, field, &str);
	if(str==NULL)
		{
		errormsg("Unable to get int value for field %s in %s",field,obj->name);
		if (db_isnewrecord(obj)==TRUE)
			g_assert(NULL);
		g_assert(NULL);
		return -1;
		}
   *retval = atoi(str);
   return 0;
   } */

/* Note, dont free the returned string from this, as its been used
   by something else. Soo NOOO YOUR NOT ALLOWED TO!. Hands off or else
   I'll poke you with nasty bits.
*/

/**
 * db_lookthroughobject:
 * @objlocal:
 * @othertable:
 * HMMM I"M DUMPING THIS CODE CAUSE IT AN"T USED MUCH 
 * Returns: %NULL on unable to find anything else the field that links it.
								  				  		  	  */ /* 
								     gchar * db_lookthroughobject(Object * objlocal, gchar * othertable) { gchar *destfield,
								     *localfield; GList *walk; DbTableDef *db; DbConstraintDef *c;
 
								     g_assert(objlocal); g_assert(othertable);
 
								     if (db == NULL) { debugmsg("No object found"); return NULL; } db =
								     db_findtable(othertable); walk = g_list_first(db->cref); while (walk != NULL) { c =
								     walk->data; g_assert(c); if (strcasecmp(c->table[0], objlocal->table->name) == 0) */

/* othertable has a ref to local object */

/*         {
         localfield = c->column[0];
         destfield = c->column[1];
         return destfield;
         }
      else if (strcasecmp(c->table[1], objlocal->table->name) == 0)
         {
         localfield = c->column[1];
         destfield = c->column[0];
         return destfield;
         }
      walk = walk->next;
      }
   return NULL;
																				 										 					 		     } */ /* 
																				  * db_getfieldstr:
																				  * obj: Data
																				  */

/*
gint
db_getfieldstr(Object * obj, gchar * field, gchar ** retval)
   {
   gint fieldpos;
 
   *retval = NULL;
   g_assert(obj);
*/

/* get field position in the data set */

/* if ((fieldpos = db_getfieldpos(obj, field)) == -1) { errormsg("field %s not found in %s", field, obj->name);
   return -1; } */

/* is the value sitting in the cache ready to be used ?? */

/* if (db_isincache(obj) == TRUE) if (globaldbwritecache->value[fieldpos] != NULL) { *retval =
   globaldbwritecache->value[fieldpos]; return 0; } */

/* do i need to flush the cache? */

/*   if (db_isflushneeded(obj) == TRUE)
      {
      debugmsg("Flushing cache cause its repeating stuff for query %s", obj->query);
      debugmsg("Gathering field %s, %d", field, obj->row);
      db_flushobjects();
      } */

/* am i allowed to read from this object ??. ie is not a blank record */

/*   if (db_isvalidforread(obj) == FALSE)
      {
      debugmsg("empty record, can not retrieve a value %s at postion %d of %d", field, obj->row, obj->num);
      return 1;
      }
 */

/* assumes row and field is set right, and res is correct */

/* The string is also stripped to not contain end trailing or beginning spaces. This shoulnd't create a memory leak,
   but you never know. I'll check for this later on */

/*	db_field_read(obj, field, retval);
   (*retval) = g_strstrip((gchar *)(*retval)); */

/* Something went straigly wrong */

/*   if (*retval == NULL)
      {
      errormsg("returned null");
      return -2;
      }
   return 0;
   }*/

/* Let there souls rest in peace ... */

/**
 * db_getobjectbyreference:
 * @obj: Database object
 * @tablename: Name of table to load object by
 *
 * This function will create a new database object based on an existing object
 * and a table name.  Constraint information will be used to work out how the
 * two tables relate to each other.
 */
Object *
db_getobjectbyreference(Object * obj, gchar * tablename)
	{
	g_assert(obj);
	g_assert(tablename);
	return db_mapme_findobjectbytable(obj, tablename);
	}

/**
 * db_numrows:
 * @obj: Database object
 *
 * Returns the number of rows there are in a object.
 *
 * Returns: interger number of rows in a object dataset
 */
gint
db_numrows(Object * obj)
	{
	g_assert(obj);
	return db_numrecord(obj);
	}

/**
 * db_addobjecttoobject:
 * @objdest: the new database object which will have information added to it
 * @objlocal: the existing base database object.
 * 
 * Please you db_loadobjectbyobjects() instead of now. 
 *
 * This is not a major api call, db_loadbyobject() will automatically call this
 * function.
 *
 * Elements of @objlocal will be used to create elements in @objdest.  This will
 * find all the links between the two objects and populate @objdest with 
 * values corresponding from those links.
 *
 * Returns: non-zero on error
 */

gint
db_addobjecttoobject(Object * destobj, Object * obj)
	{
	DbConstraintDef *cref;
	DbTableDef *desttabledef;
	DbBirth *birth;

	/* loading yourself by yourself. hmmm */
	if (strcmp(destobj->basetable, obj->basetable) == 0)
		{
		errormsg("Your trying to add yourself from yourself to yourself. confusing huh? well what about me");
		return -1;
		}
	desttabledef = db_findtable(destobj->basetable);
	g_assert(desttabledef);
	/* TODO: expand support to have more than one table source */
	cref = db_constraint_getonmatch(desttabledef->cref, obj->basetable, destobj->basetable, 0);
	if (cref == NULL)
		{
		errormsg("Unable to find a relationship between %s and %s, make sure a cref exists", obj->basetable,
		         destobj->basetable);
		return -3;
		}

	/* now the sensible stuff */
	birth = db_birth_create(NULL, destobj->basetable);
	db_birth_applycref(obj, birth, cref);
	db_birth_save(destobj, birth);
	debugmsg("Spring Chickens! - adding cref to birth");
	db_birth_springchicken(destobj);
	return 0;
	}

/**
 * db_add:
 * @obj: Database object
 *
 * Adds another record onto the end of a database object.  For example if you
 * performaned an sql query to create a database object you can then use
 * this function to append a new record onto the end of that record set.
 * Default values will be applied and if it wsa an object loaded by
 * db_loadobjectbyobject() then other nessesary values will also
 * be added.
 *
 * Returns: non-zero on failure
 */
gint
db_add(Object * obj)
	{
	gint num, retval = 0, i;

	g_assert(obj);
	obj->dead = FALSE;
	if (db_isabletoadd(obj) == FALSE || (db_isabletogetid(obj) == FALSE && obj->num > 0))
		{
		debugmsg("You can't add a record without finishing this record");
		return -1;
		}
	num = obj->num;
	/* add a new item to the cache */
	db_setforinsert(obj);
	retval -= db_moveto(obj, obj->num - 1);
	retval -= db_obj_applydefaults(obj);
	/* Apply any default values that need to be for variables. */

	for (i = 0; i < obj->numfield; i++)
		if (obj->field[i]->birthvalue)
			db_setvalue_nomark(obj, obj->field[i]->field, obj->field[i]->table, obj->field[i]->birthvalue);

	/* Adds birth values. Warning, deosn't support tables proberly */
	if (obj->birth != NULL)
		{
		debugmsg("Applying default springchicken values.");
		retval -= db_birth_springchicken(obj);
		}
	else	debugmsg("No spring chickens");
	if (obj->num != num + 1)
		{
		errormsg("unable to add new record, orginally there was %d now there is %d.\n"
		         "There should be one more", num, obj->num);
		retval = -5;
		}
	/* db_field_debug(obj); */
	return retval;
	}

/**
* db_insert:
* @obj: Database object
*
* Not yet implimented
*
* Returns: non-zero on failure
*/
gint
db_insert(Object * obj)
	{
	g_assert(obj);

	errormsg("NOT WRITTEN. Please code this. : TODO");
	g_assert(NULL);

	if (db_isabletoadd(obj) == FALSE)
		{
		warningmsg("You can't add a record without finishing this record");
		return -1;
		}
	/* add a new item to the cache */
	/* TODO c = db_toliet_addtocache(obj); c->origrow = obj->row + 1; obj->row = obj->row + 1; obj->num++;
	   db_moveto(obj, obj->row + 1); */ 
	return 0;
	}

/**
 * db_createdeadobject:
 * @tablename: Name of a table where the record will be inserted
 *
 * Will create a non-usuable dead object.  This object can be used
 * to indicate what fields etc exist on the datasource @tablename.
 *
 * Returns: A newly created database object, or %NULL on error.
 */
Object *
db_createdeadobject(gchar * tablename)
	{
	Object *obj;

	obj = db_obj_create(tablename);
	obj->numfield = db_numfields(obj);
	db_obj_handle_empty_recordset(obj);
	obj->dead = TRUE;

	return obj;
	}

void
db_setready(Object * obj, gboolean state)
	{
	g_assert(obj);
	if (state == TRUE && obj->sqlready == FALSE)
		{
		obj->sqlready = state;
		if (obj->query)
			db_obj_sqlread(obj, obj->query);
		}
	obj->sqlready = state;
	}
/**
 * db_wakemastersoftables:
 * @obj: Database object.
 * 
 * This works out what the master of tables are and enables them.
 * This calls db_masteroftable_populate() directly. 
 * 
 * Returns: Negitive on error.
 */
gint
db_wakemastersoftables(Object *obj)
	{
	return db_masteroftable_populate(obj);
	}


/**
 * db_fieldschanged:
 * @obj: Database object which a db_masteroftable_actionchange has taken effect on.
 *
 * This will return a list of all tables that are effected by a change.
 * Dont free the GList* that is returned.
 * 
 * Returns: GList* of DbField*
 */
GList *
db_fieldschanged(DbObject *obj)
	{
	g_assert(obj);
	return obj->fieldschanged;
	}

/**
 * db_save:
 * @obj: Database object to save.
 * @errormsg: if there is any error on update or insert this is it.
 * 
 * Saves @obj and does any insert's or update's that are needed for it.
 * Check for any errors with db_getlasterror().
 * 
 * Returns: Non-zero on error.
 */
gint 
db_save(DbObject * obj)
   {
   g_assert(obj);
   
   db_toliet_flushobject(obj);
   
   return 0;
   }

/**
 * db_getlasterror:
 * 
 * Returns: The last sql error message that occurred or %NULL if no errors 
 * have occurred. Do not free.
 */
gchar* 
db_getlasterror()
   {
   return globaldbconn->errormsg;
   }

/**
 * db_clearlasterror:
 * 
 * Frees the last error message created by an sql statement. This doesnt
 * need to be called as it will be automatically freed by itself.
 */
void 
db_clearlasterror()
   {
   if (globaldbconn->errormsg)
	mem_free(globaldbconn->errormsg);
   globaldbconn->errormsg = NULL;
   }
