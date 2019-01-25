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

#ifndef dbobject_h
#define dbobject_h

#include "dbheader.h"
#include "dbwrapper.h"
#include "sql_parser.h"

/* this is included as part of the db objects */

/* object is a datasource pointing to a row in a table in the back end */
struct _DbObject
	{
	/* A unique number specifying how this was created. umdedodoaa */
	gint objectinstance;
	/* Basetable, if has one. This is what table to save information back to the db */
	gchar *basetable;
	/* query used to generate the data, for refreshing when needed */
	gchar *query;
	/* Description of the query and what it composes of. This is an array to match the PGresult of explaining exactly
	   where this value was obtained from */
	DbField **field;
	gint numfield;
	/* write ahead cache. */
	DbCache **cache;
	gint numcache;
	/* Cache position corresponding to your row */
	DbCache *currentcache;
	/* Hash table. No this is not drugs */
	/* actually i lie. its not here. NAH NA */
	/* Your current unique id for selected record */
	DbUniqueId *id;
	/* currently selected row in the data selection */
	gint row;
	/* list information, on how many rows there are */
	gint num;
	/* PQresult returned from inital query to specify this datasource.  */
	DbRecordSet *res;
	/* if set to true then free res on closure, else dont touch */
	gboolean freeresult;
	/* age of record */
	gboolean newrecord;
	gboolean changed;
	gboolean unknownid;
	gboolean readonly;
	gboolean dead;
	/* DEPRECIATED */
	gboolean mapoutofsync;
	/* filtering information */
	gboolean filtered;
	DbFilterApplied *filterapplied;
	/* How much of bonddb library can i use without breaking? */
	gboolean fulldbsupport;
	/* if statement was successfully created. */
	gboolean sqlgood;
	/* if the object is ready to execute, its kinda like pausing it. */
	gboolean sqlready;
	/* A log of fields that have been affected by change. (masteroftable action) */
	GList *fieldschanged;
	/* reference information to other objects */
	/* DEPRECIATED */
	GList *mapobject;
	/* DbBirth, what defaults to set when created. NEED create by method thingy */
	DbBirth *birth;
	/* Sql Statement after been passed */
	sql_statement *statement;
        /* This is a fast lookup for finding the primary key's from hashs. */
	GHashTable *fieldhashbytable;
	};

gint db_obj_addwrite(DbObject * obj, gchar * field, gchar * table, gchar * value, gboolean mark);
gint db_obj_doread(DbObject * obj, gchar * field, gchar * table, gchar ** value);
gint db_obj_sqlwrite(gchar * query, gchar * targettable, DbUniqueId ** id);
gint db_obj_sqlread(DbObject * obj, gchar * query);
gint db_obj_test(DbObject * obj);
DbObject *db_obj_create(gchar * name);
gint db_obj_setcacheandid(DbObject * obj);
gint db_obj_free(DbObject * obj);
gint db_obj_clear(DbObject * obj);
gint db_obj_applydefaults(DbObject * obj);
gint db_obj_handle_empty_recordset(DbObject * obj);
gint db_obj_handle_new_recordset(DbObject * obj);
gint db_obj_dodelete(DbObject * obj);
gint db_obj_debug(DbObject * obj);
gint db_obj_refresh(DbObject * obj);
DbObject *db_obj_loadself(DbObject * obj);
gint db_obj_compareid(DbObject * a, DbObject * b, gchar * field, gchar * table, gboolean compareall, gint * row);
gint db_obj_addloadby(DbObject * destobj, DbObject * srcobj, sql_statement *statement, gchar **ret_ltable, gchar **ret_lfield, gchar **ret_value);
void db_obj_setdead(DbObject * obj, gboolean state);

#endif
