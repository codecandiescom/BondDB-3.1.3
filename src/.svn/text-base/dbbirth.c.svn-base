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
#include <glib.h>

#include "db.h"
#include "bc.h"

#include "dbbirth.h"
#include "dbobject.h"
#include "dbfield.h"
#include "dbconstraint.h"
#include "dbgather.h"
#include "dbtoliet.h"
#include "dbsqlparse.h"

/** Oh i'm sooo screwed */

/**
 * db_birth_addvalue:
 * @birth: Birth to assign the value to
 * @field: field in the birth statement, that was in the original sql statement
 * @value: value to assign
 *
 * Add a value into a birth structure.  
 *
 * Returns: non-zero on failure
 */
gint
db_birth_addvalue(DbBirth * birth, gchar * fieldname, gchar * value)
	{
	gint fieldpos;

	g_assert(birth);
	g_assert(fieldname);
	g_assert(value);
	fieldpos = db_field_arraypos(birth->field, birth->num, fieldname);
	if (fieldpos < 0)
		{
		errormsg("I was unable to find the field %s in the table %s", 
				fieldname, birth->tablename);
		errormsg("Invalid field position %d, hmmm. I'm looking for %s",
				fieldpos, fieldname);
		return -1;
		}
	/* free up some old data */
	if (birth->value[fieldpos] != NULL)
		mem_free(birth->value[fieldpos]);
	/* add the value */
	if (value != NULL)
		birth->value[fieldpos] = mem_strdup(value);
	else
		birth->value[fieldpos] = NULL;
	/* mark this field as essential */
	birth->essential[fieldpos] = 1;
	/* debugmsg("Added %s to %s (%s)",value,fieldname,birth->value[fieldpos]); */
	return 0;
	}

/**
 * db_birth_createsql:
 * @birth: previsouly created birth describing relationship
 *
 * Generate the sql statement needed to create the desired object set.  
 * Using the values set in the birth this will create an sql select statement 
 * from the tablename.
 * SELECT * FROM birth->tableame WHERE birth.field.name = birth.value ...
 *
 * Returns: %NULL on error else a valid birth.
 */

gchar *
db_birth_createsql(DbBirth * birth)
	{
	gchar *retstr, *tables, *endbit;
	gchar **query = NULL, **tablenames = NULL;
	gint i, j, found, numtables = 0, numwhere = 0;

	g_assert(birth);

	/* checks to see if is a simple query */
	if (birth->num < 1)
		{
		retstr = mem_strdup_printf("SELECT * FROM %s", 
				birth->tablename);
		return retstr;
		}

	query = (gchar **) mem_alloc(sizeof(gchar *) * (birth->num + 1));
	/* parse 1, get number of tables */
	for (i = 0; i < birth->num; i++)
		{
		/* dont added it to the sql if you dont need it */
		if (birth->value[i] == NULL)
			continue;

		found = 0;
		for (j = 0; j < numtables; j++)
			{
			if (strcmp(birth->field[i]->table, tablenames[j]) == 0)
				found = 1;
			}
		if (found == 0)
			{
			tablenames = (gchar **) mem_realloc(tablenames, 
					sizeof(gchar *) * (numtables + 3));
			tablenames[numtables] = mem_strdup(birth->field[i]->table);
			numtables++;
			}
		}
	/* parse 2, construct where statements */
	for (i = 0; i < birth->num; i++)
		{
		/* dont added it to the sql if you dont need it */
		if (birth->value[i] == NULL)
			continue;
		if (numtables > 1)
			query[numwhere++] =
			    mem_strdup_printf("%s.%s='%s'", 
				birth->field[i]->table, 
				birth->field[i]->field, birth->value[i]);
		else
			query[numwhere++] = mem_strdup_printf("%s='%s'", 
				birth->field[i]->field, birth->value[i]);
		}
	if (numtables == 0)
		{
		warningmsg("Nothing here, no tables to run so i'm aborting");
		return NULL;
		}
	tablenames[numtables] = NULL;
	query[numwhere] = NULL;
	tables = g_strjoinv(",", tablenames);
	endbit = g_strjoinv(" AND ", query);
	retstr = mem_strdup_printf("SELECT * FROM %s WHERE %s", tables, endbit);

	for (i = 0; i < numtables; i++)
		mem_free(tablenames[i]);

	g_free(tables);
	g_free(endbit);

	mem_free(tablenames);
	for (i = 0; i < numwhere; i++)
		mem_free(query[i]);

	mem_free(query);

	return retstr;
	}

/**
 * db_birth_createfromsql:
 * @sql: sql SELECT query string
 *
 * Given a sql select statement, create a birth object defining how it was created
 *
 * Returns: DbBirth object
 */
DbBirth *
db_birth_createfromsql(gchar * sql)
	{
	DbBirth *birth = NULL;
	DbField **fields;
	gchar **value;
	gint num, i;

	birth = (DbBirth *) mem_alloc(sizeof(DbBirth));
	memset(birth, 0, sizeof(DbBirth));
	birth->sql = mem_strdup(sql);
	birth->essential = NULL;
	birth->tablename = db_sqlparse_getfirsttable(sql);
	birth->num = db_field_numfieldbytable(birth->tablename);
	birth->field = mem_calloc(sizeof(DbField *) * (birth->num + 1));
	birth->value = mem_calloc(sizeof(gchar *) * (birth->num + 1));
	birth->essential = mem_calloc(sizeof(gchar *) * (birth->num + 1));
	for (i = 0; i < birth->num; i++)
		{
		birth->field[i] = db_field_getbytable(birth->tablename, i);
		birth->value[i] = NULL;
		}
	/* put items into the object */
	birth->id = NULL;

	num = db_sqlparse_getwherefieldsandvalues(sql, &fields, &value);
	debugmsg("Number of items %d",num);

	if (fields != NULL && value != NULL)
		{
		for (i = 0; i < num; i++)
			{
			if (value[i][0] > '9' || value[i][0] < '0')
				;
			else
				db_birth_addvalue(birth, fields[i]->field, 
						value[i]);
			db_field_free(fields[i]);
			mem_free(value[i]);
			}
		mem_free(fields);
		mem_free(value);
		}
	return birth;
	}

/**
 * db_birth_create:
 * @id: Unique id or %NULL if your not looking at loading a specific record
 * @tablename: Name of table you wish to create birth for.
 *
 * Will create a DbBirth structure, which you can then call other functions on like 
 * db_birth_addvalue().
 *
 * Returns: Newly created DbBirth contains details on how to create an object.
 */
DbBirth *
db_birth_create(DbUniqueId * id, gchar * tablename)
	{
	gint i;
	DbBirth *birth;

	g_assert(tablename);
	birth = (DbBirth *) mem_alloc(sizeof(DbBirth));
	memset(birth, 0, sizeof(DbBirth));
	birth->sql = NULL;
	birth->value = NULL;
	birth->field = NULL;
	birth->essential = NULL;
	birth->tablename = tablename;
	birth->num = 0;
	birth->id = id;
	/* get out if this is still null */
	if (tablename == NULL)
		return birth;
	/* continue with allocation then */
	birth->num = db_field_numfieldbytable(tablename);
	birth->field = (DbField **) mem_alloc(sizeof(DbField *) * birth->num);
	birth->value = (gchar **) mem_alloc(sizeof(gchar *) * birth->num);
	birth->essential = (gchar *) mem_alloc(sizeof(gchar) * birth->num);
	for (i = 0; i < birth->num; i++)
		{
		birth->value[i] = NULL;
		birth->essential[i] = 0;
		birth->field[i] = db_field_getbytable(tablename, i);
		}
	return birth;
	}

/**
 * db_birth_free:
 *
 * need to document here me feels
 */
void
db_birth_free(DbBirth * birth)
	{
	gint i;

	if (birth == NULL)
		return ;
	/* free up birth values */
	for (i = 0; i < birth->num; i++)
		{
		if (birth->value[i] != NULL)
			mem_free(birth->value[i]);
		db_field_free(birth->field[i]);
		}
	if (birth->essential != NULL)
		mem_free(birth->essential);
	if (birth->value != NULL)
		mem_free(birth->value);
	if (birth->field != NULL)
		mem_free(birth->field);
	if (birth->sql != NULL)
		mem_free(birth->sql);
	mem_free(birth);
	}

/**
 * db_birth_applycref:
 * @obj: database object source, which is used to generate new birth.  This is the source object
 * @birth: previously allocated by db_birth_createsql()
 * @cref: database constrant reference which defines the relationship between @obj and @birth
 *
 * Modify the dbbirth with db_birth_addvalue() to make it able to create an object based on the
 * cref. This is used for functions like db_loadobjectbyobject().
 *
 * Returns: non-zero on failure
 */
gint
db_birth_applycref(Object * obj, DbBirth * birth, DbConstraintDef * cref)
	{
	gint retval;
	gchar *localvalue = NULL, *destfield = NULL;
	gchar *localfield = NULL, *localtable = NULL;

	g_assert(obj);
	g_assert(birth);
	g_assert(cref);

	if (strcmp(cref->table[0], obj->basetable) == 0)
		{
		localtable = cref->table[0];
		localfield = cref->column[0];
		destfield = cref->column[1];
		}
	else if (strcmp(cref->table[1], obj->basetable) == 0)
		{
		localtable = cref->table[1];
		localfield = cref->column[1];
		destfield = cref->column[0];
		}
	else
		{
		errormsg("Invalid CREF. aborting here");
		return -1;
		}
	if (db_getvalue(obj, localfield, localtable, &localvalue) != 0)
		{
		errormsg("unable to read desired values");
		return -2;
		}
	/* debugmsg("readinng %s from %s.",localvalue,localfield); */
	if (localvalue == NULL)
		{
		debugmsg("problem occured in extracting desired value");
		return -3;
		}

	retval = db_birth_addvalue(birth, destfield, localvalue);

	return retval;
	}

/**
 * db_birth_save:
 * @obj: database object to put the birth into
 * @birth: previously allocated by db_birth_createsql() which describes how @obj was created
 * 
 * Record how @obj was created by placing the @birth into the data structure.  This is needed
 * in order to handle data refreshes and working out when values change in one object, when
 * to update other objects.
 *
 * Returns: non-zero on failure
 */
gint
db_birth_save(Object * obj, DbBirth * birth)
	{
	g_assert(obj);
	g_assert(birth);
	if (obj->birth != NULL)
		db_birth_free(obj->birth);
	obj->birth = birth;
	g_assert(birth);
	return 0;
	}

/**
 * db_birth_springchicken:
 * @obj Spring Chicken
 * @birth: Birth Mark
 *
 * Apply the birth mark to the spring chicken.
 *
 * Returns: non-zero on chicken rejecting birth mark
 */
gint
db_birth_springchicken(Object * obj)
	{
	gint i;
	DbBirth *birth;

	g_assert(obj);
	birth = obj->birth;
	if (birth == NULL)
		{
		errormsg("birth is null. highly invalid here");
		db_obj_test(obj);
		db_obj_debug(obj);
		return -1;
		}
	if (birth->value == NULL && birth->num > 0)
		{
		errormsg("Invalid birth structure");
		return -1;
		}
	for (i = 0; i < birth->num; i++)
		if (birth->value[i] && birth->essential[i] == 1)
			{
			message("adding value %s to field %s",birth->value[i],birth->field[i]->field); 
			db_setvalue_nomark(obj, birth->field[i]->field, birth->field[i]->table, birth->value[i]);
			}
	return 0;
	}

/* =======================================================================================
 * And now for something completely different...
 * =======================================================================================
 */

static GList *
db_birth_gettablelist(Object * obj)
	{
	GList *retlist = NULL;
	gint i;

	for (i = 0; i < obj->numfield; i++)
		{
		if (retlist != NULL)
			{
			if (g_list_find(retlist, obj->field[i]->table) == NULL)
				retlist = g_list_append(retlist, 
						obj->field[i]->table);
			}
		else
			retlist = g_list_append(retlist, obj->field[i]->table);
		}
	return retlist;
	}

static gboolean
db_birth_istableinside(Object * obj, GList * tablelist)
	{
	GList *walk, *othertable, *elem;

	othertable = db_birth_gettablelist(obj);
	for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
		{
		elem = g_list_find(othertable, walk->data);
		if (elem != NULL)
			{
			g_list_free(othertable);
			return TRUE;
			}
		}
	g_list_free(othertable);
	return FALSE;
	}

static gboolean
db_birth_fieldcompare(DbField * field1, DbField * field2)
	{
	g_assert(field1);
	g_assert(field2);

	if (strcmp(field1->field, field2->field) == 0 && strcmp(field2->table, field2->table) == 0)
		return TRUE;
	return FALSE;
	}

/**
 * db_birth_iseffected:
 * @objcheck: database object - object to check if the cache effects it, its the one contains the birth
 * @objchanged: database object - object that contains the cache.
 * @cache: write element to compare it against.
 *
 * Strange drunkcard coding...
 * Check to see if the @objcheck contains parts of @objchanged.
 *
 * Check to see if the @cache write will effect this object in any way, and if so, do
 * some writes to the object so it gets those values instead of the ones still sitting
 * in its recordset. This offically stops you from needing to re-run your queries in order
 * to have uptodate data.
 *
 * Returns: 1 on effected, 0 on not effected, negitive on error
 */

gboolean
db_birth_iseffected(Object * objcheck, Object * objchanged, DbCache * cache)
	{
	gint i, j, count = 0, topass = 0;
	GList *tablelist;
	DbBirth *birth;

	/* this is one hell function that required much vokda inorder to write, and or understand.  If you struggle with it
	   please help yourself to a drink. */
	if (objchanged->numfield != cache->num)
		{
		errormsg("Problem, cache->num != objchanged->num. This is on checking %s against %s", objcheck->basetable,
		         objchanged->basetable);
		warningmsg("cache->num:%d != objchanged->num:%d", cache->num, objchanged->num);
		db_obj_debug(objchanged);
		}

	/* get a list of tables this touchs */
	tablelist = db_birth_gettablelist(objcheck);
	/* see if a table occurs inside objchanged */
	if (db_birth_istableinside(objchanged, tablelist) == TRUE)
		{
		birth = objcheck->birth;
		/* if your working on the same id then of course they are effected. */
		if (birth->id != NULL)
			{
			if (db_id_compare(birth->id, cache->id) == TRUE)
				return TRUE;
			}
		/* check all the requirements for the objectcheck */
		for (i = 0; i < birth->num; i++)
			{
			if (birth->value[i] != NULL)
				{
				topass++;
				/* go though cache and see if it meets the specifications. */
				for (j = 0; j < cache->num; j++)
					{
					if (cache->value[j] != NULL)
						/* check the conditions to make sure that ... I ENDED UP SOMEHWERE ELSE */
						if (strcmp(cache->value[j], birth->value[i]) == 0)
							if ((db_birth_fieldcompare(objchanged->field[j], birth->field[i]) == TRUE))
								count++;
					}
				}
			}
		if (count == topass)
			return TRUE;
		}
	return FALSE;
	}
