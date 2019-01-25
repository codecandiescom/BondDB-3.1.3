#ifndef dbdefault_h
#define dbdefault_h

#include "dbgather.h"
#include "dbobject.h"

gint db_default_populateobject(Object *obj);


/* execute a function to get the default value for a field */
gchar *db_default_execfunction(gchar * function);

/* write the default values to cache, so things like id arn't got twice */
gint db_default_addwrite(Object * obj, gchar * field, gchar *table, gchar * value);

/* new record created */
gchar *db_default_value(DbTableDef * db, gchar * fieldname); 	/* for the simple defaults */

#endif
