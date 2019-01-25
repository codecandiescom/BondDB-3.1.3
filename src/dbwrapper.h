#ifndef dbwrapper_h
#define dbwrapper_h

/* #define _PGSQL */
/* #define _MYSQL */
/* #define _GDA */

/**
 * Low level calls to postgresql, or another database provider depending what
 * is asked for in #defines.  This is a straight wrapper around psql or gnome-db
 * or what ever else providers maybe supported 
 */

#ifdef _PGSQL

/* Postgresql specific headers to be included. */
#include <libpq-fe.h>
#endif

#ifdef _GDA

/* Gnome-db specific headers, this is for supporting lots of databases */
#include <libgda.h>
#endif

#ifdef _MYSQL

/* Mysql header files */
#include <mysql/mysql.h>
#endif

/* Standard structures for databases.  */

typedef struct
	{
	gchar *query;
#ifdef _PGSQL

	PGresult *res;
#endif
#ifdef _GDA
	GdaCommand *command;
	GdaDataModel *res;
#endif 
#ifdef _MYSQL
	   
	MYSQL_RES *res;
#endif
	}
DbRecordSet;

typedef struct
	{
	/* connection string */
	gchar *connstring;
	/* type of database your connecting to */
	enum
	{
	    DB_UNKNOWN,
	    DB_PGSQL,
	    DB_MYSQL,
	    DB_GDA
	} dbserver;
#ifdef _PGSQL

	PGconn *conn;
#endif
#ifdef _GDA
	
	GdaConnection *conn;
	GdaClient *client;
	gchar *provider;
#endif
#ifdef _MYSQL

	MYSQL *conn;
#endif
	gchar *errormsg;
	}
DbConnection;

extern DbConnection *globaldbconn;

#ifdef _PGSQL

/* Posgresql Code is here */
#include "dbpgsql.h"
#endif

#ifdef _GDA

/* Gnome DB code is here */
#include "dbgda.h"
#endif

#ifdef _MYSQL
#include "dbmysql.h"
#endif

/* some functions .. .*/
gint db_dbgetserver();

#endif
