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

#ifndef dbuniqueid_h
#define dbuniqueid_h

#include "dbheader.h"
#include "dbwrapper.h"

/* Each individual dbunique id */
struct DbUniqueId
	{
	/* postgresql identifier */
	gint pg_oid;
	/* primary keys, for non-postgresql databases */
	gchar **primarykeys;
	DbField **field;
	gint num;
	};

/* Note: UniqueId's will be negitive if its a new record. */

/* free up stuff */
gint db_id_freeall(Object * obj);
gint db_id_free(DbUniqueId * id);

/* creating the array of unique ids (DbObjectIdIndex) */
gint db_id_createall(Object * obj);

/* get the unique id from database */
DbUniqueId *db_id_remeber(Object * obj);
DbUniqueId *db_id_remeberbyrow(Object * obj, gint row);

/* get the unique id for a record */
DbUniqueId *db_id_get(Object * obj);
DbUniqueId *db_id_getbyrow(Object * obj, gint row);

/* move to a record based on the unique id */
gint db_id_moveto(Object * obj, DbUniqueId * id);

/* compares to id's */
gint db_id_compare(DbUniqueId * id1, DbUniqueId * id2);

/* create sql component. */
gchar *db_id_createsql(Object * obj, DbUniqueId * id);
gchar *db_id_verifysql(gchar * sql);

/* for doing that low level stuff dealing with uniqueid */
gint db_id_adduniqueid(Object * obj, DbUniqueId * id, gint row);
gint db_id_extractid(Object * obj, DbUniqueId * id, gint row);

/* a bit of validation code for you */
gboolean db_id_isnewrecord(DbUniqueId * id);
gboolean db_id_isvalid(DbUniqueId * id);

/* and a bit of object management in regard of uniqueid's */
gint db_id_syncid(Object * obj);
gint db_id_updateidindex(Object * obj, DbUniqueId * id, gint row);
gint db_id_updatecache(Object * obj);

/* make something empty */
DbUniqueId *db_uniqueid(DbConnection * conn, DbRecordSet * result);
DbUniqueId *db_id_generateidfromfield(DbField * field, GList * oidlist);

/* get a key for hashtables. a unique way to identify object rows */
gchar *db_id_getuniqueid_asstring(Object * obj, gchar * tablename, gchar **fieldname);
gchar *db_id_getuniqueid_fast(Object *obj, gchar *tablename, gint row, gchar **fieldname, gint *needsfreeing);

#endif
