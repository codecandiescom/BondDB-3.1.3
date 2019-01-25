#ifndef __dbsqlparse_h
#define __dbsqlparse_h

#include "dbgather.h"

/* private */
gint db_sql_findstrkeyword(gint offset, gchar * fullstr, gchar * str);

/* understanding sql statements component */
gint db_sql_reftotable(gchar * query, gchar * tablename);

/* suggest a table this could point to */
gchar *db_sqlparse_suggesttable(gchar * fieldname);

/* suggest a field this could point to */
gchar *db_sqlparse_suggestfield(gchar * fieldname);

/* get field and table stuff */
gint db_sqlparse_lookelsewhere(gchar * str, gchar ** retfield, gchar ** rettable);

/* drop the ' marks from a string */
gchar *db_sqlparse_dropquation(gchar * query);

/* gets the first table in a select statement*/
gchar *db_sqlparse_getfirsttable(gchar * sql);

/* gets fields from a select statement */
GList *db_sqlparse_getselectfields(gchar * sql);
void db_sqlparse_freeselectfields(GList *fieldlist);

/* get the common part of the sql statement*/
gchar* db_sqlparse_getcommonsql(gchar *sql);

gint db_sqlparse_typeofsql(gchar *sql);

gint db_sqlparse_getwherefieldsandvalues(gchar * query, DbField *** fields, gchar *** values);

#endif
