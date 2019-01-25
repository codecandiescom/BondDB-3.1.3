#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
/*#include <glob.h>*/
/*#include <dlfcn.h>*/

#include "dbclient.h"

#include "dbgather.h"
#include "dbobject.h"
#include "dbwrapper.h"
#include "dbtoliet.h"
#include "dbbureaucrat.h"
#include "dblog.h"

#include "bcconfig.h"
#include "bcdebug.h"

#define NUMPLUGINPATHS 4
#define NUMBONDCONFSEARCH 5

#if 0
static gchar *plugin_search_path[NUMPLUGINPATHS] = {
            "./.libs/",
            "./",
            "/usr/local/lib/",
            "/usr/lib/"
        };
#endif

/* startup database connections. This is principle init function for bonddb */

/*
 *
 * Look for @searchforstr in @parentstr, return 0 if true.
 */
	
/** why not just use strstr()  ////JB////  **/	
	
static gint
db_strcontain(gchar * parentstr, gchar * searchforstr)
	{
	gint i, pass = 0;

	if (searchforstr == NULL || parentstr == NULL)
		return 0;
	for (i = 0; i < strlen(parentstr); i++) /* O(N^2)  ////JB//// */
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

static gchar *
db_init_getconnstr(void)
	{
	gchar *retstr, *tmpstr;
	gchar *host;
	gchar *username;
	gchar *password;
	gchar *db;

	retstr = bcconfig_getvalue("db_connection_string");
	if (retstr)
		return mem_strdup(retstr);
	db = bcconfig_getvalue("db_name");
	username = bcconfig_getvalue("db_username");
	password = bcconfig_getvalue("db_password");
	host = bcconfig_getvalue("db_host");
	if (db == NULL)
		{
		error_output("No database name is specified to connect to\n");
		return NULL;
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
 * db_init:
 * @conninfo: standard postgresql database connection string or the name of the application
 *
 * Establishes database connection to postgresql, and extracts all the 
 * information postgresql needs to work with the database.  If @conninfo 
 * is NULL then the bond.conf configuration files will 
 * be read to get the database initialising string.
 *
 * @conninfo can be either the name of the config file (ie appname.conf)
 * or a database connection string. It looks for the string dbname
 * in @conninfo and if present it assumes its a database connection
 * string else it assumes its not and is a filename.
 * 
 * Returns: non-zero on failure.
 */
gint
db_init(gchar * conninfo)
	{
	gchar *c, *provider = NULL;

	/* debugmsg("passing in conninfo of %s", conninfo); */
	/* this is a tad complex and can cause a few bugs. If conninfo is a db string then initialise it else do a read on a
	   config file */
	if (conninfo == NULL || db_strcontain(conninfo, "dbname") != 0)
		c = db_init_getconnstr();
	else
		c = mem_strdup_printf("%s", conninfo);
	if (c == NULL)
		{
		errormsg("Error creating connection string from %s. Aborting", conninfo);
		return -1;
		}

	if (globaldbconn != NULL)
		db_cleanup();
	debug_output("Connecting to database: %s\n", c);
	/* dont you hate it when code you write goes missing? */
	globaldbconn = db_dbconnect(c, provider);

	if (db_dbstatus(globaldbconn) != 0)
		{
		db_dbfinish(globaldbconn);
		errormsg("Connection to database failed with %s argument.\nError msg %s", c, db_dberrormsg(globaldbconn));
		mem_free(c);
		exit( -1);
		}
	debug_output("Connected establised, will parse for info.\n");
	globaldb = db_builddef();
	debug_output("Table and field structures successfully loaded.\n");
	globaldb->connstr = mem_strdup(c);
	mem_free(c);
	return 0;
	}

/* shutdown database connection */

/**
 * db_cleanup:
 *
 * Frees up all memory used by db_init(). Will flush all write ahead caches to make sure data is 
 * saved and then close database connections.  Make sure you call this else your last record entered
 * maybe lost.
 *
 * Returns: non-zero on failure.
 */
gint
db_cleanup(void)
	{
	extern DbDatabaseDef *globaldb;

	/* flush everything outstanding */
	db_toliet_flushall();
	/* clear out the bureaucrat */
	db_bureaucrat_cleanup();
	/* empty the logs */
	db_lomem_free();
	/* free tables up */
	db_freedatabasedef(globaldb);
	/* close connections */
	db_dbfinish(globaldbconn);
	globaldbconn = NULL;

	return 0;
	}

/**
 * db_restart:
 * 
 * Restart database connection, incase it either dies.
 *
 * Returns: non-zero on failure.
 */
gint
db_restart(void)
	{
	db_dbreset(globaldbconn);
	return 0;
	}

#if 0

/* load that library eh */
static void *
check_shared_library(char *so)
	{
	GModule *handle;
	gpointer *sym;

	if (!(handle = g_module_open(so, G_MODULE_BIND_LAZY)))
		{
		errormsg("An error occured while reading the shared library %s\n%s", so, dlerror());
		return NULL;
		}

	if ((sym = g_module_symbol(handle, "plugin_init",&sym)) == 0)
		{
		g_module_close(handle);
		return NULL;
		}
	return handle;
	}
#endif
gint
db_loadbindings(gchar * driver)
	{
	printf("please write me\n");
/* that's going to help with the windozw port */
/* well, atleas it shoufd compile now! ////JB//// */	
	return 0;
	}

/*
static void*
db_load_library(gchar *so);
	{
	gint i, count;
	glob_t g;
	gchar *pattern;
 
	g_assert(library);
	
	plugin = mem_alloc(sizeof(Plugin));
	if (args != NULL) 
		args = mem_strdup(args);
	if (library!= NULL) 
		library = mem_strdup(library);
	
	plugin->name = library;
	plugin->args = args;
	plugin->func = NULL;
	plugin->numfunctions = 0; 
 
	for (i = 0; i < NUMPLUGINPATHS; i++)
		{
		pattern = mem_strdup_printf("%s*.so", plugin_search_path [i]);
		
		if (glob (pattern, 0, NULL, &g) == 0)
			{
			for (count = 0; count < g.gl_pathc; count++)
				{
				debugmsg ("Checking library: %s", g.gl_pathv[count]);
				
				plugin->dlhandle = check_shared_library (g.gl_pathv[count]);
				
				if (plugin->dlhandle!=NULL)
					{
					plugin->sym = dlsym (plugin->dlhandle, "plugin_init");
					g_assert (plugin->sym);
					if (plugin->sym (plugin) == 0)
						{
						globfree (&g);
						debugmsg("Plugin successfully loaded.");
						return plugin;
						}
					}
				}
			}
		mem_free (pattern);
		}
	warningmsg("Failed to load plugin\n");
	mem_free (plugin->name);
	mem_free (plugin->args);
	mem_free (plugin);
	} */
