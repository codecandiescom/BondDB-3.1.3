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

#ifndef bonddbid_h
#define bonddbid_h

#include <glib.h>

typedef struct _BonddbId BonddbId;

/* Each individual unique id */
struct _BonddbId
	{
	/* postgresql identifier */
	gint32 pg_oid;
	gchar *tablename;
	GList *fieldlist;
	gchar **primarykeys;
	gint num;
	};

#include "bonddbobj.h"

/* free up stuff */
void bonddb_id_free(BonddbId * id);
void bonddb_id_free_copy(BonddbId * id);

BonddbId *bonddb_id_get_currentrow(BonddbClass *obj, gchar *tablename);
BonddbId *bonddb_id_get(BonddbClass *obj, gchar *tablename, gint row);
gchar *bonddb_id_get_str(BonddbClass *obj, BonddbId *id, gchar *tablename);
BonddbId * bonddb_id_from_oid(BonddbClass *obj, gchar *tablename, gint row,
				guint32 oid);
gint bonddb_id_release_tmp(BonddbClass *obj);
BonddbId * bonddb_id_issue_tmp(BonddbClass *obj, gchar *tablename);
gint bonddb_id_get_row_from_id(BonddbClass *obj, BonddbId *id, 
				gchar *tablename);
BonddbId * bonddb_id_copy(BonddbId *srcid);
gboolean bonddb_id_is_equal(BonddbId *a, BonddbId *b);
#endif
