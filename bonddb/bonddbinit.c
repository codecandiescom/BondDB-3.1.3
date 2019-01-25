#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bonddbinit.h"
#include "bonddbplugin.h"
#include "bonddbglobal.h"
#include "bonddb.h"

#include "bcconfig.h"
#include "bcdebug.h"

gchar bonddb_lasterrmsg[256];

/**
 *
 * Look for @searchforstr in @parentstr, return 0 if true.
 */
static gint
db_strcontain(gchar * parentstr, gchar * searchforstr)
	{
	gint i, pass = 0;

	if (searchforstr == NULL || parentstr == NULL)
		return 0;
	for (i = 0; i < strlen(parentstr); i++)
		{
		if (parentstr[i] == searchforstr[pass])
			pass++;
		else
			pass = 0;
		if (pass == strlen(searchforstr))
			return 0;
		}

	return 1;
	}

/**
 * bonddb_init_getconnstr()
 * 
 * Creates a database connection string that contains the database name,
 * the users name and the users password.
 * 
 * @return  the connection string
 */
static gchar *
bonddb_init_getconnstr(gboolean admin)
	{
	gchar *retstr, *tmpstr;
	gchar *host;
	gchar *username;
	gchar *password;
	gchar *port;
	gchar *db, *backend;

	retstr = bcconfig_getvalue("db_connection_string");
	if (retstr)
		return mem_strdup(retstr);
	db = bcconfig_getvalue("db_name");
	if(!admin)
		{
		username = bcconfig_getvalue("db_username");
		password = bcconfig_getvalue("db_password");
		}
	else
		{
		username = bcconfig_getvalue("db_username_admin");
		password = bcconfig_getvalue("db_password_admin");
		}

	port = bcconfig_getvalue("db_port");
	host = bcconfig_getvalue("db_host");
	backend = bcconfig_getvalue("db_backend");
	if (db == NULL)
		{
		error_output("No database name is specified to connect to\n");
		return NULL;
		}

	/* gda does things differently */
	if (backend && strcmp(backend, "gda") == 0)
		{
		errormsg("GDA BACKEND SUPPORT IS DEPRICATED - DO NOT USE");
		retstr = mem_strdup_printf("DATABASE=%s", db);
		if (username)
			{
			tmpstr = retstr;
			retstr = mem_strdup_printf("%s;USER=%s", retstr, username);
			mem_free(tmpstr);
			}
		if (password)
			{
			tmpstr = retstr;
			retstr = mem_strdup_printf("%s;PASSWORD=%s", retstr, password);
			mem_free(tmpstr);
			}
		if (host)
			{
			tmpstr = retstr;
			retstr = mem_strdup_printf("%s;HOST=%s", retstr, host);
			mem_free(tmpstr);
			}
		return retstr;
		}
	if (bcconfig_getvalue("db_no_timeout"))
		retstr = mem_strdup_printf("dbname=%s", db);
	else
		retstr = mem_strdup_printf("dbname=%s connect_timeout=5", db);
	
/* don't try administarator if no administrator fields in the config file */
	if( !username && !password && admin ) 
		{
			mem_free(retstr);
			return NULL; 
		}
		
	if (username)
		{
		tmpstr = retstr;
		retstr = mem_strdup_printf("%s user=%s", retstr, username);
		mem_free(tmpstr);
		}
	if (password)
		{
		tmpstr = retstr;
		retstr = mem_strdup_printf("%s password=%s", retstr, password);
		mem_free(tmpstr);
		}
	if (host)
		{
		tmpstr = retstr;
		retstr = mem_strdup_printf("%s host=%s", retstr, host);
		mem_free(tmpstr);
		}
	if (port)
		{
		tmpstr = retstr;
		retstr = mem_strdup_printf("%s port=%s", retstr, port);
		mem_free(tmpstr);
		}
	return retstr;
	}

static gchar *
bonddb_init_getconnstr_remote_write(gboolean admin)
	{
	gchar *retstr, *tmpstr;
	gchar *host;
	gchar *username;
	gchar *password;
	gchar *db;

	host = bcconfig_getvalue("db_host_remote_write");
	retstr = bcconfig_getvalue("db_connection_string_remote_write");
	if (host == NULL && retstr == NULL)
		{
		return NULL;
		}
	if (retstr)
		return mem_strdup(retstr);
	db = bcconfig_getvalue("db_name_remote_write");
	if(!admin)
		{
		username = bcconfig_getvalue("db_username_remote_write");
		password = bcconfig_getvalue("db_password_remote_write");
		}
	else
		{
		username = bcconfig_getvalue("db_username_remote_write_admin");
		password = bcconfig_getvalue("db_password_remote_write_admin");
		}

	if (db == NULL)
		{
		db = bcconfig_getvalue("db_name");
		if (db == NULL)
			{
			return NULL;
			}
		}
	if (username == NULL)
		{
		username = bcconfig_getvalue("db_username");
		}
	if (password == NULL)
		{
		password = bcconfig_getvalue("db_password");
		}
	retstr = mem_strdup_printf("dbname=%s", db);
	if (username)
		{
		tmpstr = retstr;
		retstr = mem_strdup_printf("%s user=%s", retstr, username);
		mem_free(tmpstr);
		}
	if (password)
		{
		tmpstr = retstr;
		retstr = mem_strdup_printf("%s password=%s", retstr, password);
		mem_free(tmpstr);
		}
	if (host)
		{
		tmpstr = retstr;
		retstr = mem_strdup_printf("%s host=%s", retstr, host);
		mem_free(tmpstr);
		}
	return retstr;
	}

/**
 * bonddb_init()
 * @param conninfo : standard postgresql database connection string or the name
 * of the application
 *
 * Establishes database connection to postgresql, and extracts all the 
 * information postgresql needs to work with the database.  If @conninfo 
 * is NULL then the bond.conf configuration files will 
 * be read to get the database initialising string.
 *
 * @param conninfo : can be either the name of the config file (ie appname.conf)
 * or a database connection string. It looks for the string dbname
 * in @conninfo and if present it assumes its a database connection
 * string else it assumes its not and is a filename.
 * 
 * @return  NULL on error else BonddbGlobal object.
 */
BonddbGlobal *
bonddb_init(gchar * conninfo, gchar * library, GList **notice_list)
	{
	BonddbGlobal *bonddb;
	gchar *c_remote_write = NULL;
	gchar *c_admin = NULL;
	gchar *c_remote_write_admin = NULL;
	gchar *c, *provider = NULL;
	gchar *errormsg;
	gchar *dbsuffix = NULL;
	gchar ldb[] = "pgsql";
	

	dbsuffix = ldb;

	/* debugmsg("passing in conninfo of %s", conninfo); */
	/* this is a tad complex and can cause a few bugs. If conninfo is a db
	   string then initialise it else do a read on a config file */
	if (conninfo == NULL || db_strcontain(conninfo, "dbname") != 0)
		c = bonddb_init_getconnstr(FALSE);
	else
		c = mem_strdup_printf("%s", conninfo);
	if (c == NULL)
		{
		if (conninfo != NULL)
			errormsg("Error creating connection string from %s. "
			         "Aborting", conninfo);
		else
			errormsg("NULL connection string, can not continue.");
		return NULL;
		}
	c_remote_write = bonddb_init_getconnstr_remote_write(FALSE);
	c_remote_write_admin = bonddb_init_getconnstr_remote_write(TRUE);
	c_admin = bonddb_init_getconnstr(TRUE);

	debug_output("Connecting to database: %s\n", c);
	dbsuffix = bcconfig_getvalue("db_backend");
	provider = bcconfig_getvalue("db_provider");

	bonddb = mem_calloc(sizeof(BonddbGlobal)*2);
	bonddb->connectionstr = c;
	bonddb->provider = provider;
	bonddb->connectionstr_remote_write = c_remote_write;
	bonddb->connectionstr_admin = c_admin;
	/* Load backend library for running sql commands though */
	if (dbsuffix == NULL)
		bonddb->l = bonddb_plugin_init("pgsql");
	else
		bonddb->l = bonddb_plugin_init(dbsuffix);

	if (bonddb->l == NULL)
		{
		mem_free(bonddb->connectionstr);
		mem_free(bonddb->provider);
		mem_free(bonddb->connectionstr_remote_write);
		mem_free(bonddb->connectionstr_admin);
		mem_free(bonddb);
		return NULL;
		}
	mem_verify(bonddb->l);
	bonddb->conn = bonddb->l->_db_connect_db(bonddb->l, c, provider, notice_list);
	/* dont you hate it when code you write goes missing? */
	if (bonddb->l->_db_checkconnerror(bonddb->conn, &errormsg))
		{
		errormsg("Connection to database failed with %s argument.\n"
		         "%s", c, errormsg);
		strncpy(bonddb_lasterrmsg, errormsg, 254);
		bonddb_cleanup(bonddb);
		return NULL;
		}
	if (bonddb->connectionstr_remote_write == NULL)
		{
		debug_output("Remote write not activate\n");
		bonddb->conn_remote_write = bonddb->conn;
		}
	else
		{
		bonddb->conn_remote_write =  bonddb->l->_db_connect_db(
			bonddb->l, bonddb->connectionstr_remote_write,
			provider, notice_list);
		if (bonddb->l->_db_checkconnerror(
			bonddb->conn_remote_write, &errormsg))
			{
			errormsg("Connection to remote write database failed with %s "
			         "argument.\n%s", bonddb->connectionstr_remote_write,
			         errormsg);
			strncpy(bonddb_lasterrmsg, errormsg, 254);
			bonddb_cleanup(bonddb);
			return NULL;
			}
		debug_output("Remote write activate\nConnecting to database: %s\n",
		             bonddb->connectionstr_remote_write);
		}
	if (bonddb->connectionstr_admin == NULL)
		{
		debug_output("Administrator Connection Disabled: no connection info\nTo correct, specify the db_username_admin and db_password_admin configuration items.");
		bonddb->conn_admin = bonddb->conn;
		}
	else
		{
		bonddb->conn_admin =
		    bonddb->l->_db_connect_db(bonddb->l,
		                              bonddb->connectionstr_admin,
		                              provider, notice_list);
		if (bonddb->l->
		        _db_checkconnerror(bonddb->conn_admin, &errormsg))
			{
			errormsg("Connection to database as administrator failed with %s "
			         "argument.\n%s", bonddb->connectionstr_admin,
			         errormsg);
			strncpy(bonddb_lasterrmsg, errormsg, 254);
			bonddb_cleanup(bonddb);
			return NULL;
			}
		debug_output("Admin connection active.\nConnecting to database: %s\n",
		             bonddb->connectionstr_admin);
		}
	bonddb_plugin_setcache(bonddb->l, TRUE);

	debug_output("Connection to the database established successfully.\n");
	/* globaldb = db_builddef(); debug_output("Table and field structures
	   successfully loaded.\n"); globaldb->connstr = mem_strdup(c); */
	
	bonddb->oo_conns = g_hash_table_new(&g_str_hash, &g_str_equal);
	/* remote obj hack */
	return bonddb;
	}

gchar *
bonddb_connect_errormsg(void)
	{
	return bonddb_lasterrmsg;
	}

/* shutdown database connection */

/**
 * bonddb_cleanup()
 *
 * Frees up all memory used by db_init(). Will flush all write ahead caches 
 * to make sure data is saved and then close database connections.  Make 
 * sure you call this else your last record entered maybe lost.
 *
 * @return  non-zero on failure.
 */
gint
bonddb_cleanup(BonddbGlobal * bonddb)
	{
	GList *walk;
	BonddbClass *res;

	if (bonddb->conn)
	{
		if (bonddb->conn != bonddb->conn_remote_write)
			bonddb->l->_db_disconnect_db(bonddb->l, bonddb->conn_remote_write);
		bonddb->l->_db_disconnect_db(bonddb->l, bonddb->conn);
		bonddb_plugin_free(bonddb->l);
	}

	if (bonddb->alloclist)
		debugmsg("%d BonddbClass* results still allocated",
		         g_list_length(bonddb->alloclist));
	for (walk = g_list_first(bonddb->alloclist); walk != NULL; walk = walk->next)
		{
		res = walk->data;
		debugmsg("NOT FREED:\n%s", res->query);
		}
	if (bonddb->alloclist)
		g_list_free(bonddb->alloclist);
	mem_free(bonddb->connectionstr);
	mem_free_nn(bonddb->connectionstr_remote_write);
	mem_free_nn(bonddb->connectionstr_admin);
	g_hash_table_destroy(bonddb->oo_conns);
	mem_free(bonddb);
	return 0;
	}

BonddbPlugin *
bonddb_get_dbplugin(BonddbGlobal * bonddb)
	{
	g_assert(bonddb);
	return bonddb->l;
	}

GList *
bonddb_get_grouplist(BonddbGlobal * bonddb, gchar * username)
	{
	g_assert(bonddb);
	return bonddb->l->_db_findgroups(bonddb->l, bonddb->conn, username);
	}
