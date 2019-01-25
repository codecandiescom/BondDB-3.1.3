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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <glib.h>

#include "dbverifysql.h"
#include "dbsqlparse.h"
#include "dbgather.h"
#include "dbcache.h"
#include "dbobject.h"
#include "dbsuggest.h"
#include "dbconstraint.h"
#include "sql_parser.h"
#include "bc.h"

static void
db_verifysql_handlefailure(Object * obj, gchar * orginalsql, GList ** fieldlist, GList ** tablelist)
	{
	*fieldlist = NULL;
	*tablelist = NULL;

	if (!obj->query)
		{
		errormsg("No valid SQL statement.");
		return ;
		}
	}

static GList *
db_verifysql_adduniqueids(sql_statement * statement, GList * fieldtablelist, GList * fieldlist, GList * tablelist)
	{
	DbFieldDef *fd;
	DbTableDef *td;
	gboolean found;
	GList *walk, *fwalk;
	gchar *tablename;
	gchar *lookfor;
        
	g_assert(statement);
	for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
		{
		tablename = walk->data;
		found = FALSE;
		lookfor = mem_strdup_printf("%s.oid", tablename);
		for (fwalk = g_list_first(fieldlist); fwalk != NULL; fwalk = fwalk->next)
			if (strcasecmp(fwalk->data, lookfor) == 0)
				{
				found = TRUE;
				break;
				}
		if (found == FALSE)
			{
			/* sql_display(statement); */
			sql_statement_append_field(statement, tablename, "oid", NULL);
			td = db_findtable(tablename);
			fd = db_findfield(td, "oid");
			fieldtablelist = g_list_append(fieldtablelist, td);
			fieldtablelist = g_list_append(fieldtablelist, fd);
			debuglog(90, "Adding %s.oid to sql statement", tablename);
			}
		mem_free(lookfor);
		}
	return fieldtablelist;
	}

static DbFieldDef *
db_verifysql_findtablefromfield(gchar * fieldname, GList * tablelist)
	{
	GList *walk;
	DbTableDef *td;
	DbFieldDef *fd;

	for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
		{
		td = db_findtable(walk->data);
		if (td == NULL)
			{
			warningmsg("Unable to find table %s", (gchar*)walk->data);
			continue;
			}

		fd = db_findfield(td, fieldname);
		if (fd != NULL)
			return fd;
		}
	return NULL;
	}

static GList *
db_verifysql_addfield(GList * retlist, DbFieldDef * fielddef, DbTableDef * tabledef)
	{
	retlist = g_list_prepend(retlist, tabledef);
	retlist = g_list_prepend(retlist, fielddef);
	return retlist;
	}

static GList *
db_verifysql_wildcard(GList * retlist, gchar * tablename, GList * tablelist)
	{
	GList *walk;
	gint i;
	DbTableDef *td;

	/* db->num-1 is used because the last entry in td is a oid field, which should be left to the parser later on to add
	 */
	/* if tablename is specifiied */
	if (tablename != NULL)
		{
		td = db_findtable(tablename);
		for (i = 0; i < td->num - 1; i++)
			retlist = db_verifysql_addfield(retlist, td->field[i], td);
		}
	else
		{
		for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
			{
			td = db_findtable(walk->data);
			if (!td)
			     {
			     continue;
			     }
			for (i = 0; i < td->num - 1; i++)
				retlist = db_verifysql_addfield(retlist, td->field[i], td);
			}
		}
	return retlist;
	}

static GList *
db_verifysql_gettablefieldlist(GList * fieldlist, GList * tablelist)
	{
	DbFieldDef *fielddef;
	gchar *inputfield;
	gchar *fieldname;
	gchar *tablename;
	gint wildcard;
	GList *retlist = NULL;
	GList *walk;

	for (walk = g_list_first(fieldlist); walk != NULL; walk = walk->next)
		{
		inputfield = walk->data;
		fielddef = NULL;
		wildcard = db_suggest_wildcard(inputfield, &fieldname, &tablename);
		/* direct table.field fieldname */
		if (wildcard == 0)
			{
			if (tablename == NULL && fieldname != NULL)
				{
				fielddef = db_verifysql_findtablefromfield(fieldname, tablelist);
				if (fielddef)
					tablename = mem_strdup(fielddef->tabledef->name);
				}
			else if (tablename != NULL && fieldname != NULL)
				{
				fielddef = db_findfield(db_findtable(tablename), fieldname);
				}
			}
		if (wildcard == 1)
			{
			retlist = db_verifysql_wildcard(retlist, tablename, tablelist);
			if (fieldname)
				mem_free(fieldname);
			if (tablename)
				mem_free(tablename);
			continue;
			}

		/* if not wildcard add one item */
		if (fieldname != NULL && tablename != NULL && fielddef != NULL)
			retlist = db_verifysql_addfield(retlist, fielddef, fielddef->tabledef);
		else
			warningmsg("Unable to to determine field or tablename in database on a select statement. %s.%s", tablename,
			           fieldname);
		/* cleanup */
		if (fieldname)
			mem_free(fieldname);
		if (tablename)
			mem_free(tablename);
		}
	retlist = g_list_reverse(retlist);
	return retlist;
	}

/**
 * db_verifysql_parse:
 * @obj: Database object
 * @orginalsql: The orginal sql statement
 * 
 * Check the sql statement, and if it can be passed successfully.  Add any
 * unique id loading that needs to be done.
 * 
 * Returns: non-zero on error.
 */
gint
db_verifysql_parse(Object * obj, gchar * orginalsql)
	{
	gchar *tmpstr, *basetable;
	GList *fieldtablelist;
	GList *fieldlist;
	GList *tablelist;

	g_assert(obj);
	if (obj->statement)
		{
		errormsg("Sql statement already exists.");
		warningmsg("%s",sql_stringify(obj->statement));
		}
	g_assert(!obj->statement);

	obj->statement = sql_parse(orginalsql);
	if (obj->statement == NULL)
		{
		obj->sqlgood = FALSE;
		debuglog(50, "Failed to parse sql statement %s", orginalsql);
		db_verifysql_handlefailure(obj, orginalsql, &fieldlist, &tablelist);
		sql_statement_free_fields(fieldlist);
		sql_statement_free_tables(tablelist);
		return -1;
		}
	/* get some basic information */
	if (obj->statement->type == SQL_select)
		{
		fieldlist = sql_statement_get_fields(obj->statement);
		tablelist = sql_statement_get_tables(obj->statement);
		if (tablelist == NULL || fieldlist == NULL)
			{
			errormsg("field or table passed in %s is NULL", orginalsql);
			return -2;
			}
		else
			basetable = tablelist->data;
		fieldtablelist = db_verifysql_gettablefieldlist(fieldlist, tablelist);
		/* add pog_oid for every table in the select statement. Do a as to uniqueid */
		fieldtablelist = db_verifysql_adduniqueids(obj->statement, fieldtablelist, fieldlist, tablelist);
		/* create the sql statement. */

		tmpstr = sql_stringify(obj->statement);
		if (obj->query)
			mem_free(obj->query);
		obj->query = mem_strdup_printf("%s", tmpstr);
		g_free(tmpstr);
		if (obj->basetable == NULL && basetable)
			obj->basetable = mem_strdup(basetable);
		/* populate that field goodly. */
		if (obj->sqlready == TRUE)
			{
			db_field_populate(obj, fieldtablelist, basetable);
			obj->sqlgood = TRUE;
			}
		else
			{
			sql_destroy(obj->statement);
			obj->statement = NULL;
			obj->sqlgood = FALSE;
			}

		/* cleanup, make sure i free everything */
		sql_statement_free_fields(fieldlist);
		sql_statement_free_tables(tablelist);
		g_list_free(fieldtablelist);
		}

	return 0;
	}

static gboolean
db_verifysql_tableinlist(GList * tablelist, gchar * tablename)
	{
	GList *walk;

	for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
		if (strcasecmp(tablename, (gchar *) walk->data) == 0)
			return TRUE;
	return FALSE;
	}

static gint
db_verifysql_addfieldtable_appendfield(Object * obj, gchar * fieldname, gchar * tablename)
	{
	DbTableDef *td;
	DbFieldDef *fd = NULL;
	DbField *field;

	td = db_findtable(tablename);
	if (td != NULL)
		fd = db_findfield(td, fieldname);
	if (fd == NULL)
		{
		errormsg("Unable to find %s.%s", tablename, fieldname);
		return -1;
		}

	obj->field = (DbField **) mem_realloc(obj->field, sizeof(DbField *) * (obj->numfield + 1));
	field = db_field_create(fd, td);
	field->fieldposinsql = obj->numfield;
	obj->field[obj->numfield++] = field;
	return 0;
	}

static gint
db_verifysql_addjoin(Object * obj, GList * tablelist, gchar * tablename)
	{
	DbConstraintDef *c;
	gchar *check;
	gchar *left = NULL;
	gchar *right = NULL;
	gchar *leftfield = NULL;
	gchar *rightfield = NULL;
	GList *walk, *innerwalk;

	g_assert(obj);
	g_assert(tablelist);
	g_assert(tablename);

	right = tablename;

	for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
		{
		/* break out condiition. found the correct answer */
		if (rightfield)
			break;
		check = walk->data;
		for (innerwalk = g_list_first(globaldb->constraints); innerwalk != NULL; innerwalk = innerwalk->next)
			{
			c = innerwalk->data;
			if (strcasecmp(c->table[0], check) == 0)
				if (strcasecmp(c->table[1], right) == 0)
					{
					left = check;
					leftfield = c->column[0];
					rightfield = c->column[1];
					break;
					}
			}
		}
	if (leftfield != NULL && rightfield != NULL)
		return sql_statement_append_tablejoin(obj->statement, left, right, leftfield, rightfield);
	return -1;
	}

/**
 * db_verifysql_addfieldtable:
 * @obj:
 *
 * Append a @tablename.@fieldname onto a exisiting database object. An SQL
 * statement must already be associated with the object before running this.
 */
gint
db_verifysql_addfieldtable(Object * obj, gchar * fieldname, gchar * tablename)
	{
	gint toadd;
	gchar *tmpstr;
	GList *tablelist, *fieldlist;

	g_assert(obj);
	if (!obj->statement)
		{
		errormsg("Can not add %s.%s to object %s because no sql statement exists. Query:%s", tablename, fieldname,
		         obj->basetable, obj->query);
		return -1;
		}
	if (db_checkfieldtableexist(tablename, fieldname) == FALSE)
		{
		errormsg("%s.%s doesn't exist in the database. "
			 "I can not extract them.\n%s", 
			 tablename, fieldname, obj->query);
		return -2;
		}
	fieldlist = sql_statement_get_fields(obj->statement);
	tablelist = sql_statement_get_tables(obj->statement);

	if (tablename == NULL)
		tablename = obj->basetable;
	mem_free(obj->query);
	obj->query = NULL;
	toadd = obj->numfield;
	/* Check if we already know about this table. If not we need to handle this */
	if (db_verifysql_tableinlist(tablelist, tablename) == FALSE)
		{
		/* add table and where statement */

		db_verifysql_addjoin(obj, tablelist, tablename);
		/* append oid */
		sql_statement_append_field(obj->statement, tablename, "oid", NULL);
		db_verifysql_addfieldtable_appendfield(obj, "oid", tablename);
		toadd++;
		}
	toadd++;
	/* Add the field to the sql statement */
	sql_statement_append_field(obj->statement, tablename, fieldname, NULL);
	/* create a bit more in the field statement */
	db_verifysql_addfieldtable_appendfield(obj, fieldname, tablename);
	/* expand the cache to accomidate new values */
	db_cache_expandfield(obj, toadd);
	/* stringify the whole thing */
	tmpstr = sql_stringify(obj->statement);
	obj->query = mem_strdup(tmpstr);
	g_free(tmpstr);
	debugmsg("Sql statement is now %s", obj->query);
	sql_statement_free_fields(fieldlist);
	sql_statement_free_tables(tablelist);

	return 0;
	}
