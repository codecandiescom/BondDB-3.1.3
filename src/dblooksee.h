#ifndef dblooksee_h
#define dblooksee_h

/* DBLOOKSEE
	treshna Enterprises Ltd
	
 	Advanced searching front end to database.
 	Simple enough code, it just builds the sql statement and excutes it, saving
 	results back into an Object*
*/

#include "dbobject.h"

#define DBSEARCH_EQUAL 0
#define DBSEARCH_CONTAIN 1
#define DBSEARCH_NOTEQUAL 2
#define DBSEARCH_GREATER 3
#define DBSEARCH_LESS 4
#define DBSEARCH_STARTWITH 5
#define DBSEARCH_GREATEREQUAL 6
#define DBSEARCH_LESSEQUAL 7

typedef struct
   {
   gchar *table;
   gchar *where;
   gchar *sort;
   }
DbLookSee;

void db_looksee_addand(DbLookSee * looksee, gchar * field, gchar * value, gint type, gint datatype);
void db_looksee_addor(DbLookSee * looksee, gchar * field, gchar * value, gint type, gint datatype);
void db_looksee_addbracket(DbLookSee * looksee, gint type);
void db_looksee_addsort(DbLookSee * looksee, gchar * field);

DbLookSee *db_looksee_create(gchar * table);
void db_looksee_abort(DbLookSee * looksee);
void db_looksee_addquery(DbLookSee * looksee, gchar * query);
Object *db_looksee_exec(DbLookSee * looksee);

gchar *db_looksee_gettype(gint type);

#endif
