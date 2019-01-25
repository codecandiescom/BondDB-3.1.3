#ifdef _GDA

/* This is for getting postgresql specific information from the database */

#ifndef dbgdagather_h
#define dbgdagather_h

#include "dbwrapper.h"
#include "dbgather.h"

gint db_buildtabledef(DbDatabaseDef * db);
gint db_buildfielddef(DbTableDef * db);
gint db_buildviewdef(DbDatabaseDef * db);

gint db_buildfielddefcomment(DbTableDef * db);

/* for all view functions, work out orginal tables fields came from */
void db_tablesourceforallfields(DbDatabaseDef * db);

/* for dbgather stuff */
gint db_buildfielddefdefaultvalue(DbTableDef * db);
gint db_pg_getversion();

#endif /* dbgdagather_h */

#endif /* _GDA */
