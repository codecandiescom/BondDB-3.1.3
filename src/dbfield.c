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

#include <glib.h>
#include <string.h>
#include <strings.h>

#include "dbwrapper.h"
#include "dbfield.h"
#include "dbobject.h"
#include "dbobjectvalid.h"
#include "dbsqlparse.h"
#include "dbsuggest.h"
#include "bc.h"

/**
 * db_field_getpos:
 * @obj: Database Object
 * @fieldname: Field to look up
 * 
 * Find out where a field
 * 
 * Returns: negitive on failure, else the index position.
 */
gint
db_field_getpos(Object * obj, gchar * fieldname, gchar * tablename)
	{
	DbField *f;

	f = db_field_dbfind(obj, fieldname, tablename);
	if (f == NULL)
		return -1;
	return f->fieldposinsql;
	}

/**
 * db_field_read:
 * @obj: Database Object
 * @fieldname: Field to look up
 * @retval: Where to store the record after reading
 *
 * Will read current recordset based on your currently selected row in the object to get the value
 * asked for in the @fieldname.
 * 
 * Returns: non-zero on failure.
 */
gint
db_field_read(Object * obj, gchar * fieldname, gchar * tablename, char **retval)
	{
	gint pos;

	g_assert(obj);
	g_assert(fieldname);

	*retval = NULL;
	pos = db_field_getpos(obj, fieldname, tablename);
	if (pos < 0 || pos > obj->numfield)
		{
		warningmsg("Unable to find field %s in %s. \nSql : %s", fieldname, obj->basetable, obj->query);
		return -1;
		}
	if (obj->res == NULL)
		{
		warningmsg("RecordSet is NULL. I can not read field %s from %s", fieldname, obj->basetable);
		g_assert(NULL);
		return -2;
		}
	*retval = db_dbgetvalue(obj->res, obj->row, pos);
	if (*retval != NULL)
		g_strstrip(*retval);
	/* m.essage("%s read from %s.%s",*retval,tablename,fieldname); */
	return 0;
	}

/**
 * db_field_getname:
 * @obj: Database object
 * @fieldpos: Field position.
 * @fieldstr: Return string for fieldname
 * @tablestr: Return string for tablename
 *
 * Get details about a record at the field position.
 *
 * Returns: non-zero on failure
 */
gint
db_field_get(Object * obj, gint fieldpos, gchar ** fieldstr, gchar ** tablestr)
	{
	*fieldstr = NULL;
	*tablestr = NULL;
	g_assert(obj);
	g_assert(fieldpos >= 0);
	g_assert(fieldpos < obj->numfield);
	*fieldstr = obj->field[fieldpos]->field;
	*tablestr = obj->field[fieldpos]->table;
	return 0;
	}

/**
 * db_field_arraypos:
 * @field: Array of DbField, in which to search for fieldname
 * @num: number of elements in @field
 * @fieldname: Field to search for.
 *
 *
 * Returns: Negitive on error else position in array
 */
gint
db_field_arraypos(DbField ** field, gint num, gchar * fieldname)
	{
	gint i;

	for (i = 0; i < num; i++)
		{
		if (strcmp(field[i]->field, fieldname) == 0)
			return i;
		}

	return -1;
	}

/**
 * db_field_getbytable:
 *
 * Should really be in dbgather, but its here anyway.  Return a DbField based on @pos.
 * this is just another one of those nice functions. 
 *
 * Returns: %NULL on error else a newly created DbField
 */

/* note to coder: dbgather has all that you need */
DbField *
db_field_getbytable(gchar * tablename, gint pos)
	{
	DbField *field;
	DbTableDef *tabledef;
	DbFieldDef *fielddef;

	g_assert(tablename);
	tabledef = db_findtable(tablename);
	if (tabledef == NULL)
		{
		errormsg("Unable to find %s in database", tablename);
		return NULL;
		}
	if (pos >= tabledef->num || pos < 0)
		{
		errormsg("invalid pos of %d specified in %s", pos, tablename);
		return NULL;
		}
	if (tabledef->field[pos] == NULL)
		{
		errormsg("Unable to find a field at pos %d in table %s", pos, tablename);
		return NULL;
		}
	fielddef = tabledef->field[pos];
	/* Create the field object now */
	/* m.essage("creating %s",fielddef->name); */
	field = (DbField *) mem_calloc(sizeof(DbField));
	field->fielddef = fielddef;
	field->tabledef = tabledef;
	field->path = NULL;
	field->fieldposinsql = pos;
	field->field = mem_strdup(fielddef->name);
	field->table = mem_strdup(tabledef->name);
	if (strcasecmp(field->field, "oid") == 0)
		{
		field->isuniqueid = TRUE;
		field->basetable = TRUE;
		field->readonly = TRUE;
		}
	/* debugmsg("Field %s, field %s",field->fielddef->name,field->field); */
	return field;
	}

/**
 * db_field_numfieldbytable:
 *
 * Gets the number fields in a object.  
 *
 * Returns: Number of fields in the table, else negitive on failure
 */

/* Note: same as above function */
gint
db_field_numfieldbytable(gchar * tablename)
	{
	DbTableDef *tabledef;

	g_assert(tablename);
	tabledef = db_findtable(tablename);
	if (tabledef == NULL)
		{
		errormsg("Unable to find %s in database", tablename);
		return -1;
		}
	return tabledef->num;
	}

static void
db_field_expandfieldlist(Object * obj, DbField * f)
	{
	/* woow look at that saved resources now */
	if (obj->numfield % 20 == 0 || obj->numfield == 0)
		obj->field = (DbField **) mem_realloc(obj->field, sizeof(DbField *) * (obj->numfield + 21));
	f->fieldposinsql = obj->numfield;
	obj->field[obj->numfield] = f;
	obj->numfield++;
	}

static gint
db_field_addfieldlist(Object * obj, GList * fieldlist)
	{
	DbField *f;
	GList *walk;

	g_assert(obj);
	for (walk = g_list_first(fieldlist); walk != NULL; walk = walk->next)
		{
		f = walk->data;
		if (f == NULL)
			continue;
		/* check to see if data is acutally in the record set */
		/* (db_field_checkinresult(obj->res,f)!= 0) { errormsg("Unable to find %s in the record set",f->fieldname);
		   continue; } */

		/*		if (db_field_dbfind(obj, f->field, f->table) == NULL) */
		db_field_expandfieldlist(obj, f);
		}
	return 0;
	}

/**
 * db_field_create:
 * @fielddef: 
 * @tabledef: 
 * 
 * Create a DbField from a DbFielddef and DbTableDef.
 *
 * Returns: created DbField object
 */
DbField *
db_field_create(DbFieldDef * fielddef, DbTableDef * tabledef)
	{
	DbField *field;
	g_assert(fielddef);
	g_assert(tabledef);
	field = (DbField *) mem_calloc(sizeof(DbField));
	field->tabledef = tabledef;
	field->fielddef = fielddef;
	field->path = NULL;
	field->field = mem_strdup(fielddef->name);
	field->table = mem_strdup(tabledef->name);
	if (strcasecmp(fielddef->name, "oid") == 0)
		{
		field->isuniqueid = TRUE;
		field->readonly = TRUE;
		}
	return field;
	}

/**
 * db_field_populate:
 * @obj: database object
 * @fieldlist: list of field/tables.
 * 
 * Fill in all the information about that object which it doens't already know.  DbField, number 
 * of fields, what tables fields are from, and the name of table are added.  It hopes
 * that there is a recordset present, else it uses db_field_populate_single(). 
 * 
 * Returns: non-zero on failure
 */
gint
db_field_populate(Object * obj, GList * fieldtablelist, gchar * basetable)
	{
	DbFieldDef *fd;
	DbTableDef *td;
	DbField *field;
	GList *walk;

	g_assert(obj);
	db_field_freeall(obj);

	/* If there is no record set on this object then just guess, and if you cant guess give up */
	if (fieldtablelist == NULL)
		{
		errormsg("No fields selected for %s", obj->query);
		g_assert(NULL);
		return -2;
		}
	obj->field = (DbField **) mem_calloc(sizeof(DbField *) * ((g_list_length(fieldtablelist) / 2) + 1));
	for (walk = g_list_first(fieldtablelist); walk != NULL; walk = walk->next)
		{
		td = walk->data;
		walk = walk->next;
		fd = walk->data;
		if (!td || !fd)
		     continue;
		
		field = db_field_create(fd, td);
		if (basetable != NULL)
			if (strcmp(basetable, td->name) == 0)
				{
				field->basetable = TRUE;
				if (obj->basetable != NULL)
					mem_free(obj->basetable);
				obj->basetable = mem_strdup(basetable);
				}
		field->fieldposinsql = obj->numfield;
		obj->field[obj->numfield++] = field;
		}
	return 0;
	}

/**
 * db_field_allfieldsinresult:
 * @obj: Database object which has a already excuted SQL statement.
 * 
 * Given an SQL statement this returns a GList* of all the returned fields
 * in the sql statement. Use db_sqlparse_freeselectfields(); to free the results
 * of this function.
 *
 * Returns: GList* of items, use g_list_free on it when done.
 */
GList *
db_field_allfieldsinresult(Object * obj)
	{
	gchar *fieldname;
	gint numfield, i;
	GList *retlist = NULL;

	g_assert(obj);
	numfield = db_dbnumfields(obj->res);
	for (i = 0; i < numfield; i++)
		{
		fieldname = db_dbfieldname(obj->res, i);
		retlist = g_list_prepend(retlist, mem_strdup(fieldname));
		}
	return retlist;
	}

/**
 * db_field_populate_singletable:
 * @obj: Database object 
 *
 * Go to dbgather and use the information in obj->name to get the number of fields and fill in each
 * dbfield for that single table. Uses db_field_getbytable(). Its a simple handler for emtpy datasources 
 * on single table objects.
 * 
 * Returns: non-zero on error.
 */
gint
db_field_populate_singletable(Object * obj)
	{
	DbField *f;
	GList *retlist = NULL;
	gint i, num;

	g_assert(!obj->field);

	if (obj->basetable == NULL)
		{
		errormsg("Empty object. hmm. i know nothing.  Please assign a name to this object prior.");
		return -1;
		}
	num = db_field_numfieldbytable(obj->basetable);
	for (i = 0; i < num; i++)
		{
		f = db_field_getbytable(obj->basetable, i);
		retlist = g_list_append(retlist, f);
		}
	/* put items into the object */
	db_field_addfieldlist(obj, retlist);
	/* free up */
	g_list_free(retlist);

	return 0;
	}

/**
 * db_field_getbyfield:
 * @fieldname: The fieldname we are looking for
 * @tablename: The table to look for the field in
 * 
 * Create a DbField for @fieldname in table @tablename. 
 *
 * Returns: created DbField object
 */
DbField *
db_field_getbyfield(gchar * fieldname, gchar * tablename)
	{
	DbField *field;
	gint i;

	g_assert(tablename);
	field = (DbField *) mem_calloc(sizeof(DbField));
	field->tabledef = db_findtable(tablename);
	if (field->tabledef == NULL)
		{
		errormsg("Unable to find %s.%s in database", tablename, fieldname);
		mem_free(field);
		return NULL;
		}

	for (i = 0; i < field->tabledef->num; i++)
		{
		if (strcmp(fieldname, field->tabledef->field[i]->name) == 0)
			{
			field->fielddef = field->tabledef->field[i];
			field->fieldposinsql = i;
			}
		}
	field->path = NULL;
	field->field = mem_strdup(fieldname);
	field->table = mem_strdup(tablename);
	return field;
	}

/**
 * db_field_free:
 * @field: Field to be freed
 *
 * Free up a dbfield block
 */
void
db_field_free(DbField * field)
	{
	/* m.essage("freeing up field %s",field->field); */
	if (field->field != NULL)
		mem_free(field->field);
	if (field->table != NULL)
		mem_free(field->table);
	if (field->path != NULL)
		{
		g_list_free(field->path->tablepath);
		mem_free(field->path->field);
		mem_free(field->path->finaltable);
		mem_free(field->path);
		}
	if (field->tablemasterof)
		mem_free(field->tablemasterof);
	if (field->fieldmasterof)
		mem_free(field->fieldmasterof);
	mem_free(field);
	}

/**
 * db_field_freeall:
 * 
 * Free all the field values in @obj.
 */
void
db_field_freeall(Object * obj)
	{
	gint i;

	g_assert(obj);
	if (obj->field == NULL)
		return ;
	/* m.essage("freeing up %d items",obj->numfield); */
	for (i = 0; i < obj->numfield; i++)
		db_field_free(obj->field[i]);
	mem_free(obj->field);
	if (obj->fieldhashbytable)
		g_hash_table_destroy(obj->fieldhashbytable);
	obj->fieldhashbytable = NULL;
	obj->field = NULL;
	obj->numfield = 0;
	}

/**
 * db_field_test
 * @obj: database object with field information already set
 *
 * Go though the field information in @obj and work out the fieldpos for each item in the record set,
 * well atleast verify that this stuff is right you know.
 *
 * Returns: non-zero on error
 */
gint
db_field_test(Object * obj)
	{
	gint i, retval = 0;
	gchar *tmpstr;

	if (db_isrecordset(obj) == FALSE)
		return -1;
	for (i = 0; i < obj->numfield; i++)
		{
		tmpstr = db_dbfieldname(obj->res, i);
		if (strcmp(tmpstr, obj->field[i]->field) != 0)
			{
			errormsg("what i think is in record set and what is, is differnet. %s != %s",
			         tmpstr, obj->field[i]->field);
			retval++;
			}
		}
	return retval;
	}

/**
 * db_field_find:
 * @obj: database object
 * @fieldname: fieldname to search for
 * @tablename: table where it resides, or %NULL for default table.
 * 
 * Find @fieldname in the field list in the object, else 
 * returns %NULL if it can't find it. If you pass in %NULL 
 * for the tablename it will first of all look for that field in 
 * your base table (obj->name) and if that fails it will look
 * for any general match which it will return.
 *
 * Returns: %NULL if you can't find it else the DbField in 
 * object. Do not free this variable
 */
DbField *
db_field_dbfind(Object * obj, gchar * fieldname, gchar * tablename)
	{
	gint i;

	g_assert(obj);
	if (tablename == NULL)
		tablename = obj->basetable;

	if (tablename == NULL)
		{
		for (i = 0; i < obj->numfield; i++)
			if (strcmp(fieldname, obj->field[i]->field) == 0)
				return obj->field[i];
		return NULL;
		}

	for (i = 0; i < obj->numfield; i++)
		if (strcmp(fieldname, obj->field[i]->field) == 0 && strcmp(tablename, obj->field[i]->table) == 0)
			return obj->field[i];

	return NULL;
	}

DbField *
db_field_copy(DbField * srcfield)
	{
	DbField *field = NULL;

	g_assert(srcfield);

	field = mem_alloc(sizeof(DbField));
	memcpy(field, srcfield, sizeof(DbField));
	field->field = mem_strdup(srcfield->field);
	field->table = mem_strdup(srcfield->table);

	return field;
	}

gchar *
db_field_gettable(Object *obj, gchar *fieldname)
	{
	DbField *f;
	g_assert(obj);
	g_assert(fieldname);

	f = db_field_dbfind(obj, fieldname, NULL);
	return f->table;
	}

void 
db_field_debug(Object *obj)
	{
	gint i;
	g_assert(obj);
	for (i=0;i<obj->numfield;i++)
		{
		debug_output("%d: %s.%s \t",
			obj->field[i]->fieldposinsql,
			obj->field[i]->table,
			obj->field[i]->field);
		if (obj->field[i]->birthvalue)
			debug_output("Birth: %s ",(gchar*)obj->field[i]->birthvalue);
		if (obj->field[i]->isuniqueid)
			debug_output("Key ");
		if (obj->field[i]->basetable)
			debug_output("B ");
		if (obj->field[i]->masteroftable)
			debug_output("M(%s) ",obj->field[i]->tablemasterof);
		if (obj->field[i]->readonly)
			debug_output("R ");
		debug_output("\n");
		}
	}
