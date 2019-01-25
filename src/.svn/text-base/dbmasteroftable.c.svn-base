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
#include <strings.h>
#include <glib.h>

#include "dbobject.h"
#include "dbcache.h"
#include "dbmasteroftable.h"
#include "dbfield.h"
#include "dbgather.h"
#include "dbcache.h"
#include "db.h"

#include "bc.h"
#include "sql_parser.h"

/* See bug 71 for a description of what this code attempts to fix in bond. */

/* ==============================================
 * Figuring out the plan of attack.
 * ============================================== */
static gboolean
db_masteroftable_populate_istable(DbObject * obj,
                                  sql_wherejoin * wherejoin,
                                  gchar * tablelookfor, gboolean * leftside,
                                  DbField ** field)
	{
	gint i;
	gchar *tablename = NULL;

	*field = NULL;

	if (wherejoin->rightconstaint == TRUE || wherejoin->leftconstaint == TRUE)
		return FALSE;

	if (g_list_length(wherejoin->leftfield) == 2)
		{
		tablename = (gchar *) wherejoin->leftfield->data;
		if (strcasecmp(tablename, tablelookfor) == 0)
			{
			*leftside = TRUE;
			for (i = 0; i < obj->numfield; i++)
				{
				if (!obj->field[i])
					continue;
				if (strcasecmp(obj->field[i]->field,
				               wherejoin->leftfield->next->data) == 0 &&
				        strcasecmp(obj->field[i]->table,
				                   wherejoin->leftfield->data) == 0)
					{
					*field = obj->field[i];
					return TRUE;
					}
				}
			}
		}
	else
		{
		for (i = 0; i < obj->numfield; i++)
			{
			if (!obj->field[i])
				continue;
			if (strcasecmp(obj->field[i]->field,
			               wherejoin->leftfield->data) == 0)
				{
				tablename = obj->field[i]->table;
				if (strcasecmp(tablename, tablelookfor) == 0)
					{
					*leftside = TRUE;
					*field = obj->field[i];
					return TRUE;
					}
				}
			}
		}

	if (g_list_length(wherejoin->rightfield) == 2)
		{
		tablename = (gchar *) wherejoin->rightfield->data;
		if (strcasecmp(tablename, tablelookfor) == 0)
			{
			*leftside = FALSE;
			for (i = 0; i < obj->numfield; i++)
				{
				if (!obj->field[i])
					continue;
				if (strcasecmp(obj->field[i]->field,
				               wherejoin->rightfield->next->data) == 0 &&
				        strcasecmp(obj->field[i]->table,
				                   wherejoin->rightfield->data) == 0)
					{
					*field = obj->field[i];
					return TRUE;
					}
				}
			}
		}
	else
		{
		for (i = 0; i < obj->numfield; i++)
			{
			if (!obj->field[i])
				continue;
			if (strcasecmp(obj->field[i]->field,
			               wherejoin->rightfield->data) == 0)
				{
				tablename = obj->field[i]->table;
				if (strcasecmp(tablename, tablelookfor) == 0)
					{
					*leftside = FALSE;
					*field = obj->field[i];
					return TRUE;
					}
				}
			}
		}

	return FALSE;
	}

static gint
db_masteroftable_populate_primarykey(DbField * field)
	{
	g_assert(field);
	if (field->fielddef->primary)
		return 1;
	return 0;
	}

static gint
db_masteroftable_populate_findmasterof(DbObject * obj,
                                       sql_wherejoin * wherejoin,
                                       gboolean leftside,
                                       gchar ** othertable, gchar ** otherfield)
	{
	gint i;
	GList *fieldtable;

	*othertable = NULL;
	*otherfield = NULL;

	if (leftside)
		fieldtable = wherejoin->rightfield;
	else
		fieldtable = wherejoin->leftfield;

	if (g_list_length(fieldtable) == 2)
		{
		/* Table comes first. Table.field */
		*othertable = mem_strdup((gchar *) fieldtable->data);
		*otherfield = mem_strdup((gchar *) fieldtable->next->data);
		}
	else
		{
		/* go though field list looking for table name */
		for (i = 0; i < obj->numfield; i++)
			{
			if (!obj->field[i])
				continue;
			if (strcasecmp(obj->field[i]->field,
			               (gchar *) fieldtable->data) == 0)
				{
				*othertable = mem_strdup(obj->field[i]->table);
				*otherfield = mem_strdup(obj->field[i]->field);
				}
			}
		}
	if (*otherfield == NULL)
		return -1;
	return 0;
	}

#if 0

/* walk though the list of tables that exist. Once you get
 * to your current position in the main loop (@currenttable)
 * abort because you've gone to far. Check the @field to 
 * make certain it doesnt reference a table you've already
 * done. 
 * THIS DOESNT WORK IN SOME CASES 
 */
static gint
db_masteroftable_populate_alreadyref
(GList * tablelist, gchar * currenttable,
 gboolean leftside, sql_wherejoin * wherejoin)
	{
	gchar leftsidechar;
	gchar *tablename, *othertable, *otherfield;
	GList *walk;

	for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
		{
		tablename = (gchar *) walk->data;

		if (strcasecmp(tablename, currenttable) == 0)
			break;

		/* Kinda an inverse bool. to get other side */
		if (leftside == TRUE)
			leftsidechar = 0;
		else
			leftsidechar = 1;
		sql_statement_get_wherejoin_components(wherejoin,
		                                       &othertable, &otherfield,
		                                       leftsidechar);
		if (othertable != NULL)
			if (strcasecmp(tablename, othertable) == 0)
				return 1;
		}
	return 0;
	}
#endif

/**
 * db_masteroftable_populate:
 * @obj: Database object already populated with fields.
 * 
 * Find out from the sql statement what the where clauses are and joins. 
 * Using the sqlparser, figure out which is not the base table. If it isn't
 * the base table mark it a master of the table. Tricky situation is nth
 * level depth when you have inner joins etc.  See bug 71 in bugzilla.treshna.com
 * 
 * Example SQL statement:
 * SELECT * FROM base, a, b WHERE
 * base.field1 = a.field2 AND b.field4 = a.field3
 *
 * In this case base.field1 and a.field3 are the master of the tables 
 * because they control those tables they point to and what values
 * occupy them.
 * 
 * Start at the base table. Then work outwards. Find any where
 * clauses that use the basetable in them and reference another
 * table in the where statement. So base.field1 = a.field2 is the
 * first to be found. 
 * 
 * This code uses libsql (libgda gnome src) a lot to figure out the 
 * where statement components used.
 * 
 * Returns: negtive on error.
 */
gint
db_masteroftable_populate(DbObject * obj)
	{
	gint retval = 0;
	sql_wherejoin *wherejoin;
	DbField *field;
	gchar *currenttable;
	gboolean leftside;
	GList *wherelist, *tablelist, *walk;

	/* dont like it when there is no field or numfield */
	if (!obj->field || !obj->numfield)
		return -1;
	if (!obj->statement)
		return -2;
	if (!obj->basetable)
		return -3;

	/* find all the where and join statements */
	wherelist = sql_statement_get_wherejoin(obj->statement);
	tablelist = sql_statement_get_tables(obj->statement);
	if (!tablelist)
		return -4;
	currenttable = tablelist->data;
	/* debugmsg("%d items in wherelist", g_list_length(wherelist)); */
	/* loop though a list of table in the sql select statement */
	while (currenttable != NULL)
		{
		for (walk = wherelist; walk != NULL; walk = walk->next)
			{
			wherejoin = walk->data;
			/* check if it is a join linking tables */
			if (db_masteroftable_populate_istable(obj,
			                                      wherejoin, currenttable,
			                                      &leftside, &field))
				{
				/* Ignore the other side of the where statement */
				if (!db_masteroftable_populate_primarykey(field))
					{
					g_assert(!field->tablemasterof);
					/* Have a real peice of information now */
					field->masteroftable = TRUE;
					db_masteroftable_populate_findmasterof(obj,
					                                       wherejoin,
					                                       leftside,
					                                       &field->
					                                       tablemasterof,
					                                       &field->
					                                       fieldmasterof);
					retval++;
					/* debugmsg("Master of table: %s.%s. Controlling
					   %s", field->table, field->field,
					   field->tablemasterof); */
					}
				}
			}
		/* Move to next item -> Me doing a differnt loop to break the pattern */
		if (tablelist->next)
			{
			tablelist = tablelist->next;
			currenttable = tablelist->data;
			}
		else
			currenttable = NULL;
		}

	sql_statement_free_tables(tablelist);
	sql_statement_free_wherejoin(&wherelist);

	return retval;
	}

/**
 * db_masteroftable_test:
 * @obj: Database object.
 *
 * For internal use. Checks to see if
 * if db_masteroftable works ok.
 */
gint
db_masteroftable_test(DbObject * obj)
	{
	/* db_field_debug(obj); */
	db_masteroftable_populate(obj);
	db_setvalue(obj, "countryid", "soldier", "3");
	return 0;
	}

/**
 * db_masteroftable_check:
 * @obj: Database object
 *
 * Returns: %TRUE  if this field is a master of the table.
 */
gboolean
db_masteroftable_check(DbObject * obj, gchar * fieldname, gchar * tablename)
	{
	DbField *field;

	field = db_field_dbfind(obj, fieldname, tablename);
	if (!field)
		{
		warningmsg("Unable to find %s.%s", tablename, fieldname);
		return FALSE;
		}
	if (field->masteroftable)
		return TRUE;
	return FALSE;
	}

/**
 * db_masteroftable_list:
 * @obj:
 * 
 * Gets a GList* of DbField*'s that have a master of table associated with them.
 * Free the list, but dont free the elements. 
 */
GList *
db_masteroftable_list(DbObject * obj)
	{
	gint i;
	GList *retlist = NULL;

	g_assert(obj);
	for (i = 0; i < obj->numfield; i++)
		{
		if (obj->field[i]->masteroftable == TRUE)
			{
			retlist = g_list_prepend(retlist, obj->field[i]);
			}
		}
	return retlist;
	}

/* ==============================================
 * The live operations of rebuilding objects and SQL generation.
 * ============================================== */

/* need to add sql_condition_operator here at some point
  me thinks when rebuilding the sql statement. cause you klnow. will it work ? */

/* Gets  all the field names for the table that are needed.
 * tabledef is the target table the select statement is run on */
static gchar *
db_masteroftable_action_buildfield(DbObject * obj, gchar * table,
                                   GList ** actionlist)
	{
	gint i;
	gchar *retstr = NULL, *tmpstr;
	DbTableDef *tabledef = NULL;

	*actionlist = NULL;

	for (i = 0; i < obj->numfield; i++)
		{
		/* this is to speed up operations slightly */
		if (tabledef == NULL)
			if (strcasecmp(obj->field[i]->table, table) == 0)
				tabledef = obj->field[i]->tabledef;
		/* If its the same table then add it to the SQL */
		if (obj->field[i]->tabledef == tabledef)
			{
			if (retstr == NULL)
				retstr = mem_strdup_printf("SELECT %s.%s",
				                           obj->field[i]->table,
				                           obj->field[i]->field);
			else
				{
				tmpstr = mem_strdup_printf("%s, %s.%s",
				                           retstr, obj->field[i]->table,
				                           obj->field[i]->field);
				/* flip */
				mem_free(retstr);
				retstr = tmpstr;
				}
			if (i != obj->field[i]->fieldposinsql)
				errormsg("confused over field indexpos.");
			*actionlist = g_list_append(*actionlist, obj->field[i]);
			}
		}

	return retstr;
	}

static gchar *
db_masteroftable_action_buildwhere(DbObject * obj, DbField * masterfield,
                                   gchar * newvalue)
	{
	gchar *retstr;

	/* not sure on the newvalue being null. maybe abort there */
	if (newvalue)
		retstr = mem_strdup_printf("FROM %s WHERE %s.%s = '%s' LIMIT 1",
		                           masterfield->tablemasterof,
		                           masterfield->tablemasterof,
		                           masterfield->fieldmasterof, newvalue);
	else
		retstr = mem_strdup_printf("FROM %s WHERE %s.%s IS NULL LIMIT 1",
		                           masterfield->tablemasterof,
		                           masterfield->tablemasterof,
		                           masterfield->fieldmasterof);

	return retstr;
	}

static gint
db_masteroftable_action_populatecache(DbObject * obj,
                                      DbField * masterfield, DbRecordSet * res,
                                      GList * actionlist)
	{
	gint column = 0, fieldpos;
	gchar *value;
	GList *walk;

	if (g_list_length(actionlist) != db_dbnumfields(res))
		warningmsg
		("g_list_length(actionlist) != db_dbnumfields(res) (%d != db_masteroftable_actionchange)",
		 g_list_length(actionlist) != db_dbnumfields(res));
	/* debugmsg("%d of %d. state
	   %d.\n",obj->row,obj->num,obj->currentcache->state); */

	for (walk = g_list_first(actionlist); walk != NULL; walk = walk->next)
		{
		value = db_dbgetvalue(res, 0, column);
		fieldpos = ((DbField *) walk->data)->fieldposinsql;

		if (obj->currentcache->value[fieldpos] != NULL &&
		        value != obj->currentcache->value[fieldpos])
			mem_free(obj->currentcache->value[fieldpos]);

		/* printf("%s.%s=%s from %s\n",obj->field[fieldpos]->table,
		   obj->field[fieldpos]->field,value,
		   (char*)obj->currentcache->value[fieldpos]); */

		if (value != NULL)
			obj->currentcache->value[fieldpos] = mem_strdup(value);
		else
			obj->currentcache->value[fieldpos] = NULL;
		/* move along the record set */
		column++;
		}
	return 0;
	}

static GList *
db_masteroftable_action_sql(DbObject * obj, DbField * masterfield,
                            gchar * newvalue)
	{
	GList *actionlist = NULL;
	gchar *query, *field, *where;
	DbRecordSet *res;

	field = db_masteroftable_action_buildfield(obj,
	        masterfield->tablemasterof,
	        &actionlist);
	where = db_masteroftable_action_buildwhere(obj, masterfield, newvalue);
	query = mem_strdup_printf("%s %s", field, where);
	mem_free(field);
	mem_free(where);
	debugmsg("Running: %s", query);

	res = db_dbexec(globaldbconn, query);
	/* check for errors */
	if (db_dbcheckresult(res) != 0)
		{
		errormsg("error in running query %s", query);
		mem_free(query);
		return NULL;
		}
	if (db_dbnumrows(res) != 1)
		{
		warningmsg("No records found for loading part record %s", query);
		mem_free(query);
		return NULL;
		}
	mem_free(query);

	/* everything is ok */
	db_masteroftable_action_populatecache(obj, masterfield, res, actionlist);
	db_dbclear(res);
	return actionlist;
	}

/**
 * db_masteroftable_actionchange:
 * @obj: Database object
 * 
 * 
 * 
 * Returns: A GList* of fields that were effected. This isaved to @obj.
 */
GList *
db_masteroftable_actionchange(DbObject * obj, DbField * field, gchar * newvalue)
	{
	GList *retlist = NULL;

	g_assert(field);
	g_assert(obj);

	if (field->masteroftable == FALSE)
		return NULL;
	/* if no data cant action change */
	if (obj->row < 0 || obj->row >= obj->num)
		return NULL;
	if (obj->currentcache == NULL)
		return NULL;

	debugmsg("SQL:%s\nMaster of table on %s.%s", obj->query, field->table,
	         field->field);
	retlist = db_masteroftable_action_sql(obj, field, newvalue);

	return retlist;
	}
