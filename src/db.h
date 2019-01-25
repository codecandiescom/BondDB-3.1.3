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

#ifndef db_h
#define db_h

/**
 * db.h is the master api file for bonddb, it should be included in your code and it
 * in turn will include everything else you generally need. 
 */

/**
 * =================================================================================
 * db.h - part of bonddb
 * 
 * This library was developed by treshna Enterprises Ltd.
 */

/* Base Includes */
#include "dbobject.h"
#include "dbheader.h"

/* Everything else you might need */
#include "dbgather.h"
#include "dbmapme.h"
#include "dbobjectdel.h"
#include "dbobjectvalid.h"
#include "dbwrapper.h"
#include "dbclient.h"
#include "dblooksee.h"
#include "dbobjectnav.h"
#include "dbdefault.h"
#include "dbgather.h"
#include "dbconstraint.h"
#include "dbuniqueid.h"
#include "dbtoliet.h"
#include "dbfilter.h"
#include "dbcache.h"
#include "dbfield.h"
#include "dbmethod.h"
#include "dbmethodmodule.h"

#include "bc.h"

/* ok the man guts of this, this is a wrapper around some native db get field
   code for whatever */
gint db_getvalue(DbObject * obj, gchar * field, gchar * table, gchar ** retval);
gint db_setvalue(DbObject * obj, gchar * field, gchar * table, gchar * value);
gint db_setvalue_nomark(DbObject * obj, gchar * field, gchar * table, gchar * value);
/* creation and deletion of database objects */
DbObject *db_createobject(gchar * tablename);
void db_freeobject(DbObject * obj);
/* Really important function */
DbObject *db_sql(DbObject * obj, gchar * sql);
/* getting desired information functions */
gint db_loadall(DbObject * obj);
gint db_getobjectbyfield(DbObject * obj, gchar * field, gchar * value);
gint db_setrowbyfield(DbObject * obj, gchar * fieldname, gchar *tablename, gchar * value);
/* delete current row, and a few other goodies */
gint db_deleteobject(DbObject * obj);
gint db_clearobject(DbObject *obj);
gint db_refreshobject(DbObject * obj);
/* Print all the values in the object for a query */
void db_debugobject(DbObject * obj);
/* DbObject by object stuff */
gint db_loadobjectbyobjects(DbObject * obj, GList * srcobjs);
gint db_loadobjectbyobject(DbObject * obj, DbObject * destobj, gchar * desttable);
gint db_addobjecttoobject(DbObject * destobj, DbObject * objlocal);
/* getting the unique id */
DbUniqueId *db_getobjectid(DbObject * obj);
DbObject *db_getobjectbyreference(DbObject *obj, gchar *tablename);
/* number of rows in an object */
gint db_numrows(DbObject *obj);
/* adding stuff to the object */
gint db_add(DbObject * obj);
gint db_insert(DbObject * obj);
DbObject* db_createdeadobject(gchar *tablename);
void db_setready(DbObject *obj, gboolean state);
gint db_wakemastersoftables(Object *obj);
GList * db_fieldschanged(DbObject *obj);
gint db_save(DbObject * obj);
gchar* db_getlasterror();
void db_clearlasterror();

#endif
