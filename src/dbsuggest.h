#ifndef dbsuggest_h
#define dbsuggest_h

/* A C FILE DEVOTED TO SUGGESTING STUFF. */
/* OI YOU READING THIS CODE, GET BACK TO WORK! */

#include "dbobject.h"
#include "dbfield.h"

typedef struct
{
   gchar *tablename;
   gchar *fieldname;
   Object *obj;
   gint err;
	DbField *field;
} DbSuggest;

DbSuggest *db_suggest_all(Object *obj, gchar *fieldname, gchar *tablename);
gint db_suggest_wildcard(gchar *field, gchar **fieldname, gchar **tablename);
void db_suggest_free(DbSuggest *suggest);

#endif
