#include <glib.h>
#include <gmodule.h>

#include "dbgather.h"
#include "dbmethod.h"

#include "dbmethodmodule.h"


static GModule *widgetMethodModule;

gboolean
db_method_module_init (const gchar *pluginPath, const gchar *pluginName) 
{
	gchar * moduleFileName;
	
	moduleFileName = g_module_build_path (pluginPath, pluginName);
	printf ("Module filename: %s\n", moduleFileName);
	widgetMethodModule = g_module_open (moduleFileName, G_MODULE_BIND_LAZY);
	
	if (!widgetMethodModule) {
	printf ("Loading Failed!!!\n\n\n");
		return FALSE;
	}

	g_free (moduleFileName);

	return TRUE;
}


gboolean
db_method_module_cleanup ()
{
	return g_module_close (widgetMethodModule);
}


const gchar *
db_method_module_get_module_name ()
{
	return g_module_name (widgetMethodModule);
}

/**
 *
 * @obj: Hmm I still don't know
 * @method: This method to be executed. This must be somehow related to
 *           the obj
 * @retval: Some value processed by a user callback. Note that it is
 * the user's responsibility to cast the returned value properly.
 *
 * Returns: TRUE if successful. Otherwise FALSE
 */

gboolean
db_method_module_execute (
						Object *obj,
						const gchar *funcname,
						GList *arglist,
						gpointer * retval)
{
	gboolean res;
	gpointer (*callbackFunc) (Object *, GList *);
	
	g_assert (funcname);
	/*g_assert (arglist);*/
	

	*retval = NULL;
	res = g_module_symbol (
		widgetMethodModule,
		funcname,
		(gpointer)&callbackFunc);
	
	if (!res) {
		printf ("Failed to find function: %s\n", funcname);
		return FALSE;
	}
	
	/* Now let's call ! */
	*retval = callbackFunc(obj, arglist);
	
	return TRUE;
}

/**
 * Just a nice high-level function.
 * @callbackname
 */
DbMethodDef *
db_method_module_find(gchar * callbackname)
{
	DbDatabaseDef * db;
	
	db = db_get_current_database ();
	
	g_assert (db);
	
	return db_method_find (db->methods, callbackname);
}



/**
 * Return all the methods of the current database
 */
GList *
db_method_module_get_all_methods()
{
	DbDatabaseDef * db;
	
	db = db_get_current_database ();
	
	g_assert (db);
	
	return db->methods;
}
