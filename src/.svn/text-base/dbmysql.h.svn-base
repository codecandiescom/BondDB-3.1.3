#ifdef _MYSQL

/* Only include file if we are doing a postgresql build */
#ifndef dbmysql_h
#define dbmysql_h

#include "dbwrapper.h"
#include "dbheader.h"

/* This is for postgresql specific functions that are needed */

/* If you're stuck read postgresql documentation, because this code is so damn basic. */

/* Connection Based Functions */
DbConnection *db_dbconnect(gchar * initstring);
gint db_dbstatus(DbConnection * conn);
gchar *db_dberrormsg(DbConnection * conn);
void db_dbfinish(DbConnection * conn);
void db_dbreset(DbConnection * conn);

/* Result Based Functions */
gint db_dbcheckresult(DbRecordSet * result);
void db_dbclear(DbRecordSet * result);
DbRecordSet *db_dbexec(DbConnection * conn, gchar * query);
gint db_dbnumrows(DbRecordSet * result);
gchar *db_dbgetvalue(DbRecordSet * result, gint row, gint field);
gint db_dbnumfields(DbRecordSet * result);
gint db_dbuniqueid(DbRecordSet * result);
gchar *db_dbfieldname(DbRecordSet * result, gint fieldpos);

#endif /* dbmysq_h */

#endif /* _MYSQL */
