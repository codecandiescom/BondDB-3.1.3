#include <glib.h>
#include <stdarg.h>

#include "dbwrapper.h"

#ifdef _GDA

#include "bc.h"
#include "dbgda.h"
#include "dbuniqueid.h"
#include "dbsqlparse.h"

/*===========================================================================================
 * Connection based functions.
 *===========================================================================================
 */

/**
 * db_dbconnect:
 * @initstring: libgda initilising string
 * 
 * Direct wrapper for gda_client_open_connection_from_string(@initstring).  
 * 
 * Returns: Connection to database.
 */
DbConnection *
db_dbconnect(gchar * initstring, gchar *provider)
	{
	static char gda_application_initalised = 0;	/* if connection established */
	DbConnection *ret;
	gchar *newconnectstr;
	gchar providerdefault[] = "postgresql";
	
	/* If no provider mentoned default to postgresql. */
	if (provider == NULL)
		provider = providerdefault;
	/* Strip and tidy up the init string for database conncetion */
	g_assert(initstring);
	if (gda_application_initalised == 0)
		{
		gda_init("Bonddb","0.8",0,NULL);
	 	gda_application_initalised = 1;
		}
	
	newconnectstr = g_strdelimit(initstring, " \t\r\n", ' ');
	newconnectstr = g_strstrip(newconnectstr);
	
	ret = (DbConnection *) mem_alloc(sizeof(DbConnection));
	ret->connstring = mem_strdup(newconnectstr);
	ret->dbserver = DB_GDA;
	ret->client = gda_client_new();
	ret->provider = provider;
		
	ret->conn = gda_client_open_connection_from_string(
					ret->client,provider,newconnectstr,
					GDA_CONNECTION_OPTIONS_READ_ONLY);
	if (!GDA_IS_CONNECTION(ret->conn)) 
		{
        errormsg("Could not open connection from string `%s', using provider `%s'\n"
					   "",
                        newconnectstr, provider);
		
		mem_free(ret->connstring);
		mem_free(ret);
        return NULL;
        }
	debugmsg("Connected ok!");
	return ret;
	}

/**
 * db_dbstatus:
 * @conn: Database Connection
 * 
 * Direct wrapper for gda_connection_is_open
 *
 * Returns: status, a non-zero on failure
 */
gint
db_dbstatus(DbConnection * conn)
	{
	g_assert(conn);
	if (!gda_connection_is_open(conn->conn))
		return 1;
	return 0;
	}

/**
 * db_dberrormsg:
 * @conn: Database Connection
 * 
 * Calls gda_connection_get_errors and rolls the list into a string  
 *
 * Returns: gchar* not to be freed.
 */
gchar *
db_dberrormsg(DbConnection * conn)
	{
	GList *errors, *walk;
	gchar *retstr=NULL, *tmpbuf;
	GdaError *err;

	g_assert(conn);
	errors = gda_error_list_copy(gda_connection_get_errors(conn->conn));
	/* cat error messages together */
	for (walk = errors; walk != NULL; walk = walk->next)
		{
		err = walk->data;
		if (retstr)
			{
			tmpbuf = mem_strdup_printf("%s %s\n", retstr,
		                      gda_error_get_description(err));
			mem_free(retstr);
			}
		else
			tmpbuf = mem_strdup_printf("%s\n", 
							  gda_error_get_description(err));
		retstr = tmpbuf;
		}
	gda_error_list_free(errors);

	return retstr;
	}

/**
 * db_dbfinish:
 * @conn: Database Connection
 * 
 * Direct wrapper for gda_connection_close(@conn).
 */
void
db_dbfinish(DbConnection * conn)
	{
	g_assert(conn);
	gda_client_close_all_connections (conn->client);
	g_object_unref(G_OBJECT(conn->client));
	mem_free(conn->connstring);
	mem_free(conn);
	gda_main_quit();
	}

/**
 * db_dbreset:
 * @conn: Database Connection
 * 
 *  This function is not essential and i dont think it is 
 * even called anywhere. So no need to make use of it.
 * hey, I wont even write a libgda wrapper for it then.
 */
void
db_dbreset(DbConnection * conn)
	{
	errormsg("Not implemented for libgda");
	}

/*===========================================================================================
 * Result based functions.
 *===========================================================================================
 */

/**
 * db_dbcheckresult:
 * @result: Query result
 * 
 * checks to see if the recordset is null, or
 * if the GdaResultSet is null
 *
 * 
 * 
 * Returns: non-zero on error
 */
gint
db_dbcheckresult(DbRecordSet * result)
	{
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

	return 0;
	}

/**
 * db_dbclear:
 * @result: Query result
 * 
 * Direct wrapper for gda_recordset_free(@result).
 */
void
db_dbclear(DbRecordSet * result)
	{
	if (result == NULL)
		return ;
	gda_command_free(result->command);
	g_object_unref(result->res);
/*	gda_recordset_free(result->res); */
	mem_free(result->query);
	mem_free(result);
	result = NULL;
	}

/**
 * db_dbexec:
 * @conn: Database connection
 * @query: Query string
 * 
 * Direct wrapper for gda_connection_exec(@conn, @query).
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
	/* build up command */
	result->command = gda_command_new(result->query,
		GDA_COMMAND_TYPE_SQL, 
		GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	/* Execute the query */
	result->res = gda_connection_execute_single_command(
		conn->conn, result->command, NULL);

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
	return gda_data_model_get_n_rows (result->res);
	}

/**
 * db_dbgetvalue:
 * @result: Result from query
 * @row: Position in recordset
 * @field: Field position
 * 
 * moves to the row, gets the value, moves back
 *
 * Returns: String of field
 */
gchar *
db_dbgetvalue(DbRecordSet * result, gint row, gint column)
	{
	GdaValue *value;

	value = (GdaValue *) gda_data_model_get_value_at (
		result->res, column, row);
	
	if (value == NULL)
		{
		errormsg("Tried to move to an invalid row (%d) or column (%d) in the recordset",row,column);
		return NULL;
		}
	return gda_value_stringify(value);
/*	if (gda_recordset_move(result->res, row) == GDA_RECORDSET_INVALID_POSITION)
		{
		errormsg("Tried to move to an invalid row in the recordset");
		return null;
		}
	gfield = gda_recordset_field_idx(result->res, field);

	gda_recordset_move(result->res, -row); */
	/* try this, not sure if it will only work if the field is a string, or if
	   it will convert to String auto like */ 
	/*return gda_stringify_value(NULL, 0, gfield); */
	}

/**
 * db_dbnumfields:
 * @result: Result from a query
 * 
 * Direct wrapper for gda_recordset_rowsize(@result);.
 *
 * Returns: Number of fields in this row of the @result.
 */
gint
db_dbnumfields(DbRecordSet * result)
	{
	g_assert(result);
	g_assert(result->res);
	return gda_data_model_get_n_columns(result->res);
	}

/**
 * db_dbfieldname:
 * @result: Result from a query
 * @fieldpos: Field position
 * 
 * gets the specified field from the current row and returns its name
 *
 * Returns: Name of a field in recordset.
 */ 
gchar *
db_dbfieldname(DbRecordSet * result, gint fieldpos)
	{
#warning "Possible memory leak"
	return (gchar*)gda_data_model_get_column_title (result->res,fieldpos);
	}

/**
 * db_dbuniqueid:
 * @result: Result from a insert
 * @fieldpos: Field position
 * 
 * Direct wrapper for PQoidValue(@result);.
 * Assuming you run am insert query with db_dbexec(), this will return the unique id assocated with
 * that record.  Note this works on just getting the oid value from postgresql, on other databases
 * a bit of a work around will be needed.
 *
 * Returns: Name of a field in recordset.
 */
glong
db_dbuniqueid(DbConnection *conn, DbRecordSet * result)
	{
	gchar *value;
	glong oid;
	
	g_assert(conn);
	g_assert(result);
	g_assert(result->res);	
	value = gda_connection_get_last_insert_id (conn->conn, result->res);
	oid = atol(value);
	g_free(value);
	return oid;
	}

#endif
