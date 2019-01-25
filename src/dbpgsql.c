#include <string.h>
#include <glib.h>

#include "dbwrapper.h"

#ifdef _PGSQL

#include "bcdebug.h"
#include "dbpgsql.h"
#include "dbuniqueid.h"
#include "dbsqlparse.h"

/*===========================================================================================
 * Connection based functions.
 *===========================================================================================
 */

/**
 * db_dbconnect:
 * @initstring: Postgresql initilising string
 * 
 * Direct wrapper for PQconnectdb(@initstring).  
 * Provider is ignored when connecting directly to postgresql.
 * 
 * Returns: Connection to database.
 */
DbConnection *
db_dbconnect(gchar * initstring, gchar *provider)
	{
	DbConnection *ret;

	g_assert(initstring);
	ret = (DbConnection *) mem_alloc(sizeof(DbConnection));
	ret->connstring = mem_strdup(initstring);
	ret->conn = PQconnectdb(initstring);
	ret->dbserver = DB_PGSQL;
	ret->errormsg = NULL;
	return ret;
	}

/**
 * db_dbstatus:
 * @conn: Database Connection
 * 
 * Direct wrapper for PQstatus(@conn).  
 *
 * Returns: status, a non-zero on failure
 */
gint
db_dbstatus(DbConnection * conn)
	{
	g_assert(conn);
	if (PQstatus(conn->conn) == CONNECTION_BAD)
		return 1;
	return 0;
	}

/**
 * db_dberrormsg:
 * @conn: Database Connection
 * 
 * Direct wrapper for PQerrormsg(@conn).  
 *
 * Returns: gchar* not to be freed.
 */
gchar *
db_dberrormsg(DbConnection * conn)
	{
	g_assert(conn);
	return PQerrorMessage(conn->conn);
	}

/**
 * db_dbfinish:
 * @conn: Database Connection
 * 
 * Direct wrapper for PQfinish(@conn).
 */
void
db_dbfinish(DbConnection * conn)
	{
	g_assert(conn);
	PQfinish(conn->conn);
        if (conn->errormsg)
	     mem_free(conn->errormsg);
	mem_free(conn->connstring);
	mem_free(conn);
	}

/**
 * db_dbreset:
 * @conn: Database Connection
 * 
 * Direct wrapper for PQreset(@conn).  This function is not essential and i dont think it is 
 * even called anywhere. So no need to make use of it.
 */
void
db_dbreset(DbConnection * conn)
	{
	g_assert(conn);
	PQreset(conn->conn);
	}

/*===========================================================================================
 * Result based functions.
 *===========================================================================================
 */

/**
 * db_dbcheckresult:
 * @result: Query result
 * 
 * Checks if any errors occured when running a query to postgresql, and if so return the
 * print the error messages and free any associated objects.
 *
 * Possibly a wrapper for PQresultStatus
 * 
 * Returns: non-zero on error
 */
gint
db_dbcheckresult(DbRecordSet * result)
	{
	int err;

	g_assert(result);
	if (result == NULL)
		{
		errormsg("Result is NULL");
		return -1;
		}
	if (result->res == NULL)
		{
		mem_free(result);
		errormsg("Result->res is NULL");
		return -3;
		}
	err = PQresultStatus(result->res);
	if (err == PGRES_BAD_RESPONSE || err == PGRES_NONFATAL_ERROR
	        || err == PGRES_FATAL_ERROR)
		{
		if (globaldbconn->errormsg)
			mem_free(globaldbconn->errormsg);
		globaldbconn->errormsg = NULL;
		
		globaldbconn->errormsg = mem_strdup(PQresultErrorMessage(result->res));
		errormsg("Result failed with %s", globaldbconn->errormsg);
		errormsg("Query was %s", result->query);
		/* Please note: people put dumb stuff into db programs. If we kill the
		   app every time they do, it's gonna suck.  Badly. So don't! */
		/* I'm putting this in again to do some crazy testing. */
		/* g_assert(NULL); */

		/* db_dbclear(result); result->res = NULL; */
		return -2;
		}
	
	if (globaldbconn->errormsg)
		{
		mem_free(globaldbconn->errormsg);
		globaldbconn->errormsg = NULL;
		}
	return 0;
	}

/**
 * db_dbclear:
 * @result: Query result
 * 
 * Direct wrapper for PQclear(@result).
 */
void
db_dbclear(DbRecordSet * result)
	{
	if (result == NULL)
		return ;
	PQclear(result->res);
	mem_free(result->query);
	mem_free(result);
	result = NULL;
	}

/**
 * db_dbexec:
 * @conn: Database connection
 * @query: Query string
 * 
 * Direct wrapper for PQexec(@conn, @query).
 *
 * Returns: DbRecordSet to database.
 */
DbRecordSet *
db_dbexec(DbConnection * conn, gchar * query)
	{
	DbRecordSet *result;

	g_assert(conn);
	g_assert(query);
	result = (DbRecordSet *) mem_alloc(sizeof(DbRecordSet));
	result->query = mem_strdup(query);
	result->res = PQexec(conn->conn, query);
	/* Francis: Added the query result check */
	if (!result->res)
		{
		errormsg
		("According to postgresql documentation NULL value return for PQexec is a fatal error:\n%s\n",
		 PQerrorMessage(conn->conn));
		/* g_assert(0); */
		mem_free(result->query);
		mem_free(result);
		return NULL;
		}

	/* debugmsg("Adding query %s",result->query); */
	return result;
	}

/**
 * db_dbnumrows:
 * @result: Result from query
 * 
 * Direct wrapper for db_dbnumrows(@result);.
 *
 * Returns: Number of rows.
 */
gint
db_dbnumrows(DbRecordSet * result)
	{
	g_assert(result);
	g_assert(result->res);
	return PQntuples(result->res);
	}

/**
 * db_dbgetvalue:
 * @result: Result from query
 * @row: Position in recordset
 * @field: Field position
 * 
 * Direct wrapper for PQgetvalue(@result, @row, @field);.
 *
 * Returns: String of field
 */
gchar *
db_dbgetvalue(DbRecordSet * result, gint row, gint field)
	{
	gchar *retval;

	retval = PQgetvalue(result->res, row, field);
	if (retval == NULL)
		{
		errormsg("NULL returned for extraction at point %d, %s\n%s", 
			 field, PQresultErrorMessage(result->res),
			 result->query);
		g_assert(NULL);
		return NULL;
		}
	if (strlen(retval) == 0)
		return NULL;
	return retval;
	}

/**
 * db_dbnumfields:
 * @result: Result from a query
 * 
 * Direct wrapper for PQnfields(@result);.
 *
 * Returns: Number of fields in the @result.
 */
gint
db_dbnumfields(DbRecordSet * result)
	{
	g_assert(result);
	g_assert(result->res);
	return PQnfields(result->res);
	}

/**
 * db_dbfieldname:
 * @result: Result from a query
 * @fieldpos: Field position
 * 
 * Direct wrapper for PQfname(@result,@fieldpos);.
 *
 * Returns: Name of a field in recordset.
 */
gchar *
db_dbfieldname(DbRecordSet * result, gint fieldpos)
	{
	return PQfname(result->res, fieldpos);
	}

/**
 * db_dbuniqueid:
 * @result: Result from a insert
 * @fieldpos: Field position
 * 
 * Kinda a wrapper for PQoidValue(@result);
 * Assuming you run am insert query with db_dbexec(), this will return the unique id assocated with
 * that record.  Note this works on just getting the oid value from postgresql, on other databases
 * a bit of a work around will be needed.
 *
 * Returns: the pg OID value.
 */
glong
db_dbuniqueid(DbConnection *conn, DbRecordSet * result)
	{
	gint oid;

	g_assert(result);
	g_assert(result->res);
	oid = PQoidValue(result->res);
	if (oid <= 0)
		return 0;
	return oid;
	}

#endif
