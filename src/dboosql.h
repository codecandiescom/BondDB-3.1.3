#ifndef __dboosql_h
#define __dboosql_h

#include "dbpath.h"
#include "dbobject.h"

/*=====================================================================================*/

/* The theory:
	An object will normally have a base effilation with a table. 
	The traditional way is a single sql statement getting a single table contents.
	What this module seeks to do is allow you to tap other tables onto your
	base table.
*/

/* Add a table into the SQL statement */
gint db_oo_generatesql(Object * obj, gchar * table);

/* Generationg a SQL query from complex paths */
gint db_oo_generatesql_addpath(Object * obj, DbPath * path);

#endif
