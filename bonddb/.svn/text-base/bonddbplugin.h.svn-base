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
#ifndef bonddbplugin_h
#define bonddbplugin_h

#include <glib.h>

typedef struct _BonddbPlugin BonddbPlugin;

struct _BonddbPlugin
{
	gchar *libraryname;
        gchar *type;
	/* dynmically linked symbol */
        void *handle;

	/* Some functions */
        /* an array of functions */
	
	char *(*_db_type) ();
	void *(*_db_exec) (void *connection, char *query);
	int (*_db_checkconnerror) (void *connection, char **errmsg);
	int (*_db_checkerror) (void *l, void *connection, void *res, char **errmsg);
	int (*_db_tuples_ok) (void *connection, void *res);
	int (*_db_numtuples) (void *res);
	int (*_db_reset) (void *res);
	int (*_db_numcolumn) (void *res);
	char *(*_db_fieldname) (void *res, int pos);
	int (*_db_tablename) (void *l, void *conn, void *res, int pos, 
	char **retstr);
	int (*_db_fieldlist) (void *l, void *conn, char *tablename, GList **);
	int (*_db_primarykeylist) (void *l, void *conn, char *tablename, 
		GList **retlist);
	char *(*_db_defaultvalue) (void *l, void *conn, void *conn_remote_write, 
			char *tablename, char *fieldname);
	void (*_db_freefieldlist) (void *list);
	GList *(*_db_findreference) (void *l, void *conn, gchar *tablename);
	GList *(*_db_findgroups) (void *l, void *conn, gchar *username);
	/* a spacer for future additions */
	char spacer[16];
	void (*_db_freereference) (GList *);
	char *(*_db_get_value) (void *res, int row, int field);
	guint32 (*_db_uniqueid) (void *conn, void * result);		
	void *(*_db_connect_db) (void *l, char *connectstr, char *provider, GList **list);
	int (*_db_disconnect_db) (void *l, void *connection);
	int (*_db_clear) (void *res);
	int (*_db_cleanup) (void *l);
	int (*_db_init) (void *l);
	gchar *(*_db_check_notify) (void *conn);
	gchar *(*_db_get_user) (void *conn);
	gchar *(*_db_conn_has_role) (void *l, void *conn_admin, void *conn, gchar *role);
	gchar *(*_db_user_has_role) (void *l, void *conn_admin, gchar *username, gchar *role);
	/* Cached information */
	gboolean cache_dictonary;
	GHashTable *tablehash;
	GHashTable *fieldlisthash;
	GHashTable *primarykeyhash;
	GHashTable *defaulthash;
	GHashTable *referencehash;
	GList **notices;
};

BonddbPlugin *bonddb_plugin_init(gchar *dbsuffix);
void bonddb_plugin_free(BonddbPlugin *plugin);
void bonddb_plugin_setcache(BonddbPlugin *l, gboolean state);
	
#endif
