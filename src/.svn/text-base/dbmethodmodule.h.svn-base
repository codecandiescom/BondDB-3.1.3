#ifndef bondmethod_h
#define bondmethod_h

gboolean
db_method_module_init (const gchar *pluginPath, const gchar *pluginName);

gboolean
db_method_module_cleanup ();

const gchar *
db_method_module_get_module_name ();

gboolean
db_method_module_execute(Object *obj,const gchar *funcname, GList *arglist, gpointer * retval);

DbMethodDef *
db_method_module_find(gchar * callbackname);

GList *
db_method_module_get_all_methods();

#endif
