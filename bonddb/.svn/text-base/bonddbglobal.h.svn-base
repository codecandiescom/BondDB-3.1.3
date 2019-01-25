#ifndef __bonddbglobal_h
#define __bonddbglobal_h

#include "bonddbplugin.h"

typedef struct _BonddbGlobal BonddbGlobal;

struct _BonddbGlobal
	{
	/* Plugin functions dynmically loaded */
	BonddbPlugin *l;
	/* Database connection pointer */
	void *conn;
	/* Database connection provider */
	gchar *provider;
	/* Database connection pointer for remote write, 
	 * =*conn if disabled*/
	void *conn_remote_write;
	/* Database administrative connection */
	void *conn_admin;
	/* Database administrative connection - remote*/
	void *conn_remote_write_admin;
	/* orignal connection string used in config */
	gchar *connectionstr;
	/* orignal connection string used in config for remote write*/
	gchar *connectionstr_remote_write;
	/* original connection string used in config for admin */
	gchar *connectionstr_admin;
	/* tmp id counter for bonddb_id_issue_tmp() */
	gint tmpid;
	/* List of BonddbClass* still allocated */
	GList *alloclist;
	/** List of one off database connections */
	GHashTable *oo_conns;
	};

/** Wrap _BonddbNotify structure */
typedef struct _BonddbNotify BonddbNotify;

/** 
 * Store information about a notification event.  This is only currently
 * supported by the Postgresql backend.
 */
struct _BonddbNotify {
	gchar *name; /**< Name of the notification event */
	gboolean local; /**< Whether the nofication was triggered by this client or another */
};

/** Wrap _BonddbConnection structure */
typedef struct _BonddbConnection BonddbConnection;

/**
 * Store a db conn and references to it; only drop once references hit zero.
 */
struct _BonddbConnection {
	/** Connection pointer */
	void *conn;
	/** Number of times referenced */
	gint references;
	/** Global bonddb object; needed so that the db disconnect function can be called if this gets unreferenced */
	BonddbGlobal *bonddb;
};

BonddbNotify* bonddb_notify_check(BonddbGlobal *glob);
int bonddb_notify_listen(BonddbGlobal *bonddb,gchar* notify);
void bonddb_notify_free(BonddbNotify *notify);

gchar *bonddb_get_user(BonddbGlobal *glob);
gboolean bonddb_db_conn_has_role(BonddbGlobal *glob, gchar *role);
gboolean bonddb_user_has_role(BonddbGlobal *glob, gchar *username, gchar *role);

BonddbConnection *bonddb_connection_new(BonddbGlobal *bonddb, gchar *connectionstr, gchar *provider, GList **list);
void bonddb_connection_ref(BonddbConnection *bcon);
gboolean bonddb_connection_unref(BonddbConnection *bcon);
	
#endif
