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

#ifndef bonddbobj_h
#define bonddbobj_h

#include <glib.h>

#include "bonddbglobal.h"

typedef struct _BonddbClass BonddbClass;

typedef enum {
	BONDDB_INVALID,		/* Nothing useable */
	BONDDB_NEW,		/* No record set, empty object with table */
	BONDDB_RAW,		/* Unparsed SQL statement without a result */
	BONDDB_READ_RAW,	/* Unparsed SQL statement with a result */
	BONDDB_READ_ONLY,	/* SQL statement, no parsing possible */
	BONDDB_READ_WRITE	/* SQL with reads and writes avaliable */
} BonddbClassState;

typedef enum {
	BONDDB_READ,		/* Object is on a record row */
	BONDDB_UPDATE,		/* Object is updating a row */
	BONDDB_INSERT,		/* Object is on a new record */
	BONDDB_INSERT_SAVED	/* Object is on a new record which is saved */
} BonddbClassMode;

#include "bonddbid.h"
#include "bonddbfield.h"
#include "bonddbrecord.h"
#include "bonddbid.h"
#include "bonddbfilter.h"
#include "sql_parser.h"

/* object is a datasource pointing to a row in a table in the back end */
struct _BonddbClass
{
	/* global pointer stuff */
	BonddbGlobal *bonddb;
	/* Optional identifying stuff */
	gint objectinstance;
	gchar *name;
	
	/* For record navigation */
	gint currentrow;
	gint numrecord;
	gint numcolumn;
	BonddbId *currentid;
	BonddbId *tmpid;
	
	/* Object state */
	BonddbClassState state;
	BonddbClassMode mode;
	
	GHashTable *fieldhash;
	/* Record information */
	BonddbRecord *currentrecord;
	BonddbRecord **record;
	/* Primary keys for tables which are known */
	GHashTable *primarykey;
	GHashTable *idlookup;

	/* write queue for stuff to be saved */
	GList *writequeue;
	GHashTable *writequeuehash;
	gboolean autosave;

	/* boolean control switchs */
	/* this ignores asserts and checks */
	gboolean ignoreassert;
	gboolean noprimarykey;
	
	/* Relationship information */
	GList *reflist;
	GHashTable *birthhash;

	/* Filtering */
	GHashTable *filterhash;
	GList *filterrequest;
	
	/* Sql Statement after been passed */
	sql_statement *statement;
	gchar *query;
	gchar *firsttable;
	GList *tablelist;
	/* Record set returned by native db driver */
	gboolean sqlgood;
	/* Generate query is TRUE if you need to generate the SQL 
	 * statement using the sql_statement * instead of using the
	 * existing query. This is used for loadby's and modifications
	 * of query before ready for final use. */
	gboolean generatequery;
	gboolean editable;
	gboolean admin;
	void *conn ; /* originating database connection */
	/** If using a one off connection, the name of the connection used */
	gchar *conn_name;
	void *res;
};

void bonddb_setstate(BonddbClass *obj, BonddbClassState state);
BonddbClassState bonddb_getstate(BonddbClass *obj);

#define bonddb_new(a) bonddb_new_full(a, NULL)
#define bonddb_new_sql(a,b,c,d) bonddb_new_sql_full(a,b,c,d,NULL)
#define bonddb_new_sql_withadmin(a,b,c,d,e) bonddb_new_sql_withadmin_full(a,b,c,d,e,NULL)
#define bonddb_new_basetable(a,b,c) bonddb_new_basetable_full(a,b,c,NULL)

BonddbClass *bonddb_new_full(BonddbGlobal *bonddb, gchar *conn_name);
BonddbClass * bonddb_new_sql_full(BonddbGlobal *bonddb, gchar *sql, gboolean run, gchar **errmsg, gchar *conn_name);
BonddbClass * bonddb_new_sql_withadmin_full(BonddbGlobal *bonddb, gchar *sql, gboolean run, gboolean admin, char **errmsg, gchar *conn_name);
BonddbClass * bonddb_new_basetable_full(BonddbGlobal *bonddb,  gchar *basetable, gchar **errmsg, gchar *conn_name);
void bonddb_setname(BonddbClass *obj, gchar *name);
void bonddb_setfirsttable(BonddbClass *obj, gchar *firsttable);
gint bonddb_sql(BonddbClass *obj, gchar *sql, gboolean runnow, gchar **errmsg);
gint bonddb_sql_runnow(BonddbClass *obj, gchar **errmsg);
gint bonddb_sql_runnow_write(BonddbClass *obj, gchar **errmsg);
gint bonddb_sql_exec(BonddbGlobal *bonddb, gchar *sql, gboolean admin, gchar **errmsg);
gint bonddb_sql_exec_write(BonddbGlobal *bonddb, gchar *sql, gboolean admin, gchar **errmsg);
gint bonddb_parsesql(BonddbClass *obj);
gint bonddb_parsesql_proper(BonddbClass *obj, gboolean addoid);
gint bonddb_jumpto(BonddbClass *jumpfrom, BonddbClass *jumpto);
gint bonddb_jumpto_id(gchar *idval, BonddbClass *jumpto);

gint bonddb_setvalue(BonddbClass *obj, gchar *table, gchar *field, 
		gchar *value, gboolean mark);
gint bonddb_getvalue(BonddbClass *obj, gchar *table, gchar *field, 
		gchar **value);
gint bonddb_getvaluebyindex(BonddbClass *obj, gint index,gchar **value);

gint bonddb_setprimarykey(BonddbClass *obj, gchar *tablename, 
		GList *primarykey);

gint bonddb_save(BonddbClass *obj, gchar **errormsg);
gint bonddb_forcemark(BonddbClass *obj);
gint bonddb_abortsave(BonddbClass *obj);
gint bonddb_setautosave(BonddbClass *obj, gboolean autosave);
gint bonddb_validate(BonddbClass *obj, GList **failures);
gint bonddb_add(BonddbClass *obj, gchar *tablename);
gint bonddb_delete(BonddbClass *obj, gchar *tablename, gboolean recusive);
gint bonddb_moveto(BonddbClass *obj, gint row);
gint bonddb_moveto_id(BonddbClass *obj, BonddbId *id);
gint bonddb_numrecord(BonddbClass *obj);
gint bonddb_currentrow(BonddbClass *obj);
gint bonddb_realrow(BonddbClass *obj);
gint bonddb_clear(BonddbClass *obj);
gint bonddb_free(BonddbClass *obj);
gint bonddb_loadall(BonddbClass *obj, gchar **errmsg);
gint bonddb_blank(BonddbClass *obj, gchar **errmsg);
gint bonddb_undo(BonddbClass *obj);
void bonddb_debug(BonddbClass *res);

gint bonddb_assert_moveto(BonddbClass *obj);
gint bonddb_assert_recordset(BonddbClass *obj);
gint bonddb_assert_position(BonddbClass *obj, gint row);
gboolean bonddb_isrecordset(BonddbClass *obj);
BonddbClassState bonddb_getstate(BonddbClass *obj);
BonddbClassMode bonddb_getmode(BonddbClass *obj);
gboolean bonddb_isnewrecord(BonddbClass *obj);
gboolean bonddb_geteditable(BonddbClass *obj);
void bonddb_seteditable(BonddbClass *obj, gboolean editable);
gint bonddb_settable(BonddbClass *obj, gchar *table);
GList *bonddb_fieldlist(BonddbClass *obj);

#endif
