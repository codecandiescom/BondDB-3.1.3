
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
#include <glib.h>

#include "dbwrapper.h"
#include "bc.h"
#include "dbdefault.h"
#include "dbclient.h"
#include "db.h"
#include "dbobject.h"
#include "dbfield.h"

/**
 * db_default_populateobject:
 *
 * Populate a object which is a create item with all teh default values
 * its suppose to have.
 *
 */
gint
db_default_populateobject(Object * obj)
	{
	gint i, j, slen, retval = 0;
	gchar *defvalue;
	DbFieldDef *field;

	g_assert(obj);
	/* m.essage("populating table %s",obj->name); */

	if (db_isnewrecord(obj) != TRUE)
		{
		warningmsg("this isn't a new record, cant apply defaults to %s", obj->basetable);
		return -1;
		}
	for (i = 0; i < obj->numfield; i++)
		{
		/* check if there is a default value there */
		field = obj->field[i]->fielddef;
		if (field == NULL)
			continue;
		/* find the default value */
		if (field->defaultfunc == 1)
			defvalue = mem_strdup("EXECFUNCTION");
		else
			defvalue = mem_strdup(field->defaultvalue);
		/* debugmsg("defvalue is %s and func %d",field->defaultvalue,field->defaultfunc); */
		/* if we dont have a null default value to apply */
		if (defvalue == NULL)
			continue;
		slen = strlen(defvalue);
		if (defvalue[0] == 39 && defvalue[slen - 1] == 39)	/* ' character */
			{
			for (j = 1; j < slen - 1; j++)
				defvalue[j - 1] = defvalue[j];
			defvalue[j - 1] = 0;
			}

		/* add the default value to the object */
		/* debugmsg("Adding write of %s into %s", defvalue, obj->field[i]->field); */
		retval -= db_obj_addwrite(obj, obj->field[i]->field, obj->field[i]->table, defvalue, FALSE);
		mem_free(defvalue);
		}

	return retval;
	}

/* run a sql function to get the default value .. */
gchar *
db_default_execfunction(gchar * function)
	{
	DbRecordSet *res;
	gchar *query;
	gchar *retstr = NULL;

	debugmsg("Executing SQL function %s", function);
	/* clear the contents of the object before creating a new isntance of it */
	query = mem_strdup_printf("SELECT %s", function);
	res = db_dbexec(globaldbconn, query);
	if (db_checkpgresult(res) != 0)
		{
		/* Problem here cause it == null stuff */
		warningmsg("error in getting default value for %s", function);
		mem_free(query);
		return function;
		}
	if (db_dbnumrows(res) <= 0)
		{
		/* Problem here cause it == null stuff */
		warningmsg("no result for getting default value for %s", function);
		mem_free(query);
		db_dbclear(res);
		return function;
		}

	retstr = mem_strdup(db_dbgetvalue(res, 0, 0));

	db_dbclear(res);
	mem_free(query);
	debugmsg("%s returning for function %s... ", retstr, function);
	/* i Am under the false impression that i write brillant code under the inflence of drugs. Tommorow will really tell
	   when i test the bugs if this really is the case.  Personally, being 3:50am I think it is true. I write brillant
	   code when sloshed... */
	/* I came across the code, after not remebering writing it and have amazed, being so many months after I wrote that
	   i never had to debug it?. */
	/* Note to self and others., drink more alchomle while coding. */

	return retstr;
	}

/**
 * db_default_addwrite:
 * @obj: database object to add the write to
 * @field: field to add the write to
 * @value: new value to set it to
 * 
 * Write a function add to write cache for an object the default values. it sets
 * changed to false but still updates the write cache. hmmm.
 * 
 * Returns: non-zero on failure
 */
gint
db_default_addwrite(Object * obj, gchar * field, gchar * table, gchar * value)
	{
	gboolean oldchange;

	g_assert(obj);
	g_assert(field);
	if (value == NULL)
		return -1;
	/* man i have an annoyingly massive hangover . now next moring and agrrr coding agrrr nomore drninkess. */
	oldchange = obj->changed;
	db_obj_addwrite(obj, field, table, value, FALSE);
	return 0;
	}

/**
 * db_default_value:
 * @tabledef: Database table defination.
 * @fieldname: Fieldname in the database.
 *
 * the simple one, just returns the value as defined in the db.
 *
 * Returns: gchar* value of what is the default value. Please free this with mem_free().
 */

gchar *
db_default_value(DbTableDef * db, gchar * fieldname)
	{
	DbFieldDef *field;

	g_assert(db);
	g_assert(fieldname);
	field = db_findfield(db, fieldname);
	if (field == NULL)
		return NULL;
	/* hmmmmmmm errr call the function to get the value of the default widget */
	if (field->defaultfunc == 1)
		return db_default_execfunction(field->defaultvalue);
	return mem_strdup(field->defaultvalue);
	}
