#ifndef dbverifysql_h
#define dbverifysql_h

#include "bonddbobj.h"

/**
 *dbverifysql.h
 *
 *These functions check a sql statement parsed in and verify that its ok, modify
 *it to have the nesserary fields to extract and return a structure detailing 
 *how to fill your object.
*/
/*static GList *bonddbparse_verifysql_addfield(GList *retlist, DbFieldDef *fielddef, DbTableDef *tabledef);
static gint bonddbparse_verifysql_addjoin(BonddbClass *obj, GList *tablelist, gchar *tablename); */
gint bonddbparse_verifysql(BonddbClass *obj, gchar *orginalsql);
gint bonddbparse_verifysql_addfieldtable(BonddbClass *obj, gchar *fieldname, gchar *tablename);
gchar *bonddb_parse_getfirsttable(gchar * sql);
gint bonddb_parse_tablelist(BonddbClass *obj);

#endif
