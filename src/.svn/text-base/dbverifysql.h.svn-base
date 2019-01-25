#ifndef dbverifysql_h
#define dbverifysql_h

#include "dbobject.h"

/**
 dbverifysql.h
 
 These functions check a sql statement parsed in and verify that its ok, modify
 it to have the nesserary fields to extract and return a structure detailing 
 how to fill your object.
*/

gint db_verifysql_parse(Object * obj, gchar *orginalsql);
gint db_verifysql_addfieldtable(Object *obj, gchar *fieldname, gchar *tablename);

#endif
