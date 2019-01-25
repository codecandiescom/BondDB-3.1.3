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

#ifndef dbgather_h
#define dbgather_h

/* dbgather, by andru.
 *
 * gather information from a postgresql database.  Wrapper to postgresql.
 * I would have used someone elses but i suffered from misgiving. while
 * gnome-db tempting i was kinda demanding speed in the speed of essense,
 * hmmm maybe i should support both. tricky.
 */

#include "dbwrapper.h"
#include "dbheader.h"

#define DBTEXT 1
#define DBCHAR 2
#define DBVARCHAR 3
#define DBBOOL 4
#define DBDATE 5
#define DBTIME 6
#define DBDATETIME 7
#define DBFLOAT 8
#define DBINT 9
#define DBPGOID 10

typedef struct DbTableDef DbTableDef;

typedef struct
	{
	gchar *name;
	gchar *typename;
	gint datatype;
	gint tableoid;
	/* Forign reference value, 0 is normal 1 is reference to other table, no view present 2 is reference to other
	   table, view present by same name describing relation. 3 no field ref, dummy list value 4 direct reference to a
	   view, no cref record present */
	enum
	{
	    DBNORMAL,
	    DBREF,
	    DBREFVIEW,
	    DBNOREF,
	    DBVIEW
	}
	fieldtype;
	/* default value that will appear when this record is created */
	gchar *defaultvalue;			  /* what is stored in posgresql for default value */
	gint defaultfunc;				  /* if it defaults from a function. */
	gchar *tmpvalue;				  /* if default from function this is last value. PS dru not know what this is */
	/* comment/s associated with this widget */
	GList *comment;
	gboolean notnull;				  /* set to true if not-null is set on field */
	gboolean fake;					  /* if a oid field later added on for easier referencing. */
	gboolean primary;	/* If this is a primary key */
	gboolean unique;
	DbMethodDef *method;			  /* Pointer to the comment callback in DbDatabaseDef->methods */
	DbTableDef *tabledef;
	}
DbFieldDef;

struct DbTableDef
	{
	gchar *name;
	gchar *query;
	gint type;
	gint oid;

	gchar *uidfield;
	
	DbFieldDef **field;
	gint num;

	/* references to other tables */
	GList *cref;

	/* list of tables used in query if this is view */
	GList *fromtable;
	/* list of views that make references to this table */
	GList *subtable;
	};

typedef struct
	{
	gchar *name;
	gchar *connstr;
	
	DbTableDef **table;
	gint numtable;

	DbTableDef **view;
	gint numview;

	GList *constraints;

	/* Francis: Here goes new callback lists. */
	GList *commentcallbacks;	  /* List of type DbCommentCallbackDef */
	GList *methods;				  /* List of type DbMethodDef */
	}
DbDatabaseDef;

extern DbDatabaseDef *globaldb;

/* ============================================================================
 * external code
 * ============================================================================*/

/* top level function */
DbDatabaseDef *db_builddef();

GList *db_getalltablenames();
GList *db_getallviewnames();
DbTableDef *db_findtable(gchar * table);
DbFieldDef *db_findfield(DbTableDef * dt, gchar * field);
gint db_getfieldposintable(DbTableDef * table, gchar * field);
gint db_generalisetype(DbDatabaseDef * db);
gint db_viewforthis(gchar * viewname);
gint db_showallfields();
gint db_checkpgresult(DbRecordSet * result);
gboolean db_checkfieldtableexist(gchar *table, gchar *field);

/* ============================================================================
 * internal code 
 * ============================================================================*/

gint db_buildrelationdef(DbDatabaseDef * db);
gint db_buildfieldinfo(DbDatabaseDef * db);
void db_freefielddef(DbFieldDef * fielddef);
void db_freetabledef(DbTableDef * db);
gint db_freedatabasedef(DbDatabaseDef * db);
/* Reference code for working out how variables relate to each other */
gint db_buildsubtabledef(DbDatabaseDef * db, DbTableDef * table);
gint db_extracttablesinfields(DbDatabaseDef * db, DbTableDef * table);
/* support for above function */
gint db_tablesourceforfield(DbDatabaseDef * db, gchar * fieldname, gint parenttableoid, gchar * parentquery);
/* just fixing stuff */
gint db_isview(DbDatabaseDef * db, gint tableoid, gchar ** retstr);
/* treshna Enterprises was here! */
DbTableDef *db_findtable(gchar * table);
DbFieldDef *db_findfield(DbTableDef * dt, gchar * field);
gint db_default_isitafunction(gchar * value1, gchar * value2);

DbDatabaseDef *db_get_current_database();

#endif
