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

#include "dbuniqueid.h"

#include "db.h"
#include "dbwrapper.h"
#include "bc.h"
#include "dbobject.h"
#include "dbgather.h"
#include "dbfield.h"
#include "dbtoliet.h"
#include "dbsqlparse.h"

static gchar *db_id_verifysql_pg(gchar * sql);
static gchar *db_id_verifysql_other(gchar * sql, DbUniqueId * uid);

gint globaltemporaryids;

/**
 * db_id_createblank:
 * @tablename: The base table to work out what are the primary keys
 *
 * Will generate a uniqueid structure with no data in it, ready to be assigned
 * values. The id will still be unique but it would change as soon as you 
 * wrote it back to the database, this attempts to be a tempory use id.
 *
 * Returns: freshly allocated uniqueid.
 */
static DbUniqueId *
db_id_createblank()
	{
	DbUniqueId *id;

	id = (DbUniqueId *) mem_alloc(sizeof(DbUniqueId));
	globaltemporaryids--;
	id->pg_oid = globaltemporaryids;
	id->primarykeys = NULL;
	id->num = 0;
	return id;
	}

/**
 * db_id_free:
 * @id: Unique id
 *
 * Frees up the invidual @id, which is allcoed by a db_id_createblank.
 * This needs support for primary key cleanup at some point, you go baz go 
 * write that code
 *
 * Returns: non-zero no failure
 */
gint
db_id_free(DbUniqueId * id)
	{
	mem_free(id);
	/* TODO: Have to add in support for primary key freeing at some point */
	return 0;
	}

/**
 * db_id_remeber:
 * @obj: Database Object
 *
 * Because all the id's are not read in straight away, this function will 
 * set the unique ids in the array to what they are suppose to based on 
 * the object->row.
 *
 * Returns: NULL on zero or newly created id.
 */
DbUniqueId *
db_id_remeber(Object * obj)
	{
	return db_id_remeberbyrow(obj, obj->row);
	}

/**
 * db_id_remeberbyrow:
 * @obj: Database Object
 * @row: Row to remeber by.
 * @idindexpos: A returned interger of what position in the array this record 
 * is in. 
 *
 * Similar to db_id_remeber() but allows you to specify the row you want to 
 * remeber the unique id for.  This will give a unique temporary id if one 
 * is not availiable for use.
 *
 * Returns: NULL on failure else the uniqueid for the newly created row.
 */
DbUniqueId *
db_id_remeberbyrow(Object * obj, gint row)
	{
	DbUniqueId *id;

	g_assert(obj);
	/* See if you already have this position is set already */
	if ((id = db_id_getbyrow(obj, row)) != NULL)
		return id;

	if (db_isabletogetid(obj) == FALSE)
		{
		/* create that id */
		id = db_id_createblank();
		/* m.essage("Adding temporary unique id of %d to object %s at row
		   %d",id->pg_oid,obj->basetable,row); */
		/* db_id_adduniqueid(obj, id, row); */
		obj->unknownid = FALSE;
		/* Object is new and yet to be written */
		}
	else
		{
		/* Just so we dont run out of intergers */
		globaltemporaryids++;
		   
		/* m.essage("getting id for %s at row %d",obj->basetable,row); */
		/* Load value if not. */
		id = db_id_createblank();
		if (db_id_extractid(obj, id, row) == 0)
			{
			/* The extractid function will write to cache the value of the
			   id */
			/* m.essage("Extracting unique id of %d to object %s at row
			   %d",id->pg_oid,obj->basetable,row); */
			;
			}
		else
			{
			errormsg("Unable to extract uniqueid for record");
			db_id_free(id);
			return NULL;
			}
		}
	return id;
	}

/**
 * db_id_get:
 * @obj: Database Object
 *
 * Based on what row your currently on, it will return the unique identifier 
 * for that record.  
 *
 * Returns: DbUniqueId value, or %NULL on failure
 */
DbUniqueId *
db_id_get(Object * obj)
	{
	return db_id_getbyrow(obj, obj->row);
	}

/**
 * db_id_getbyrow:
 * @obj: Database Object
 * @row: Row to get unique id for.
 *
 * Based on what row your currently on, it will return the unique identifier 
 * for that record.
 *
 * Returns: DbUniqueId value, or %NULL on failure
 */
DbUniqueId *
db_id_getbyrow(Object * obj, gint row)
	{
	if (row < obj->numcache && row >= 0)
		if (obj->cache[row] != NULL)
			return obj->cache[row]->id;
	return NULL;
	}

/**
 * db_id_moveto:
 * @obj: Database Object
 * @id: Unique Id
 *
 * Move to a row in the recordset based on the @id.
 *
 * Returns: negitive number on failure, else positive number reflecting position
 */
gint
db_id_moveto(Object * obj, DbUniqueId * id)
	{
	gint i;

	g_assert(obj);

	for (i = 0; i < obj->numcache; i++)
		if (obj->cache[i] != NULL)
			if (db_id_compare(id, obj->cache[i]->id) == 0)
				return obj->cache[i]->origrow;
	return -1;
	}

/**
 * db_id_compare:
 * @id1: Unique id
 * @id2: Unique id
 *
 * Compare two uniqueids to see if they are the same.
 *
 * Returns: non-zero if there isn't a match.
 */
gint
db_id_compare(DbUniqueId * id1, DbUniqueId * id2)
	{
	gint i, fail;

	if (id1->pg_oid != 0 && id2->pg_oid != 0)
		{
		if (id1->pg_oid == id2->pg_oid)
			return 0;
		return 1;
		}

	fail = 1;
	/* This relies on the primary keys been kept in the same order */
	for (i = 0; i < id1->num; i++)
		if (strcmp(id1->primarykeys[i], id2->primarykeys[i]) != 0)
			{
			fail = 1;
			break;
			}
	if (fail == 0)
		return 0;

	return 1;
	}

/* SELECT * FROM table WHERE primarykeynames = oid */

/**
 * db_id_createsql:
 * @obj: Database Object
 * @id: Unique id
 *
 * Allocates a string that you can put at the end of an SQL statement to get 
 * the record specified by @id.  You will need to put a WHERE in your SQL 
 * statement prior to appending this string onto it.  
 *
 * Unlike most functions, the returned string here needs to be freed.  Use 
 * mem_free(retstr), on successful running of this function.
 *
 * Returns: newly created sql string, or %NULL on failure.
 */

gchar *
db_id_createsql(Object * obj, DbUniqueId * id)
	{
	gchar *retstr, *tmpstr;
	gchar **strlist;
	gint i;

	g_assert(id);

	if (id->pg_oid > 0)
		{
		retstr = mem_strdup_printf("%s.oid=%d", obj->basetable, id->pg_oid);
		return retstr;
		}
	else if (id->num > 0)
		{
		strlist = (gchar **) mem_alloc(sizeof(gchar *) * (id->num + 1));
		for (i = 0; i < id->num; i++)
			strlist[i] =
			    mem_strdup_printf("%s.%s='%s'", id->field[i]->table,
			                      id->field[i]->field, id->primarykeys[i]);

		tmpstr = g_strjoinv(" AND ", strlist);
		retstr = mem_strdup(tmpstr);
		g_free(tmpstr);

		for (i = 0; i < id->num; i++)
			mem_free(strlist[i]);
		mem_free(strlist);
		return retstr;
		}
	return NULL;
	}

/**
 * db_id_verifysql:
 * @sql: SQL Statement to be verified  
 *
 * Checks to see if a sql select statement has its unique identifies listed 
 * last, if not it adds them and returns a new string based on @sql.  
 * This assumes you already have the object name set as this is a reference
 * point to extract the unique id.
 *
 * Returns: Newly created SQL string, NULL on error
 */
gchar *
db_id_verifysql(gchar * sql)
	{
	DbUniqueId *uid;
	gchar *retstr, *tablename;

	tablename = db_sqlparse_getfirsttable(sql);
	uid = db_id_createblank(tablename);
	mem_free(tablename);

	/* Checks to see if it is a pg Unique ID */
	if (db_dbgetserver() == DB_PGSQL)
		{
		retstr = db_id_verifysql_pg(sql);
		db_id_free(uid);
		return retstr;
		}
	/* Checks to see if it has some primary key fields */
	else
		{
		retstr = db_id_verifysql_other(sql, uid);
		db_id_free(uid);
		return retstr;
		}
	warningmsg("Unable to determine the type of uniqueid this is");
	/* couldn't find a unique identifier so return NULL */
	return NULL;
	}

/**
 * db_id_verifysql_pg:
 *
 * verifies sql statements for nice postgresql databases 
 */
static gchar *
db_id_verifysql_pg(gchar * sql)
	{
	int i, isokay = 0, isselect = 0;
	gchar *retstr;

	g_assert(sql);

	retstr = (gchar *) mem_alloc(strlen(sql) + 6);
	/* Is this a select statement anyway? */
	for (i = 0; i < strlen(sql) - 4; i++)
		{
		if (sql[i] == 'f' || sql[i] == 'F')
			if (sql[i + 1] == 'r' || sql[i + 1] == 'R')
				if (sql[i + 2] == 'o' || sql[i + 2] == 'O')
					if (sql[i + 3] == 'm' || sql[i + 3] == 'M')
						{
						isselect = 1;
						break;
						}
		retstr[i] = sql[i];
		}
	/* checks for ,oid adds it if not there */
	if (isselect == 1)
		{
		if (sql[i - 4] == 'o' || sql[i - 4] == 'O')
			if (sql[i - 3] == 'i' || sql[i - 3] == 'I')
				if (sql[i - 2] == 'd' || sql[i - 2] == 'D')
					isokay = 1;
		if (isokay == 1)
			{
			mem_free(retstr);
			/* debugmsg("This sql string should already work with
			   postgresql"); */ 
			return mem_strdup(sql);
			}
		else
			{
			retstr[i - 1] = ',';
			retstr[i] = 'o';
			retstr[i + 1] = 'i';
			retstr[i + 2] = 'd';
			retstr[i + 3] = ' ';
			for (; i < strlen(sql) + 1; i++)
				retstr[i + 4] = sql[i];
			retstr[strlen(sql) + 4] = 0;
			}
		}
	else
		{
		errormsg("Not a select statement");
		mem_free(retstr);
		return NULL;
		}
	return retstr;
	}

/**
 * db_id_verifysql_other:
 *
 * Verifies other non postgresql databases that dont have pgoids, damn them 
 * As you can see this is a scary function written by baz.
 *
 * Returns: a static
 */
static gchar *
db_id_verifysql_other(gchar * sql, DbUniqueId * uid)
	{
	int i, j, k, isselect = 0, found = 0, placeholder;
	int foundat[uid->num];
	gchar *retstr = NULL, *firstbit = NULL, *endbit = NULL;
	GList *fieldlist = NULL, *listrunner = NULL;

	g_assert(sql);
	g_assert(uid);

	/* Ok I know I'm mallocing too much but I cant help myself */
	firstbit = (gchar *) mem_alloc(strlen(sql) + 4);
	endbit = (gchar *) mem_alloc(strlen(sql) + 4);

	/* makes sure its a select statement */
	for (i = 0; i < strlen(sql); i++)
		{
		if (sql[i] == 'f' || sql[i] == 'F')
			if (sql[i + 1] == 'r' || sql[i + 1] == 'R')
				if (sql[i + 2] == 'o' || sql[i + 2] == 'O')
					if (sql[i + 3] == 'm' || sql[i + 3] == 'M')
						{
						isselect = 1;
						break;
						}
		firstbit[i] = sql[i];
		}

	if (isselect == 0)
		{
		errormsg("This is not an select statement, so I'm returning NULL");
		return NULL;
		}
	placeholder = i - 1;

	fieldlist = db_sqlparse_getselectfields(sql);
	listrunner = g_list_first(fieldlist);
	if (fieldlist == NULL)
		warningmsg("No fields found in sql query.");

	while (listrunner != NULL)
		{
		for (j = 0; j < uid->num; j++)
			{
			if (strcmp((gchar *) listrunner->data,
			           (gchar *) uid->field[j]->field) == 0)
				{
				foundat[j] = 1;
				found++;
				}
			else
				{
				firstbit = (gchar *) mem_realloc(firstbit,
				                                 strlen(firstbit) +
				                                 strlen(uid->field[j]->
				                                        field) + 2);
				}
			}
		listrunner = g_list_next(listrunner);
		}
	/* free up the fieldlist */
	db_sqlparse_freeselectfields(fieldlist);

	if (found != uid->num)
		{
		for (j = 0; j < uid->num; j++)
			{
			if (foundat[j] != 1)
				{
				firstbit[placeholder] = ',';
				placeholder++;
				for (k = 0; k < strlen(uid->field[j]->field); k++)
					{
					firstbit[placeholder] = uid->field[j]->field[k];
					placeholder++;
					}

				}
			}
		firstbit[placeholder] = 0;
		for (i = 0, j = 0; i < strlen(sql); i++, j++)
			endbit[j] = sql[i];
		endbit[j] = 0;
		}
	else
		{
		debugmsg("The sql statement is fine just as it is");
		mem_free(firstbit);
		mem_free(endbit);
		mem_free(retstr);
		return sql;
		/* and now excute some fancy arse stuff that no one will see cause its
		   on the end of this line */
		}
	retstr = mem_strdup_printf("%s %s", firstbit, endbit);
	mem_free(endbit);
	mem_free(firstbit);
	return retstr;
	}

/**
 * db_id_extractid:
 * @obj: Database Object
 * @id: Id record to record results into
 * @row: Row at which to extract key.
 *
 * Read from your recordset to find the uniqueid for @row.  The value is written
 * into @id.  It adds to cache the value 
 *
 * Returns: non-zero on error
 */
gint
db_id_extractid(Object * obj, DbUniqueId * id, gint row)
	{
	gchar *val;
	gint i;
	glong pg_oid;

	mem_verify(id);
	if (id == NULL)
		{
		errormsg("DbUniqueId is NULL, can not proceed for %s", obj->query);
		return -3;
		}
	if (obj->res == NULL)
		{
		debugmsg("Obj->res is NULL so i'm just making something up here.");
		return -1;
		}
	if (row >= obj->num)
		{
		errormsg("Your trying to read more than you are allowed...");
		return -4;
		}
	if (obj->cache[row] == NULL)
		{
		errormsg("cache isn't present on this row, I dont know what to do.");
		return -5;
		}
	if (obj->numfield != obj->cache[row]->num)
		errormsg("cache and field numbers dont match.");
	for (i = 0; i < obj->numfield; i++)
		if (obj->field[i]->isuniqueid == TRUE)
			{
			if (obj->field[i]->fielddef->datatype == DBPGOID)
				{
				if (obj->cache[row]->value[i] == NULL)
					{
					/* message("field %d %s.%s",i,
					   obj->field[i]->table,obj->field[i]->field); */
					obj->cache[row]->value[i] = mem_alloc(sizeof(long));
					val = db_dbgetvalue(obj->res, row, i);
					if (val)
						{
						pg_oid = atoi(val);
						memcpy((void *)(obj->cache[row]->value[i]),
						       (void *)&pg_oid, sizeof(long));
						}
					else
						warningmsg
						("Unable to extract pgoid %s.%s at position %d, row %d",
						 obj->field[i]->table, obj->field[i]->field, i,
						 row);
					/* m.essage("loading id. %d",pg_oid); */
					}
				else
					pg_oid = *((long *)obj->cache[row]->value[i]);
				if (obj->field[i]->basetable == TRUE)
					{
					/* m.essage("basetable set on %s",
					   obj->field[i]->field); */
					id->pg_oid = pg_oid;
					}
				}
			else
				errormsg("Code Me!. I can only handle int.");
			}

	if (id->pg_oid < 0)
		{
		db_cache_debug(obj);
		errormsg("Unable to extract the unique id for this record. "
		         "Aborting here. pg_oid:%d row:%d.\nAbortings now.",
		         id->pg_oid, row);
		g_assert(NULL);
		return -2;
		}
	/* m.essage ("Extracted id %d from %s at %d pos
	   %d",id->pg_oid,obj->name,obj->row, pos); */ 
	return 0;
	}

/**
 * db_id_isnewrecord:
 * @id: Id record to check
 *
 * Check to see if @id is a new record that has yet to be inserted into the 
 * database, meaning we manage the uniqueid until the database can take 
 * it over then.  The id will typically be  invalid like negitive but 
 * still unique.
 *
 * Returns: TRUE if it is a new record, otherwise FALSE
 */
gboolean
db_id_isnewrecord(DbUniqueId * id)
	{
	g_assert(id);
	if (id->pg_oid < 0)
		return TRUE;
	return FALSE;
	}

/**
 * db_id_isvalid:
 * @id: Id record to check
 * Check to see if @id a valid record to index on.
 *
 * Returns: TRUE if it is record ok, otherwise FALSE if it isn't good.
 */
gboolean
db_id_isvalid(DbUniqueId * id)
	{
	if (id == NULL)
		return FALSE;
	if (db_id_isnewrecord(id) == TRUE)
		return FALSE;
	return TRUE;
	}

/**
 * db_id_syncid:
 * @obj: Database object
 *
 * Set the object id which it is pointing to, to whatever it should be 
 * based on the row you have selected.  Sounds complex i know.
 *
 * Returns: non-zero on failure
 */
gint
db_id_syncid(Object * obj)
	{
	if (obj->currentcache != NULL)
		{
		obj->id = obj->currentcache->id;
		return 0;
		}
	return -1;
	}

/**
 * db_uniqueid:
 * @result: Result from a insert
 * @fieldpos: Field position
 * 
 * Kinda a wrapper for PQoidValue(@result);
 * Assuming you run an insert query with db_dbexec(), this will return the 
 * unique id assocated with that record.  Note this works on just getting 
 * the oid value from postgresql, on other databases a bit of a work 
 * around will be needed.
 *
 * Returns: the pg OID value. %NULL is returned if PQoidValue fails.
 */
DbUniqueId *
db_uniqueid(DbConnection * conn, DbRecordSet * result)
	{
	gint oid;
	DbUniqueId *id;

	oid = db_dbuniqueid(conn, result);
	if (oid == 0)
		return NULL;

	id = db_id_createblank();
	id->pg_oid = oid;
	return id;
	}

/**
 * db_id_generateidfromfield:
 * @field:
 * @oid:
 * 
 * Generate a uniqueid from a GList* of void* pointers, used in flushes.
 * 
 * Returns: Newly generated id, needs to be freed after use.
 */
DbUniqueId *
db_id_generateidfromfield(DbField * field, GList * oidlist)
	{
	long pg_oid;
	DbUniqueId *id;

	g_assert(field);
	id = db_id_createblank();
	globaltemporaryids--;
	/* handle one uniqueid */
	if (field->fielddef->datatype == DBPGOID && oidlist->next == NULL)
		{
		memcpy(&pg_oid, oidlist->data, sizeof(long));
		id->pg_oid = pg_oid;
		}
	else
		errormsg("Code Me!. This should have support for multiple "
		         "primary keys.");
	return id;
	}

/* must free afterwards */

/**
 * db_id_getuniqueid_asstring:
 * @obj:
 * @tablename:
 * @fieldname:
 * 
 * Given a tablename and previously created object it finds the primary key
 * which it returns as a string and also gives a @fieldname as the name of the
 * primary key field.  It doens't support multiple primary keys.
 * 
 * Returns: Primary key, must free afterwards with mem_free().
 */
gchar *
db_id_getuniqueid_asstring(Object * obj, gchar * tablename, gchar ** fieldname)
	{
	gint i;
	long pg_oid;
	gchar *retkey = NULL;

	*fieldname = NULL;

	g_assert(obj);
	if (obj->cache != NULL && obj->field != NULL && obj->row >= 0)
		for (i = obj->numfield - 1; i >= 0; i--)
			if (obj->field[i]->isuniqueid)
				if (strcmp(obj->field[i]->table, tablename) == 0)
					{
					if (obj->cache[obj->row]
					        && obj->cache[obj->row]->value[i] == NULL)
						{
						db_getvalue(obj, obj->field[i]->field,
						            obj->field[i]->table, &retkey);
						*fieldname = obj->field[i]->field;
						}
					if (obj->cache[obj->row] && obj->cache[obj->row]->value[i])
						{
						if (obj->field[i]->fielddef->datatype == DBPGOID)
							{
							memcpy(&pg_oid, obj->cache[obj->row]->value[i],
							       sizeof(long));
							retkey = mem_strdup_printf("%ld", pg_oid);
							}
						else
							retkey = mem_strdup(obj->cache[obj->row]->value[i]);
						*fieldname = obj->field[i]->field;
						return retkey;
						}
					}

	if (obj->num <= 0)
		{
		debugmsg("No records so no key to return for %s", tablename);
		return NULL;
		}

	if (obj->id && retkey == NULL)
		{
		/* Line temparerly removed -- not sure what effect will be */
		/* if (strcmp(obj->basetable,tablename)==0) */
		retkey = mem_strdup_printf("%d", obj->id->pg_oid);
		}

	if (retkey == NULL)
		{
		errormsg("Unable to generate a key for %s in object with "
		         "basetable of %s", tablename, obj->basetable);
		g_assert(NULL);
		}

	return retkey;
	}

/* In case its a new record and not written to db yet */
static gchar *
db_id_getuniqueid_fast_newrecord(Object * obj, gchar * tablename, gint row,
                       gchar ** fieldname)
	{
/*	gint i; */
	gchar *retstr;
	DbUniqueId *id;
	*fieldname = NULL;
	if (obj->cache[row])
	     	{
		globaltemporaryids --;
		
		if (obj->cache[row]->id == NULL)
		     id = db_id_remeberbyrow(obj, row);
		else
		     id = obj->cache[row]->id;
		*fieldname = mem_strdup("oid");
		retstr = mem_strdup_printf("%d",globaltemporaryids);
		/*g_strdup_printf("%d",globaltemporaryids--);*/
		/*id->pg_oid); */
		/* message("returning %s",retstr); */
		return retstr;
		}
	return NULL;
	}


gchar *
db_id_getuniqueid_fast(Object * obj, gchar * tablename, gint row,
                       gchar ** fieldname, gint *needsfreeing)
	{
	gint i;
	DbField *fielddef;
	*needsfreeing = 0;
	
	if (obj->num <= 0)
		{
		debugmsg("No record in object so cant get key (%d)",obj->num); 
		*fieldname = NULL;
		return NULL;
		}
        else if (obj->res == NULL)
	        {
		*fieldname = NULL;
		return NULL; 
		}
	else if (obj->cache[row]->state == OBJ_EDITNEW || 
		 obj->cache[row]->state == OBJ_NEW)
	     	{
		message("using walk around because new record");
		*needsfreeing = 1;
		return db_id_getuniqueid_fast_newrecord(obj, tablename, 
			    row, fieldname);
		}
	   
	if (obj->fieldhashbytable == NULL)
		{
		obj->fieldhashbytable = g_hash_table_new(g_str_hash, g_str_equal);
		}
	else
		{
		fielddef = g_hash_table_lookup(obj->fieldhashbytable, tablename);
		if (fielddef)
			{
			/* debugmsg("obj is %s. looking for %s",obj->basetable,tablename); */
			/* mem_verify(fielddef); */
			*fieldname = fielddef->field;
   		        return db_dbgetvalue(obj->res, row, fielddef->fieldposinsql);		
			}
		}

	for (i = obj->numfield-1; i >= 0; i--)
		{
		/* m.essage("Checking %s.%s", tablename, obj->field[i]->field); */

		if (obj->field[i]->isuniqueid == TRUE)
			{
			if (g_ascii_strcasecmp(obj->field[i]->table, tablename) != 0)
		        	continue;
			/* m.essage("Found item!"); */
			g_hash_table_insert(obj->fieldhashbytable, tablename,
			                    obj->field[i]);
			/* mem_verify(obj->field[i]); */
			*fieldname = obj->field[i]->field;
		        return db_dbgetvalue(obj->res, row,
			                     obj->field[i]->fieldposinsql);
			}
		}
	warningmsg("Unable to find a id for table %s", tablename);
	*fieldname = NULL;
	return NULL;
	}
