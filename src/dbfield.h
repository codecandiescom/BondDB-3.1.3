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

#ifndef dbfield_h
#define dbfield_h

#include "dbheader.h"
#include "dbgather.h"
#include "dbpath.h"
#include "dbwrapper.h"

/* Ok this is so hard to generate from a straight sql statement .hmmmmm */
struct DbField
	{
	gchar *field;					/* name of the field */
	gchar *table;					/* table it is stored in */
	DbFieldDef *fielddef;
	DbTableDef *tabledef;
	void *birthvalue;				/* A default value attached to anything created */

	/* Path associated with this field */
	DbPath *path;
	/* where abouts in the sql return statement this field is positioned */
	gint8 fieldposinsql;

	gboolean isuniqueid;
	gboolean basetable;
	gboolean readonly;
	/* Master of table is if this field controls the values in every field in of 
	 * that same table. I.e. in select * from b,a where b.d = a.c b.d is master */
	gboolean masteroftable;
	gchar *tablemasterof;
	gchar *fieldmasterof;
	};

gint db_field_getpos(Object * obj, gchar * fieldname, gchar * tablename);
gint db_field_read(Object * obj, gchar * fieldname, gchar * tablename, char **retval);
gint db_field_get(Object * obj, gint fieldpos, gchar ** fieldstr, gchar ** tablestr);
gint db_field_arraypos(DbField ** field, gint num, gchar * fieldname);

DbField *db_field_getbytable(gchar * tablename, gint pos);
gint db_field_numfieldbytable(gchar * tablename);
GList *db_field_allfieldsinresult(Object * obj);

/* heres an important function.. */
gint db_field_populate(Object * obj, GList * fieldtablelist, gchar * basetable);
GList *db_field_populate_element(gchar * inputfield, gchar * deftable);
DbField *db_field_create(DbFieldDef * fielddef, DbTableDef * tabledef);
gint db_field_populate_singletable(Object * obj);

gint db_field_generatefield(gchar * sqlquery, DbField *** retfield, gint * num);
void db_field_free(DbField *);
void db_field_freeall(Object * obj);
gint db_field_test(Object * obj);
DbField *db_field_dbfind(Object * obj, gchar * fieldname, gchar * tablename);
gchar * db_field_gettable(Object *obj, gchar *fieldname);
DbField *db_field_getbyfield(gchar * fieldname, gchar * tablename);

void db_field_debug(Object *obj);

#endif
