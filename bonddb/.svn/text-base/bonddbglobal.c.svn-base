#include "bonddbglobal.h"
#include "../bondcommon/bc.h"

BonddbNotify* bonddb_notify_check(BonddbGlobal *glob)
	{	
	if (glob->conn_remote_write)
		return (BonddbNotify*) glob->l->_db_check_notify(glob->conn_remote_write);
	else
		return (BonddbNotify*) glob->l->_db_check_notify(glob->conn);
	}
	
/* starts listen for a notice, use bonddb_notify_check to get updates */
int bonddb_notify_listen(BonddbGlobal *glob,gchar* notify)
	{	
		gchar* sql=mem_strdup_printf("LISTEN \"%s\";",notify),*errmsg;
		void *conn = glob->conn;
		void *res = glob->l->_db_exec(conn,sql);
		int status;
		status=glob->l->_db_checkerror(glob->l, conn,res,&errmsg);
		if(status) errormsg("got error:%s",errmsg);
		mem_free(errmsg);
		glob->l->_db_clear(res);
		if(glob->conn_remote_write && conn != glob->conn_remote_write)
			{
			errmsg=NULL;
			conn = glob->conn_remote_write;
			res = glob->l->_db_exec(conn,sql);
			status=glob->l->_db_checkerror(glob->l, conn,res,&errmsg);
			if(status) errormsg("got error:%s",errmsg);
			mem_free(errmsg);
			glob->l->_db_clear(res);
			}
		mem_free(sql);
		return status;
	}

gchar *bonddb_get_user(BonddbGlobal *glob)
	{
	return glob->l->_db_get_user(glob->conn);
	}

gboolean bonddb_db_conn_has_role(BonddbGlobal *glob, gchar *role)
	{
	if(!glob->conn_admin)
		return (gboolean) glob->l->_db_conn_has_role(glob->l, glob->conn, glob->conn, role);
	return (gboolean) glob->l->_db_conn_has_role(glob->l, glob->conn_admin, glob->conn, role);
	}
	
gboolean bonddb_user_has_role(BonddbGlobal *glob, gchar *username, gchar *role)
	{
	if(!glob->conn_admin)
		return (gboolean) glob->l->_db_user_has_role(glob->l, glob->conn, username, role);
	return (gboolean) glob->l->_db_user_has_role(glob->l, glob->conn_admin, username, role);
	}

/** bonddb_notify_free()
 * @param notify BonddbNotify object to free
 *
 * Frees memory associated with a ::BonddbNotify object.
 */
void bonddb_notify_free(BonddbNotify *notify)
	{
	mem_free(notify->name);
	mem_free(notify);
	}

/**
 * bonddb_connection_new()
 * @param bonddb : global bond object
 * @param connectionstr : connection information
 * @param provider : provider information
 *
 * Create a new connection to the database, for use as a one off connection.
 *
 * @return Allocated BonddbConnection or NULL
 */
BonddbConnection *bonddb_connection_new(BonddbGlobal *bonddb, gchar *connectionstr, gchar *provider, GList **list)
	{
	gchar *errormsg = NULL;
	BonddbConnection *bcon;
	void *conn = bonddb->l->_db_connect_db(bonddb->l, connectionstr, provider, list);
	void *res;
	gchar *errmsg = NULL;
	/* Check for error; shouldn't happen, as the connection has already succeeded with
	 * these parameters */
	if (bonddb->l->_db_checkconnerror(conn, &errormsg))
		{
		errormsg("Connection to database failed with %s argument; aborting.\n" "%s", connectionstr, errormsg);
		return NULL;
		}
	bcon = mem_alloc(sizeof(BonddbConnection));
	bcon->references = 1;
	bcon->bonddb = bonddb;
	bcon->conn = conn;

	/* Start a transaction */
	res = bonddb->l->_db_exec(conn, "BEGIN");
	if(bonddb->l->_db_checkerror(bonddb->l, conn, res, &errmsg) != 0)
		{
		errormsg("SQL Failed with %s\n",errmsg); 
		bonddb->l->_db_clear(res);
		bonddb_connection_unref(bcon);
		return NULL;
		}
	return bcon;
	}

/**
 * bonddb_connection_ref()
 * @param bcon : 
 *
 * Increase the reference count for the given connection.
 */
void bonddb_connection_ref(BonddbConnection *bcon)
	{
	g_assert(bcon);
	bcon->references++;
	}

/**
 * bonddb_connection_unref()
 * @param bcon :
 *
 * Decrease the reference count for the given connection, disconnecting
 * and freeing the memory if the references drops to zero.
 *
 * @return TRUE if the object has been freed, otherwise FALSE
 */
gboolean bonddb_connection_unref(BonddbConnection *bcon)
	{
	g_assert(bcon);
	bcon->references--;
	if(bcon->references == 0)
		{
		/* Abort the transaction */
		void *res = bcon->bonddb->l->_db_exec(bcon->conn, "ABORT");
		bcon->bonddb->l->_db_clear(res);
		bcon->bonddb->l->_db_disconnect_db(bcon->bonddb->l, bcon->conn);
		mem_free(bcon);
		return TRUE;
		}
	return FALSE;
	}
