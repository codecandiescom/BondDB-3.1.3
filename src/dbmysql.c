#include <string.h>
#include <glib.h>

#include "dbwrapper.h"

#ifdef _MYSQL

#include "bcdebug.h"
#include "dbmysql.h"
#include "dbuniqueid.h"
#include "dbsqlparse.h"

/*===========================================================================================
 * Connection based functions.
 *===========================================================================================
 */

/**
 * db_dbconnect:
 * @initstring: Mysql initilising string
 * 
 * Direct wrapper for mysql_init(@initstring).  
 *
 * Returns: Connection to database.
 */
DbConnection *
db_dbconnect(gchar * initstring)
	{
	DbConnection *ret;
	gchar **conn_fields;
    gchar *newconnectstr;
                                                                                
    char *host = NULL;
    char *user = NULL;
    char *passwd = NULL;
    char *db = NULL;
                                                                                
	int i;

	g_assert(initstring);
	ret = (DbConnection *) mem_alloc(sizeof(DbConnection));
	ret->connstring = mem_strdup(initstring);
	ret->conn = mysql_init(NULL);
	ret->dbserver = DB_MYSQL;
	
	/* Francis: I am worried about the delimeters used in the connection string. 
	 * Hope this eliminate any problems */
	newconnectstr = g_strdelimit(initstring, " \t\r\n", ' ');
                                                                                                                                                            
    /* Francis: I think 4 is enough (db, host, user, passwd) */
#define MAX_TOKEN 4
                                                                                                                                                            
        conn_fields = g_strsplit(newconnectstr, " ", MAX_TOKEN);
                                                                                                                                                            
        for (i = 0; i < MAX_TOKEN && conn_fields[i]; i++)
                {
                /* printf("conn_fields[%d]:%s\n", i, conn_fields[i]); */
                g_strchomp(conn_fields[i]);
                                                                                                                                                            
                /* if host is null, then assign a string position if "host=" is found */
                if (!host && (host = strstr(conn_fields[i], "host=")))
                        {
                        host += strlen("host=");
                        }
                else if (!db && (db = strstr(conn_fields[i], "dbname=")))
                        {
                        db += strlen("dbname=");
                        }
                else if (!user && (user = strstr(conn_fields[i], "user=")))
                        {
                        user += strlen("user=");
                        }
                else if (!passwd && (passwd = strstr(conn_fields[i], "passwd=")))
                        {
                        passwd += strlen("passwd=");
                        }
                }
                                                                                                                                                            
        /* printf("host:%s\nuser:%s\npasswd:%s\ndb:%s\n", host,user,passwd,db); */
        /* port; use default port number by inspecting host */
        /* unix_socket; use appropriate socket by looking at host */
        /* client_flag */
        if (!mysql_real_connect(ret->conn, host, user, passwd, db, 0, NULL, 0))
                {
                errormsg("Mysql: failed to connect to database: Error :%s",
                         mysql_error(ret->conn));
				mem_free(ret);
                return NULL;
                }

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
	gchar *retstr;
	g_assert(conn);
	retstr = mysql_error(conn->conn);
	
	if (retstr[0] != 0)
		return retstr;
	return NULL;
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
    mysql_close(conn->conn);
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
	warningmsg("Function not supported under mysql.");
	}

/*===========================================================================================
 * Result based functions.
 *===========================================================================================
 */

/**
 * db_dbcheckresult:
 * @result: Query result
 * 
 * This handled by db_dberrormsg().
 * 
 * Returns: non-zero on error
 */
gint
db_dbcheckresult(DbRecordSet * result)
	{
	return 0;
	}

/**
 * db_dbclear:
 * @result: Query result
 * 
 * Direct wrapper for mysql_free_result@result).
 */
void
db_dbclear(DbRecordSet * result)
	{
	if (result == NULL)
		return ;
	mysql_free_result(result->res);
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
	
	/* Francis: Added the query result check */
	if (mysql_real_query(conn->conn, query, strlen(query)))
		{
		errormsg("According to postgresql documentation NULL value return for PQexec is a fatal error:\n%s\n",
		         mysql_error(conn->conn));
		mem_free(result->query);
		mem_free(result);
		/* g_assert(0); */
		return NULL;
		}
	else
			result->res = mysql_store_result(conn->conn);
	/* debugmsg("Adding query %s",result->query); */
	return result;
	}

/**
 * db_dbnumrows:
 * @result: Result from query
 * 
 * Direct wrapper for mysql_num_rows(@result);.
 *
 * Returns: Number of rows.
 */
gint
db_dbnumrows(DbRecordSet * result)
	{
	g_assert(result);
	g_assert(result->res);
	return mysql_num_rows(result->res);
	}

/**
 * db_dbgetvalue:
 * @result: Result from query
 * @row: Position in recordset
 * @field: Field position
 * 
 * Direct wrapper for mysql_data_seek(@result, @row, @field);.
 *
 * Returns: String of field
 */
gchar *
db_dbgetvalue(DbRecordSet * result, gint row, gint field)
	{
	gchar *retval;
    MYSQL_ROW setrow;
	
	g_assert(result->res);
	mysql_data_seek(result->res, (unsigned long long)row);
	setrow = mysql_fetch_row(result->res);
	retval = setrow[field];
	
	if (retval == NULL)
	     {
	     errormsg("NULL returned for extraction at point %d",field);
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
 * Direct wrapper for mysql_num_fields(@result);.
 *
 * Returns: Number of fields in the @result.
 */
gint
db_dbnumfields(DbRecordSet * result)
	{
	g_assert(result);
	g_assert(result->res);
	return mysql_num_fields(result->res);
	}

/**
 * db_dbfieldname:
 * @result: Result from a query
 * @fieldpos: Field position
 * 
 * Direct wrapper for mysql_fetch_field_direct(@result,@fieldpos);.
 *
 * Returns: Name of a field in recordset.
 */
gchar *
db_dbfieldname(DbRecordSet * result, gint fieldpos)
	{
	MYSQL_FIELD *field;
	field = mysql_fetch_field_direct(result->res, fieldpos);
	if (field)
		return field->name;
	return NULL;
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
gint
db_dbuniqueid(DbRecordSet * result)
	{
	warningmsg("Function not implimented in mysql");
	return NULL;	
	}

#endif
