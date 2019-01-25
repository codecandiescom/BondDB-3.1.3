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

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include "bc.h"
#include "db.h"

#include "dbsync.h"
#include "dbobject.h"

static gint
db_sync_loadobj(DbObject *dbobj, DbSync *dbsync, gchar *newvalue)
{
   gchar *query;
   g_assert(dbsync->query);
   g_assert(dbsync->fieldnamekey);
	message("running loadby");
   query = mem_strdup_printf("%s WHERE %s.%s=%s",dbsync->query,dbsync->tablename,dbsync->fieldnamekey,newvalue);
   dbsync->dbobj = db_sql(dbsync->dbobj,query);
   mem_free(query);

   if (db_numrecord(dbsync->dbobj)==1)
     return 0;
	warningmsg("Failed to retrieve record for sync");
   return -1;
}

/* Add another dbobj that contains all the possible values that can be on the sync */
static gint
db_sync_init_loadobj(DbObject *dbobj, DbSync *dbsync)
	{
	GList *fieldlist = NULL;
	gchar *tmpstr, *query, *uniqueid, *keyfield;
	gint i;
	gint retval=0;
	
	query = mem_strdup("SELECT"); 
	
	/* Get all the fields that need obstraction */
	for (i=0;i<dbobj->numfield;i++)
		if (strcmp(dbobj->field[i]->table,dbsync->tablename)==0)
			{
			if (retval == 1)
				tmpstr = mem_strdup_printf("%s, %s.%s",query, dbobj->field[i]->table, dbobj->field[i]->field);
			else
				tmpstr = mem_strdup_printf("%s %s.%s",query, dbobj->field[i]->table, dbobj->field[i]->field);
			mem_free(query);
			query = tmpstr;
			fieldlist = g_list_prepend(fieldlist,dbobj->field[i]);
			retval = 1;
			}
	
	if (retval == 0)
		{
		mem_free(query);
		warningmsg("Table %s not found in sql statement %s",dbsync->tablename,dbobj->query);
		return -1;
		}
	else
		{
		/* add rest of bits onto sql */
		tmpstr = mem_strdup_printf("%s FROM %s",query,dbsync->tablename);
		mem_free(query);
		query = tmpstr;
		/* Save items to dbsync */
		dbsync->query = query;
		dbsync->fieldlist = fieldlist;
		}
	return 0;
	}

/* Finds the join statement */
static gint
db_sync_init_join(DbObject *dbobj, DbSync *dbsync)
	{
	
	return 0;
	}
	
/* Finds the where statement */
static gint
db_sync_init_where(DbObject *dbobj, DbSync *dbsync)
	{
	
	return 0;
	}

static gint
db_sync_init_dependon(DbObject *dbobj, DbSync *dbsync)
	{
	
	return 0;
	}

void 
db_sync_free(DbObject *dbobj)
	{
	DbSync *dbsync;
	GList *walk;
	for (walk=g_list_first(dbobj->synclist);walk!=NULL;walk=walk->next)
		{
		dbsync = walk->data;
		if (dbsync->fieldlist)
			g_list_free(dbsync->fieldlist);
		mem_free(dbsync->query);
	   mem_free(dbsync);
		}
	g_list_free(dbobj->synclist);
	}

gint 
db_sync_init(DbObject *dbobj, gchar *tablename)
	{
	gint retval = 0;
	DbSync *dbsync;
	DbObject *retobj;
	
	g_assert(dbobj);
	
	dbsync = mem_calloc(sizeof(DbSync));
	dbsync->tablename = tablename;
	retval -= db_sync_init_loadobj(dbobj, dbsync);
	retval -= db_sync_init_dependon(dbobj, dbsync);
	if (!retval)
		dbsync->syncset = TRUE;
	dbobj->synclist = g_list_prepend(dbobj->synclist,dbsync);
	return retval;
	}


gint 
db_sync_getvalues(DbObject *dbobj, gchar *synctablename, GList **retfield, GList **retvalue)
	{
	gint retval=1;
	DbSync *dbsync = NULL;
	GList *walk;
	gchar *value;
	DbField *field;
	
	*retfield = NULL;
	*retvalue = NULL;
	for (walk=g_list_first(dbobj->synclist);walk!=NULL;walk=walk->next)
		{
		dbsync = walk->data;
		if (strcmp(dbsync->tablename,synctablename) == 0)
			break;
		}
	if (!dbsync)
		{
		errormsg("Table was not found");
		return -1;
		}
	
	for (walk=g_list_first(dbsync->fieldlist);walk!=NULL;walk=walk->next)
		{
		field = walk->data;
		db_getvalue(dbsync->dbobj,field->field,field->table,&value);
		(*retvalue) = g_list_append((*retfield),value);
		retval = 0;
		}
	
	*retfield = dbsync->fieldlist;
	return retval;
	}

gint 
db_sync_row(DbObject *dbobj, gchar *synctablename, gchar *tablename, gchar *fieldname, gchar *newvalue)
	{
	gint retval;
	gchar *value;
	gboolean found=FALSE;
	DbSync *dbsync;
	GList *walk;
	/* check conditions */
	g_assert(dbobj);
	g_assert(synctablename);
	g_assert(tablename);
	g_assert(fieldname);
	g_assert(newvalue);
	if (synctablename == dbobj->basetable)
		{
		warningmsg("basetable is same as ask for table %s",synctablename);
		return -1;
		}
	/* find dbsync object, if not exist create */	
	for (walk=g_list_first(dbobj->synclist);walk!=NULL;walk=walk->next)
		{
		dbsync = walk->data;
		if (strcmp(dbsync->tablename,synctablename) == 0)
			{
			found = TRUE;
			break;
			}
		}
	if (!found)
		{
		if (db_sync_init(dbobj, synctablename))
			{
			errormsg("Failed to intialise a sync with table %s",synctablename);
			return -1;
			}
		dbsync = dbobj->synclist->data;
		}
	/* now actual work */
	
	/* Go though dependent fields and get new values, so they are assigned to cache */
	retval = db_sync_loadobj(dbobj, dbsync, newvalue);
	
	return retval;
	}

gint 
db_sync_test(gint testnum)
	{
	gchar *retstr;
	DbObject *obj;
	gint retval=0, pass=0;
	
	obj = db_sql(NULL,"SELECT * FROM soldier,country WHERE soldier.countryid=country.id order by soldier.id");
	switch (testnum)
		{
	case 0:
		db_moveto(obj,0);
		db_sync_row(obj, "country", "soldier","countryid", "2"); 
		db_setvalue(obj,"countryid","soldier","2");
		/* check results */
		db_getvalue(obj,"name","country", &retstr);
		test_result(retstr, "'Australia'", &pass);
		db_getvalue(obj,"continent","country", &retstr);
		test_result(retstr, "'Pacific'", &pass);
		/* back to normal */
		db_setvalue(obj,"countryid","soldier","1"); 
		test_total(pass, 2);
		break;
	default:
		warningmsg("test not supported");
		break;
		}
	g_assert(NULL);
	/*db_freeobject(obj); */
	return retval;
	}
