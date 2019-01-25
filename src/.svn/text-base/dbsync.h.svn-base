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

/**
 * Function to build additional information of dependences.
 * Stored as seperate structure. Only intialised when it does a check on change
 *
 * Ie db_sync_record(obj,table,field); will call db_sync_init(obj) if it hasn't
 * been set.
 * db_sync_init() will do a select statement and place it in sync->dbobj.
 * Each DbObject will contain a list of syncs and what table they represent.
 *
 * 
 * For each record 
 * 
 */

#ifndef dbsync_h
#define dbsync_h

#include "dbheader.h"
 
typedef struct _DbSync DbSync;
 
struct _DbSync
	{
	/* Whats covered by this sync */
	gchar *tablename;
	gchar *fieldnamekey;
	gchar *currentkey;
	gchar *query;
	GList *fieldlist;
	/* if we have done this sync set */
	gboolean syncset;
	/* Dependency */
	DbField *dependon;
	/* A object of all possible values to populate the sync with. oh
	 * no the speed lag here */
	DbObject *dbobj;
 	};

void db_sync_free(DbObject *dbobj);
gint db_sync_init(DbObject *dbobj, gchar *tablename);
gint db_sync_row(DbObject *dbobj, gchar *synctablename, gchar *tablename, gchar *fieldname, gchar *newvalue);
gint db_sync_getvalues(DbObject *dbobj, gchar *synctablename, GList **retfield, GList **retvalue);

gint db_sync_test(gint testnum);

#endif
