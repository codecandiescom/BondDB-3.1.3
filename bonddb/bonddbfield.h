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

#ifndef bonddbfield_h
#define bonddbfield_h

typedef struct _BonddbField BonddbField;

#include "bonddbobj.h"

/* Ok this is so hard to generate from a straight sql statement .hmmmmm */
struct _BonddbField
	{
	BonddbClass *parent;

	gchar *tablename;			/* table it is stored in */
	gchar *fieldname;			/* name of the field */
	
	/* where abouts in the sql return statement this field is positioned */
	guint16 fieldposinsql;
	
	void *birthvalue;		/* A default value attached to 
					 * anything created */
	gboolean isuniqueid;
	gboolean readonly;
	/* Master of table is if this field controls the values in every 
	 * field in of that same table. 
	 * Ie in select * from b,a where b.d = a.c b.d is master */
	gboolean masteroftable;
	gchar *tablemasterof;
	gchar *fieldmasterof;
	};

#include "bonddbobj.h"

gint bonddb_field_populatebyresult(BonddbClass *obj);
gint bonddb_field_populatebytablename(BonddbClass *obj, gchar *tablename,
		gint startfieldpos);
void bonddb_strfree(void *str);
gint bonddb_field_setfirsttable(BonddbClass *obj);

BonddbField * bonddb_field_get(BonddbClass *obj, gchar *tablename, 
		gchar *fieldname);
gint bonddb_field_default(BonddbClass *obj, gchar *tablename);
gint bonddb_field_debug(BonddbClass *obj);
gint bonddb_field_get_name_by_index(BonddbClass * obj, gint fieldnumber, gchar** fieldname,gchar **tablename);
void bonddb_field_addtolist(void *key, void *value, void *ptr);

#endif
