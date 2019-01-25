
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gmodule.h>
#include <bc.h>

/*#include <dlfcn.h>  -- changed to use glib mechanism instead */

/*#include <glob.h>  -- changed to use g_dir isntead */

#include "bonddbplugin.h"


/* Where to search for the plugin */
static char *plugin_search_path[/*NUMPLUGINPATHS*/] = {
#ifdef UNIX
        "./.libs/",
        "./lib/",
        "/usr/local/lib/",
        "/usr/lib/",
#endif	
#ifdef WIN32 
        "./", /* windows: search current dir */
#endif
	};
	
#define NUMPLUGINPATHS ((sizeof plugin_search_path)/(sizeof plugin_search_path[0]))
#ifdef UNIX
#  define PATHVAR "LD_LIBRARY_PATH"
#  define PATHSEP ':'
#  define SLASHES "/"
#else
#  ifdef WIN32
#    define PATHVAR "PATH"
#    define PATHSEP ';'
#    define SLASHES "\\/"
#  else
#warning "How are libraries named?"
#  endif
#endif

#define ADD_BINDING(af)\
  do {gpointer p;\
  if (!g_module_symbol (handle,#af,&p)){\
      errormsg ("Unable to load function " #af);			\
      return -1;							\
    }\
  plugin->af=p;\
  } while (0)

/*  assigment to plugin->af is done in two stages to handle the any case where
 * sizeof ( gpointer) != sizeof( gpointer(*)()) */

static gint
bonddb_plugin_build(BonddbPlugin * plugin, GModule * handle)
	{

	/* void *(*bdb_exec) (void *connection, char *query); int (*bdb_numrecord)
	   (void *res); int (*bdb_reset) (void *res); int (*bdb_numcolumn) (void
	   *res); char *(*bdb_columnname) (void *res, int pos); char
	   *(*bdb_get_value) (void *res, char *field); void *(*bdb_connect_db) (char 
	   *connectstr, char *provider); int (*bdb_disconnect_db) (void
	   *connection); int (*bdb_clear) (void *query); int (*dbd_cleanup) (void);
	   int (*bdb_init) (); */
	ADD_BINDING(_db_type);
	ADD_BINDING(_db_exec);
	ADD_BINDING(_db_checkconnerror);
	ADD_BINDING(_db_checkerror);
	ADD_BINDING(_db_tuples_ok);
	ADD_BINDING(_db_numtuples);
	ADD_BINDING(_db_reset);
	ADD_BINDING(_db_numcolumn);
	ADD_BINDING(_db_fieldname);
	ADD_BINDING(_db_tablename);
	ADD_BINDING(_db_fieldlist);
	ADD_BINDING(_db_freefieldlist);
	ADD_BINDING(_db_primarykeylist);
	ADD_BINDING(_db_defaultvalue);
	ADD_BINDING(_db_findreference);
	ADD_BINDING(_db_findgroups);
	ADD_BINDING(_db_freereference);
	ADD_BINDING(_db_get_value);
	ADD_BINDING(_db_uniqueid);
	ADD_BINDING(_db_connect_db);
	ADD_BINDING(_db_disconnect_db);
	ADD_BINDING(_db_clear);
	ADD_BINDING(_db_cleanup);
	ADD_BINDING(_db_init);
	ADD_BINDING(_db_check_notify); 
	ADD_BINDING(_db_get_user);
	ADD_BINDING(_db_conn_has_role);
	ADD_BINDING(_db_user_has_role);
	/* ADD_BINDING(); ADD_BINDING(); */
	if (plugin->_db_init(plugin) != 0)
		return -1;

	{ /* setup notify callback where available */
 	}
	return 0;
}

/*
 * Load the library
 */
static BonddbPlugin *
add_library(char *library, char *suffix)
	{
	BonddbPlugin *l;
	GModule *handle;
	char *(*func) (void);
	char *tmpstr;

	if (!library)
		return NULL;
	handle = g_module_open(library, G_MODULE_BIND_LAZY);
	if (!handle)
		{
		errormsg("Couldn't open module: %s\n%s", library, 
			g_module_error());
		return NULL;
		}

	/* if (!g_module_symbol(handle, "_db_type",(gpointer*)(&func)))
		 printf ("1no _db_type\n");
	 else
		 printf ("1got _db_type\n");
	*/
	if (!g_module_symbol(handle, "_db_type", (void *) (&func)))
		{
		warningmsg("%s, %s", g_module_error(), library);
		warningmsg("This is normally caused by not running ./configure "
		           "properly at compile time or this database backend "
		           "is not supported. ");
		g_module_close(handle);
		return NULL;
		}
	tmpstr = func();
	if (!tmpstr || strcmp(tmpstr, suffix) != 0)
		{
		debug_output("Library %s doesnt match %s suffix internally,",
		             library, suffix);
		if (tmpstr)
			debug_output("library identifies itself as %s", tmpstr);
		g_module_close(handle);
		return NULL;
		}

	l = (BonddbPlugin *) mem_calloc(sizeof(BonddbPlugin));

	l->handle = handle;
	l->notices = NULL;

	l->libraryname = mem_strdup(library);
	l->type = mem_strdup(tmpstr);
	if (bonddb_plugin_build(l, handle) != 0)
		{
		mem_free(l->type);
		mem_free(l->libraryname);
		mem_free(l);
		g_module_close(handle);
		errormsg("Failed to build plugin library");
		return NULL;
		}
	debug_output("Library %s successfully loaded.\n", library);
	return l;
	}

/**
 * bonddb_plugin_init
 * @param dbsuffix :
 *
 * Looks for libraries with suffix @dbsuffix
 *
 * @return  the library plugin for library with @dbsuffix
 *
 */
BonddbPlugin *
bonddb_plugin_init(gchar * dbsuffix)
	{
	BonddbPlugin *plugin = NULL;
	gchar *pattern;
	GDir *d;
	gchar *dirname=NULL,*precursor=NULL;	
	/* places to search for plugins before searching Plugin_search_path */
	gchar *searches[]={  
		getenv("BONDDBLIBS"), 					/* system variable */
		bcconfig_getvalue("plugin_search_path"),/* config setting */
		getenv(PATHVAR),                         /* os-specific variable */
		};

	int i=-(sizeof(searches)/sizeof(searches[0]));
	/* debuglog(99,"I=%d\n",i); */
	
	g_assert(dbsuffix);

	pattern = mem_strdup_printf(
#ifndef BONDDBLIBNAMESKEL
#ifdef UNIX
	           "libbonddb2_%s.so"
#endif
#ifdef WIN32
	           "bonddb2_%s.dll"
#endif
#else
	           BONDDBLIBNAMESKEL
#endif
	           , dbsuffix);


	plugin = NULL;
	
 for(;;)	 
	 {
	 	/*gchar *target_dir;*/
		const gchar *fname=NULL;
		 

		if(precursor){ /* use env vars first */
			gchar *t=strchr(precursor,PATHSEP);
			mem_strdup_replace(&dirname,
		        "%.*s%c",t?t-precursor:255,precursor,'/');
					/* / should work in windows paths... */
			precursor=t?t+1:0;
		}
		else if(i<0)
		{
			precursor=searches[(sizeof(searches)/sizeof(searches[0]))+i++];
			continue;
		}
		else if(i<NUMPLUGINPATHS)
		{
			mem_strdup_replace(&dirname,"%s",plugin_search_path[i++]);
		}
		else break;
		
		debuglog(95," checking %s for %s ",dirname,pattern);
		
		{  /* trim double trailing slash */
			int i=strlen(dirname);
			if(i>1 && strchr(SLASHES,dirname[i-2]))dirname[i-1]=0;
		}
		
		d=g_dir_open(dirname,0,NULL);
		/* debuglog(99,"checking directory '%s' for '%s' \n", dirname, pattern); */
	 
		while (d && (fname=g_dir_read_name(d)))
		{
			if (
#ifndef WIN32			
				!strcmp(fname,pattern)  
#else			
				!g_ascii_strcasecmp(fname,pattern) /* for windows ignore filename case */
			/* should I be using a UTF-8 compare here(?)- No because we make up the filenames, */
#endif
					)	break;
		}
		if(fname){ 
				gchar *fn;
				fn=mem_strdup_printf("%s%s",
					dirname,fname);
				debug_output("Checking library: %s for %s\n", fn,dbsuffix);
				plugin = add_library(fn, dbsuffix); 
				if(plugin)debug_output("Loaded library: %s\n", fn);
				mem_free(fn);
			}
		if (d)	g_dir_close(d);
		if(plugin) break;
		} /* for loop across list of dirs */
	mem_free(pattern);
	if(dirname)mem_free(dirname);	
	if(!plugin)errormsg("Failed to load plugin library for %s\n", dbsuffix);
	return plugin;
	}

/**
 * bonddb_plugin_free
 * @param l :
 *
 * close the dynamic loading, and free the memory allocated for the plugin
 *
 */
void
bonddb_plugin_free(BonddbPlugin * l)
	{
	l->_db_cleanup(l);
	g_module_close(l->handle);
	mem_free(l->type);
	mem_free(l->libraryname);
	mem_free(l);
	}

/**
 * bonddb_plugin_setcache
 * @param l :
 * @param state :
 *
 * sets the cache to the state passed in for the plugin l.
 *
 */
void
bonddb_plugin_setcache(BonddbPlugin * l, gboolean state)
	{
	l->cache_dictonary = state;
	}
