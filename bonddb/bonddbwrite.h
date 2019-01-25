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

#ifndef bonddbwrite_h
#define bonddbwrite_h

#include "bonddbobj.h"
#include "bonddbid.h"

typedef enum {
	BONDDB_WRITE_UPDATE,
	BONDDB_WRITE_INSERT,
	BONDDB_WRITE_DELETE
} BonddbWriteType;

typedef struct _BonddbWrite BonddbWrite;

struct _BonddbWrite
	{
	BonddbClass *parent;
	
	gint row;		/* row to save back at */
	BonddbWriteType type;
	gchar *tablename;
	gchar *fieldname;
	gchar *value;
	gchar *hashkey;
	BonddbId *id; 		/* Id of record to save by */

	/* general option flags */
	gboolean mark;		/* If to save on change, mark false is 
				 * for defaults */
	gboolean recusive;	/* for recusive deletes */
	gboolean dontfree;	/* if operation aborts keep this in write cache */
	gboolean freeid; 	/* if id is generated */
	};

void bonddb_write_freelist(BonddbClass *obj);
void bonddb_write_add(BonddbClass *obj, gchar *tablename, gchar *fieldname,
		gchar *value, gboolean mark);
void bonddb_write_delete(BonddbClass *obj, gchar *tablename, 
		gboolean recusive);
gint bonddb_write_all(BonddbClass *obj, gchar **sqlerror);

#endif

