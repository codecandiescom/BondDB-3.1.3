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
#include <strings.h>
#include <glib.h>

/* Database abstration layer. This will incude the guts of the
 *	dbgather code, like the stuff in dbpgsqlgather or whatever other database.  This
 * header file will include other header files. 
 */
#include "dbwrapper.h"
#include "bc.h"

/* Other stuff which is more important */
#include "dbclient.h"
#include "dbgather.h"
#include "dbsqlparse.h"
#include "dbconstraint.h"
#include "dbdefault.h"
#include "dbpgsqlgather.h"
#include "dbcommentcallback.h"

/**
 * Build database definiations.  Query the database for information to build
 * a list of tables defined in the database by using sql queries against the database
 * and using the results to make the structure it returns.  Function returns NULL
 * on failure
 */

/* global database information variable */
DbDatabaseDef *globaldb;

/**
 * db_builddef:
 * 
 * This will probe the database your connected to for detailed information regarding everything
 * and anything.  This is normally done on startup automatically by db_init().  This function
 * may take a while to execute as it has to do a fair few sql statements to the backend.
 *
 * Returns: Database structure
 */

DbDatabaseDef *
db_builddef()
	{
	DbDatabaseDef *db = NULL;

	if (globaldb != NULL)
		return globaldb;

	/* malloc away */
	db = mem_alloc(sizeof(DbDatabaseDef));
	db->constraints = NULL;
	db->name = mem_strdup("Postgresql");
	/* basic data gathering, like tables, views, fields etc */
	db_buildtabledef(db);
	db_buildviewdef(db);
	db_buildrelationdef(db);

	/* make it global so other stuff gets it */
	globaldb = db;
	/* the more intensese but proper relationship information */
	g_assert(db_constraints_init(db));

	/* Francis: */
	db->methods = NULL;

	/*
	   { GList *wk; for (wk = g_list_first(db->constraints); wk; wk = wk->next) printf("data %p\n",
	   ((DbConstraintDef*)wk->data)->table[0]); } */
	db_comment_callback_build_constraints(db);

	db_buildfieldinfo(db);

	return db;
	}

/**
 * db_checkpgresult:
 * @result: Recordset from a query
 *
 * Check a query result from db_dbexec().  Will display an error message if the query failed.
 *
 * Returns: non-zero on failure
 */
gint
db_checkpgresult(DbRecordSet * result)
	{
	return db_dbcheckresult(result);
	}

/* Data Free and shutdown functions */
void
db_freefielddef(DbFieldDef * dbfield)
	{
	GList *walk;

	g_assert(dbfield);
	if (dbfield->name != NULL)
		mem_free(dbfield->name);
	if (dbfield->typename != NULL)
		mem_free(dbfield->typename);
	if (dbfield->comment != NULL)
		{
		walk = g_list_first(dbfield->comment);
		while (walk != NULL)
			{
			mem_free(walk->data);
			walk = walk->next;
			}
		g_list_free(dbfield->comment);
		}
	if (dbfield->defaultvalue != NULL)
		mem_free(dbfield->defaultvalue);

	}

void
db_freetabledef(DbTableDef * dbtable)
	{
	gint i;

	g_assert(dbtable);
	if (dbtable->name != NULL)
		mem_free(dbtable->name);
	if (dbtable->query != NULL)
		mem_free(dbtable->query);
	if (dbtable->uidfield != NULL)
		mem_free(dbtable->uidfield);
	if (dbtable->subtable != NULL)
		g_list_free(dbtable->subtable);
	if (dbtable->fromtable != NULL)
		g_list_free(dbtable->fromtable);
	for (i = 0; i < dbtable->num; i++)
		{
		db_freefielddef(dbtable->field[i]);
		mem_free(dbtable->field[i]);
		}
	g_list_free(dbtable->cref);

	mem_free(dbtable->field);
	}

/* EOF */

gint
db_showallfields()
	{
	gint i, j;
	DbDatabaseDef *db = globaldb;

	for (i = 0; i < db->numtable; i++)
		for (j = 0; j < db->table[i]->num; j++)
			printf("%s.%s\n", db->table[i]->name, db->table[i]->field[j]->name);

	return 0;
	}

/* no no just kidding, not EOF */

gint
db_freedatabasedef(DbDatabaseDef * db)
	{
	gint i;

	mem_verify(db);
	mem_free(db->connstr);

	for (i = 0; i < db->numtable; i++)
		{
		db_freetabledef(db->table[i]);
		mem_free(db->table[i]);
		}
	mem_free(db->table);

	for (i = 0; i < db->numview; i++)
		{
		db_freetabledef(db->view[i]);
		mem_free(db->view[i]);
		}
	mem_free(db->view);
	mem_free(db->name);
	db_constraints_uninit(db);

	/* Francis: blame me if this has problems */
	if (db->methods)
		{
		db_method_list_cleanup(db->methods);
		}

	mem_free(db);
	return 0;
	}

gint
db_buildfieldinfo(DbDatabaseDef * db)
	{
	gint j, viewoffset;
	gchar *table;
	gchar *field;
	GList *walk;
	DbTableDef *dt;
	DbFieldDef *df;
	DbConstraintDef *c;

	g_assert(db);
	db_generalisetype(db);

	walk = g_list_first(db->constraints);
	/* meaniless comment placed here for your convence */
	while (walk != NULL)
		{
		c = walk->data;
		if (c->casual == 0)
			for (j = 0; j < 1; j++)
				{
				/* find the table and field of the reference */
				table = c->table[j];
				field = c->column[j];
				dt = db_findtable(table);
				if (dt == NULL)
					continue;
				df = db_findfield(dt, field);
				if (df != NULL)
					{
					/* mark the fieldtype as a reference type */
					df->fieldtype = 1;
					/* see if a table view exists with this same field name */
					viewoffset = db_viewforthis(df->name);
					if (viewoffset != -1)
						df->fieldtype = 2;
					}
				}
		walk = walk->next;
		}
	/* todo. go though views, and if field exists for exactly same name mark that field as fieldtype 4 */
	return 0;
	}

/* Go though the database and assign a numerical number to the type in the
   general form to represent the data.  */
gint
db_generalisetype(DbDatabaseDef * db)
	{
	int x, y;
	char *typename;
	int *datatype;

	for (y = 0; y < db->numtable; y++)		 /* for each table in the database ... */
		{
		for (x = 0; x < db->table[y]->num; x++)	/* for each field in the table ... */
			{
			typename = db->table[y]->field[x]->typename;	/* shortcut to typename of field */
			if (typename == NULL)
				continue;
			g_strdown(typename);
			datatype = &db->table[y]->field[x]->datatype;	/* shortcut to datatype of field */
			*datatype = DBTEXT;			 /* Default to DBTEXT datatype */

			if ((strstr(typename, "int") != NULL) || (strstr(typename, "integer") != NULL)
			        || (strstr(typename, "long") != NULL))
				{
				*datatype = DBINT;
				}
			if ((strstr(typename, "pgoid") != NULL))
				{
				*datatype = DBPGOID;
				}
			if ((strstr(typename, "text") != NULL))
				{
				*datatype = DBTEXT;
				}
			if ((strstr(typename, "char") != NULL))
				{
				if ((strstr(typename, "varchar") != NULL))
					{
					*datatype = DBTEXT;
					}
				else
					{
					*datatype = DBCHAR;
					}
				}
			if ((strstr(typename, "float") != NULL) || (strstr(typename, "double"))
			        || (strstr(typename, "precision")))
				{
				*datatype = DBFLOAT;
				}
			if ((strstr(typename, "bool") != NULL) || (strstr(typename, "boolean") != NULL))
				{
				*datatype = DBBOOL;
				}
			if ((strstr(typename, "date") != NULL))
				{
				*datatype = DBDATE;
				}
			if ((strstr(typename, "time") != NULL))
				{
				*datatype = DBTIME;
				}
			if ((strstr(typename, "datetime") != NULL))
				{
				*datatype = DBDATETIME;
				}
			}
		}
	return 0;
	}

GList *
db_getalltablenames()
	{
	DbDatabaseDef *db;
	int i;
	GList *tables = NULL;

	db = globaldb;

	for (i = 0; i < db->numtable; i++)
		{
		g_assert(db->table[i]);
		tables = g_list_append(tables, db->table[i]->name);
		}

	return tables;
	}

GList *
db_getallviewnames()
	{
	DbDatabaseDef *db;
	int i;
	GList *views = NULL;

	db = globaldb;

	for (i = 0; i < db->numview; i++)
		{
		g_assert(db->view[i]);
		views = g_list_append(views, db->view[i]->name);
		}

	return views;
	}

GList *
db_get_fields(gchar * tablename)
	{
	DbDatabaseDef *db;
	int i;
	GList *fields = NULL;

	db = globaldb;
	g_assert(tablename);

	for (i = 0; i < db->numtable; i++)
		fields = g_list_append(fields, db->table[i]->name);

	return fields;
	}

DbTableDef *
db_findtable(gchar * table)
	{
	DbDatabaseDef *db;
	int i;

	db = globaldb;
	g_assert(table);

	for (i = 0; i < db->numtable; i++)
		if (strcmp(db->table[i]->name, table) == 0)
			return db->table[i];
	for (i = 0; i < db->numview; i++)
		if (strcmp(db->view[i]->name, table) == 0)
			return db->view[i];

	return NULL;
	}

gint
db_getfieldposintable(DbTableDef * table, gchar * field)
	{
	int i;

	g_assert(field);
	g_assert(table);
	for (i = 0; i < table->num; i++)
		{
		g_assert(table->field[i]->name);
		if (strcmp(table->field[i]->name, field) == 0)
			return i;
		}
	warningmsg("Unable to find field %s in table %s", field, table->name);
	return -1;
	}

/* build the definiation of how Object*s relate to other Object*s in the database */
gint
db_buildrelationdef(DbDatabaseDef * db)
	{
	int i;

	for (i = 0; i < db->numtable; i++)
		db_buildsubtabledef(db, db->table[i]);
	for (i = 0; i < db->numview; i++)
		db_buildsubtabledef(db, db->view[i]);
	return 0;
	}

/* find all the sub table references for lovely list... */
gint
db_buildsubtabledef(DbDatabaseDef * db, DbTableDef * table)
	{
	gint i;

	/* error checking */
	g_assert(table);
	g_assert(db);
	if (table->subtable != NULL)
		{
		errormsg("sub table is already set, can't set again");
		return -1;
		}
	/* look for references to this table though the views */
	for (i = 0; i < db->numview; i++)
		{
		g_assert(db->view[i]);
		g_assert(db->view[i]->query);
		if (db_sql_reftotable(db->view[i]->query, table->name) > 0)
			if (strcmp(db->view[i]->name, table->name) != 0)
				table->subtable = g_list_append(table->subtable, db->view[i]);
		}
	return 0;
	}

/* Find all the tables used by the view */
gint
db_extracttablesinfields(DbDatabaseDef * db, DbTableDef * table)
	{
	gint i, j, match = -1;

	if (table->fromtable != NULL)
		{
		errormsg("cant create from table glist twice");
		return -1;
		}
	for (i = 0; i < table->num; i++)
		{
		/* skip if this is the same oid as last time */
		if (match == table->field[i]->tableoid)
			continue;
		match = -1;
		for (j = 0; j < db->numtable; j++)
			if (db->table[j]->oid == table->field[i]->tableoid)
				{
				match = j;
				break;
				}
		if (match == -1)
			continue;
		if (table->fromtable != NULL)
			if (g_list_find(table->fromtable, db->table[match]) != NULL)
				continue;
		table->fromtable = g_list_append(table->fromtable, db->table[match]);
		}
	return 0;
	}

/* internal function */
gint
db_isview(DbDatabaseDef * db, gint tableoid, gchar ** retstr)
	{
	int i;

	g_assert(db);
	for (i = 0; i < db->numview; i++)
		{
		if (db->view[i]->oid == tableoid)
			{
			*retstr = db->view[i]->name;
			return 1;
			}
		}

	for (i = 0; i < db->numtable; i++)
		{
		if (db->table[i]->oid == tableoid)
			{
			*retstr = db->table[i]->name;
			return 0;
			}
		}

	return -1;
	}

/* find out how the view was made and all the source for each field */
void
db_tablesourceforallfields(DbDatabaseDef * db)
	{
	gint i, j, oid;

	for (i = 0; i < db->numview; i++)
		{
		for (j = 0; j < db->view[i]->num; j++)
			{
			/* see if we can find a tablesource for this individual field */
			oid = db_tablesourceforfield(db, db->view[i]->field[j]->name, db->view[i]->oid, db->view[i]->query);
			if (oid != -1)
				db->view[i]->field[j]->tableoid = oid;
			}
		/* build a list of what tables are used for this view */
		db_extracttablesinfields(db, db->view[i]);
		}
	}

/* does a view correspond to this name, if so return it. */
gint
db_viewforthis(gchar * viewname)
	{
	gint i;
	DbDatabaseDef *db = globaldb;

	g_assert(viewname);
	for (i = 0; i < db->numview; i++)
		if (strcasecmp(db->view[i]->name, viewname) == 0)
			return i;
	return -1;
	}

DbFieldDef *
db_findfield(DbTableDef * dt, gchar * field)
	{
	gint i;
	gchar *suggestfield;
	if (!dt)
	     {
	     warningmsg("TableDef is null, can not get field %s",field);	     
	     return NULL;
	     }
	   
	for (i = 0; i < dt->num; i++)
		if (strcmp(field, dt->field[i]->name) == 0)
			return dt->field[i];
	/* ok can't find it here, so lets try again by being a bit more agressive */
	suggestfield = db_sqlparse_suggestfield(field);
	if (suggestfield != NULL)
		{
		for (i = 0; i < dt->num; i++)
			if (strcmp(suggestfield, dt->field[i]->name) == 0)
				{
				mem_free(suggestfield);
				return dt->field[i];
				}
		mem_free(suggestfield);
		}

	return NULL;
	}

/**
 * db_default_isitafunction:
 * 
 * Find out if the default value is actually a function to call from db 
 * Not sure what value1 or value2 is, someone please fill us all in.
 */
gint
db_default_isitafunction(gchar * value1, gchar * value2)
	{
	gint i, num, matchcount = 0, cont = 0;
	gint retval = 0;
	gchar compare[10] = "FUNC";

	/* debugmsg("got %s and %s",value1,value2); */
	/* HELLLLLOO WORLD! */
	g_assert(value1);
	g_assert(value2);
	num = strlen(value1);
	/* go through and find first bracket */
	for (i = 0; i < num; i++)
		if (value1[i] == '(')
			retval = 1;
	/* find closing off bracket */
	if (retval == 1)
		{
		for (i = 0; i < num; i++)
			if (value1[i] == ')')
				retval = 2;
		/* make it once less if u get what i mean */
		retval--;
		}
	/* ok now check value2, which is the adbin field in postgresql for the clause FUNC */
	if (retval == 1)
		{
		num = strlen(value2);
		for (i = 0; i < num; i++)
			{
			if (value2[i] == compare[matchcount])
				{
				matchcount++;
				cont = 1;
				if (matchcount == strlen(compare))
					{
					/* finished here, we have a match */
					retval = 2;
					cont = 0;
					break;
					}
				}
			/* reset matchcount cause of non-consitent match */
			else if (cont == 1)
				{
				cont = 0;
				matchcount = 0;
				}
			}
		retval--;
		}
	if (retval == 0)
		{
		num = strlen(value1);
		/* looking for x::y in value1 */
		for (i = 1; i < num - 2; i++)
			{
			if (value1[i] == ':' && value1[i + 1] == ':')
				{
				retval = 1;
				break;
				}
			}
		}
	/* ps. i'm really drunk at moment if you didnt notice and i am having problems typing. drinking vodkaa. with friends
	   while watching exocides. expect 3 of 5 of us have fallen asleep on flloor. So ummm. I'm here coding. Man I'm
	   typing well at moment. excuess me fro any really bad coding styles I may while in this state */

	/* debugmsg("returning %d . yeah. for %s",retval,value1); */
	return retval;
	}

/**
 * This was added by Francis. The purpose is to provide local way to access
 * current database rather than using: extern DbDatabaseDef *db
 */
DbDatabaseDef *
db_get_current_database()
	{
	return globaldb;
	}

gboolean
db_checkfieldtableexist(gchar * table, gchar * field)
	{
	DbFieldDef *fielddef;
	DbTableDef *tabledef;

	if (table == NULL)
		return FALSE;
	if (field == NULL)
		return FALSE;
	tabledef = db_findtable(table);
	if (!tabledef)
		return FALSE;
	fielddef = db_findfield(tabledef, field);
	if (!fielddef)
		return FALSE;
	return TRUE;
	}
