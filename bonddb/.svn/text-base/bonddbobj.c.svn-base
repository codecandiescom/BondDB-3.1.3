/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include "bonddbobj.h"
#include "bonddbkey.h"
#include "bonddbrecord.h"
#include "bonddbfield.h"
#include "bonddbid.h"
#include "bonddbwrite.h"
#include "bonddbparse.h"
#include "bonddbloadby.h"
#include "bonddbfilter.h"
#include "sql_parser.h"
#include "bc.h"

gint bonddbclasscounter;

/**
 * bonddb_setstate()
 * @param obj :
 * @param state :
 *
 * Sets the state of a database @obj
 */
void
bonddb_setstate(BonddbClass *obj, BonddbClassState state)
{
	obj->state = state;
}

/**
 * bonddb_getstate()
 * @param obj :
 *
 * @return  The state of a database @obj
 */
BonddbClassState
bonddb_getstate(BonddbClass *obj)
{
	return obj->state;
}
BonddbClassMode
bonddb_getmode(BonddbClass *obj)
{
	return obj->mode;
}
gboolean
bonddb_isnewrecord(BonddbClass *obj)
{
	if (obj->mode == BONDDB_INSERT)
		return TRUE;
	return FALSE;
}
gboolean
bonddb_geteditable(BonddbClass *obj)
{
	if (obj->mode == BONDDB_INSERT)
		return TRUE;
	return obj->editable;
}
void
bonddb_seteditable(BonddbClass *obj, gboolean editable)
{
	obj->editable = editable;
}

/**
 * bonddb_new_full()
 * @param bonddb :
 * @param conn_name : connection to use, or NULL for default connection
 *
 * Creates a new database object and sets its state to INVALID
 *
 * @return  New Object
 */
BonddbClass *
bonddb_new_full(BonddbGlobal *bonddb, gchar *conn_name)
{
	BonddbClass *obj;
	g_assert(bonddb);
	obj = (BonddbClass*) mem_calloc(sizeof(BonddbClass));
	obj->bonddb = bonddb;
	obj->objectinstance = bonddbclasscounter++;
	obj->editable = TRUE;
	bonddb_setstate(obj,BONDDB_INVALID);
	bonddb->alloclist = g_list_prepend(bonddb->alloclist, obj); 
	if(conn_name)
		{
		/* Check to see if the connection exists */
		BonddbConnection *bcon = g_hash_table_lookup(bonddb->oo_conns, conn_name);
		/* If it's already there, increase the reference count */
		if(bcon)
			bonddb_connection_ref(bcon);
		/* If not, we add it */
		else
			{
			/* The intention of the one of connections is to allow the application to do 
			 * something in a transaction; the only purpose of this is if we intend to write.
			 * Therefore, this should use the writeconnection if one is defined (otherwise
			 * we are writing locally anyway) */
			if(bonddb->connectionstr_remote_write)
				bcon = bonddb_connection_new(bonddb, bonddb->connectionstr_remote_write, bonddb->provider, bonddb->l->notices);
			else
				bcon = bonddb_connection_new(bonddb, bonddb->connectionstr, bonddb->provider, bonddb->l->notices);
			if(!bcon)
				{
				bonddb_free(obj);
				return NULL;
				}
			g_hash_table_insert(bonddb->oo_conns, conn_name, bcon);
			}
		obj->conn = bcon->conn;
		obj->conn_name = mem_strdup(conn_name);
    }
	else
		obj->conn=bonddb->conn;
	obj->currentid = NULL;
	return obj;
}

/**
 * bonddb_new_sql()
 * @param bonddb :
 * @param sql :
 * @param run :
 *
 * Creates a new database object, and sets the state to NEW.
 *
 * @return  A new database object or %NULL on error
 * 
 * 
 */
BonddbClass * bonddb_new_sql_full(BonddbGlobal *bonddb, gchar *sql, gboolean run, gchar **errmsg, gchar *conn_name)
{
	BonddbClass *obj;

	obj = bonddb_new_full(bonddb, conn_name);
	obj->admin = FALSE;
	bonddb_setstate(obj,BONDDB_NEW);
	if (bonddb_sql(obj, sql, run, errmsg) != 0)
		{
		bonddb_free(obj);
		return NULL;
		}
	return obj;
}

BonddbClass * bonddb_new_sql_withadmin_full(BonddbGlobal *bonddb, gchar *sql, gboolean run, gboolean admin, char **errmsg, gchar *conn_name)
{
	BonddbClass *obj;

	obj = bonddb_new_full(bonddb, conn_name);
	if(!obj)
		return NULL;
	obj->admin = admin;
	bonddb_setstate(obj,BONDDB_NEW);
	if (bonddb_sql(obj, sql, run, errmsg) != 0)
		{
		bonddb_free(obj);
		return NULL;
		}
	return obj;
}

/**
 * bonddb_new_basetable()
 * @param bonddb :
 * @param basetable :
 *
 * Creates a reference from firsttable in the database obj to the basetable 
 * passed in.
 *
 * @return  database object referencing the bastable
 */
BonddbClass * bonddb_new_basetable_full(BonddbGlobal *bonddb,  gchar *basetable, gchar **errmsg, gchar *conn_name)
{
	BonddbClass *obj;
	gchar *query;
	g_assert(basetable);
	query = mem_strdup_printf("SELECT * FROM %s",basetable);
	obj = bonddb_new_sql_full(bonddb,query,FALSE,errmsg, conn_name);
	bonddb_setfirsttable(obj,basetable);
	bonddb_setname(obj,basetable);
	bonddb_field_populatebytablename(obj, basetable, 0);
	mem_free(query);
	return obj;
}

/**
 * bonddb_setfirsttable
 * @param obj :
 * @param firsttable :
 *
 * Sets the firsttable of the object
 */
void bonddb_setfirsttable(BonddbClass *obj, gchar *firsttable)
{
	g_assert(obj);
	if(obj->firsttable != NULL)
		{
		mem_free(obj->firsttable);
		}
	if(firsttable == NULL)
		obj->firsttable = NULL;
	else
		obj->firsttable = mem_strdup(firsttable);
}

/**
 * bonddb_setname()
 * @param obj :
 * @param name :
 *
 * Sets the name of the object
 */
void
bonddb_setname(BonddbClass *obj, gchar *name)
{
	g_assert(obj);
	if (obj->name != NULL)
		mem_free(obj->name);
	obj->name = mem_strdup(name);
}

/**
 * bonddb_sql_internal_exec()
 * @param obj :
 * @param errormsg : any error messages from the database are stored here
 *
 * Executes the query in the obj and checks the result set for errors.
 *
 * @return  non-zero on failure, errmsg may get a pointer to memory that must be freed.
 */
static gint
bonddb_sql_internal_exec(BonddbClass *obj, gboolean writeaction, gchar **errmsg)
{
	void *conn = obj->conn;
	if (errmsg) *errmsg = NULL;
	
	/* Don't switch things if we have a one of database connection */
	if(!obj->conn_name)
		{
		/* Are you sure this is required? The obj should already have the right
		 * connection specified */
		if (writeaction)
			{
			if(obj->admin)
				{
				if(NULL != obj->bonddb->conn_remote_write_admin)
					conn = obj->bonddb->conn_remote_write_admin;
				else
					{
					*errmsg = mem_strdup("Remote There is no administrator configured; please ensure that db_username_admin and db_password_admin are configured correctly in the application configuration file.");
					errormsg(*errmsg);
					bonddb_setstate(obj,BONDDB_INVALID);
					obj->bonddb->l->_db_clear(obj->res);
					obj->res = NULL;
					return -20;
					}
				}
			else
				conn = obj->bonddb->conn_remote_write;
			}
		else if (obj->admin)
			{
			if(NULL != obj->bonddb->conn_admin)
				conn = obj->bonddb->conn_admin;
			else
				{
				*errmsg = mem_strdup("There is no administrator configured; please ensure that db_username_admin and db_password_admin are configured correctly in the application configuration file.");
				errormsg(*errmsg);
				bonddb_setstate(obj,BONDDB_INVALID);
				obj->bonddb->l->_db_clear(obj->res);
				obj->res = NULL;
				return -20;
				}
			}
		}

/*	debuglog(75,"internal-exec %s//\n%s\n------------------------",
		writeaction?"REMOTE":"local",g_strstrip(obj->query)); */
	/* Sent query to sql exec thing */
	obj->res = obj->bonddb->l->_db_exec(conn, obj->query);
	/* check for errors */
	if (obj->bonddb->l->_db_checkerror(obj->bonddb->l, conn, obj->res,
				errmsg) != 0)
		{
		errormsg("SQL Failed with %s\nOn: %s",*errmsg,obj->query);	
		bonddb_setstate(obj,BONDDB_INVALID);
		obj->bonddb->l->_db_clear(obj->res);
		obj->res = NULL;
		return -10;
		}
	return 0;
}
/* check the query sql */
static void
bonddb_sql_internal_query(BonddbClass *obj)
{
	gchar *tmpstr;
	if (obj->generatequery == FALSE)
		return;
	if (obj->query != NULL)
		mem_free (obj->query);
	tmpstr = sql_stringify(obj->statement);
	obj->query = mem_strdup(tmpstr);
	mem_free(tmpstr);
}

/**
 * bonddb_sql_internal()
 * @param obj :
 * @param activate :
 * @param errmsg : store sql error messages here
 * 
 * @return  non-zero on failure
 */
static gint 
bonddb_sql_internal(BonddbClass *obj, gboolean activate, gboolean writeaction, gchar **errmsg)
{
	gint retval = 0;
	void *conn = obj->conn;

	/* Don't switch things if we have a one of database connection */
	if(!obj->conn_name)
		{
		if (writeaction)
			{
			if(obj->admin)
				{
				if(NULL != obj->bonddb->conn_remote_write_admin)
					conn = obj->bonddb->conn_remote_write_admin;
				else
					{
					*errmsg = mem_strdup("Remote There is no administrator configured; please ensure that db_username_admin and db_password_admin are configured correctly in the application configuration file.");
					errormsg(*errmsg);
					bonddb_setstate(obj,BONDDB_INVALID);
					obj->bonddb->l->_db_clear(obj->res);
					obj->res = NULL;
					return -20;
					}
				}
			else
				conn = obj->bonddb->conn_remote_write;
			}
		else if (obj->admin)
			{
			if(NULL != obj->bonddb->conn_admin)
				conn = obj->bonddb->conn_admin;
			else
				{
				*errmsg = mem_strdup("There is no administrator configured; please ensure that db_username_admin and db_password_admin are configured correctly in the application configuration file.");
				errormsg(*errmsg);
				bonddb_setstate(obj,BONDDB_INVALID);
				obj->bonddb->l->_db_clear(obj->res);
				obj->res = NULL;
				return -20;
				}
			}
		}
		
		bonddb_sql_internal_query(obj);
			
		if ((retval = bonddb_sql_internal_exec(obj, writeaction, errmsg)) != 0)
			return retval;
		
	/* short cut out of here */
	if (activate == FALSE)
		return 0;

	obj->mode = BONDDB_READ;
	obj->currentrow = 0;
	/* If it wasn't a select statement */
	if (obj->bonddb->l->_db_tuples_ok(conn,obj->res) <= 0)
		{
		bonddb_setstate(obj,BONDDB_RAW);
		obj->numrecord = 0;
		obj->currentid = NULL;
		obj->tmpid = NULL;
		return 0;
		}
	else
		{
		bonddb_setstate(obj,BONDDB_READ_RAW);
		obj->numrecord = obj->bonddb->l->_db_numtuples(obj->res);
		}
	
	/* Handle record set */
	if (bonddb_field_populatebyresult(obj))
		{
		errormsg("Failed to do field parsing object");
		bonddb_setstate(obj,BONDDB_INVALID);
		obj->bonddb->l->_db_clear(obj->res);
		obj->res = NULL;
		return -11;
		}
	if (obj->state == BONDDB_READ_RAW)
		obj->currentrecord = bonddb_record_populate(obj, 0);
	
	return retval;
	}
	
/**
 * bonddb_sql()
 * @param obj :
 * @param sql :
 * @param runnow :
 * 
 * Stores the sql query passed in to the database obj. 
 * Executes the sql query if runnow is TRUE.
 * 
 * @return  non-zero on failure
 */
gint 
bonddb_sql(BonddbClass *obj, gchar *sql, gboolean runnow, gchar **errmsg)
{
	if(errmsg)
		*errmsg = NULL;

	g_assert(obj);
	g_assert(sql);
	if (obj->query || obj->res)
		bonddb_clear(obj);
	obj->query = mem_strdup(sql);
	if (runnow)
		return bonddb_sql_internal(obj,TRUE,FALSE, errmsg);
	return 0;
}

static gint sql_runitnow(
	gboolean writeaction,gboolean admin,BonddbGlobal *bonddb, gchar *query, gchar **errmsg)
{
	void *conn;
	gchar *tmp = NULL;

	if(!errmsg) 
		errmsg=&tmp;

	if(writeaction)
		{
		if(admin)
			{
			if(NULL != bonddb->conn_remote_write_admin)
				conn = bonddb->conn_remote_write_admin;
			else
				{
				*errmsg = mem_strdup("Remote There is no administrator configured; please ensure that db_username_admin and db_password_admin are configured correctly in the application configuration file.");
				errormsg(*errmsg);
				return -20;
				}
			}
		else
			conn = bonddb->conn_remote_write;
		}
	else if (admin)
		{
		if(NULL != bonddb->conn_admin)
			conn = bonddb->conn_admin;
		else
			{
			*errmsg = mem_strdup("There is no administrator configured; please ensure that db_username_admin and db_password_admin are configured correctly in the application configuration file.");
			errormsg(*errmsg);
			return -20;
			}
		}
	else
		conn = bonddb->conn;

	*errmsg = NULL;
	g_assert(bonddb);
	g_assert(query);

	void *res = bonddb->l->_db_exec(conn, query);
	
	if ( bonddb->l->_db_checkerror(bonddb->l, conn, res, errmsg) != 0)
		{
		/* if caller ignores errmsg print it */
		if(tmp) errormsg("SQL Failed with %s\nOn: %s",*errmsg,query); 
		bonddb->l->_db_clear(res);
		mem_free_nn(tmp);
		return -10;
		}
	bonddb->l->_db_clear(res);
	mem_free_nn(tmp);
	return 0;
}

gint 
bonddb_sql_exec(BonddbGlobal *bonddb, gchar *sql, gboolean admin, gchar **errmsg)
{ return sql_runitnow(FALSE,admin,bonddb,sql,errmsg);}
gint
bonddb_sql_exec_write(BonddbGlobal *bonddb, gchar *sql, gboolean admin, gchar **errmsg)
{ return sql_runitnow(TRUE,admin,bonddb,sql,errmsg);}


/**
 * bonddb_sql_runnow()
 * @param obj :
 * @param errmsg : location to store sql error messages
 * 
 * Checks if the obj has executed the query, if it hasn't, 
 * then execute the query now.
 * 
 * @return  non-zero on failure
 */
gint 
bonddb_sql_runnow(BonddbClass *obj, gchar **errmsg)
	{
	gint retval;
	if(errmsg)
	    *errmsg = NULL;
	g_assert(obj);
	g_assert(obj->query);
	if (obj->res != NULL)
		return -1;
	retval = bonddb_sql_internal(obj,TRUE, FALSE, errmsg);
	return retval;
	}
/**
 * bonddb_sql_runnow_write()
 * @param obj :
 * 
 * Checks if the obj has executed the query, if it hasn't, 
 * then execute the query now.  Runs exection on remote server
 * which is master replication
 * 
 * @return  non-zero on failure
 */
gint 
bonddb_sql_runnow_write(BonddbClass *obj, gchar**errmsg)
	{
	gint retval;
	if(errmsg)
	    *errmsg = NULL;
	g_assert(obj);
	g_assert(obj->query);
	if (obj->res != NULL)
		return -1;
	obj->conn=obj->bonddb->conn_remote_write; /* use remote connection as source */
	retval = bonddb_sql_internal(obj,TRUE,TRUE,errmsg);
	return retval;
	
	}
	
static void
bonddb_parsesql_free(BonddbClass *obj)
{
	if (obj->statement != NULL)
	{
		sql_destroy(obj->statement);
		obj->statement = NULL;
	}
	if (obj->tablelist)
	{
		sql_statement_free_tables(obj->tablelist);
		obj->tablelist = NULL;
	}
	bonddb_setfirsttable(obj,NULL);
	obj->sqlgood = FALSE;
}
/**
 * bonddb_sql_runnow()
 * @param obj :
 * 
 * Free the memory allocated for the result set and 
 * free the memory allocated for the query if it hasn't been freed.
 *	
 * @return  0 on success
 */
gint 
bonddb_clear(BonddbClass *obj)
{
	gint retval=0;
	gchar *errmsg=NULL;
	g_assert(obj);
	if (obj->autosave)
		retval = bonddb_save(obj,&errmsg);
	if (errmsg)
		errormsg("Failed to save record. \n%s",errmsg);
	mem_free(errmsg);
	if (obj->tmpid)
		bonddb_id_release_tmp(obj);
	bonddb_parsesql_free(obj);
	bonddb_filter_clear(obj);
	bonddb_record_freeall(obj);
	g_assert(obj->record == NULL);
	bonddb_write_freelist(obj);
	if (obj->res)
		obj->bonddb->l->_db_clear(obj->res);
	obj->res = NULL;
	if (obj->query)
		mem_free(obj->query);
	obj->query = NULL;
	obj->generatequery = FALSE;
	obj->currentrow = 0;
	obj->currentid = NULL;
	obj->tmpid = NULL;
	obj->sqlgood = FALSE;
	obj->ignoreassert = FALSE;
	obj->noprimarykey = FALSE;
	return retval;
}

/**
 * bonddb_sql_runnow()
 * @param obj :
 * 
 * Free's the memory allocated for the object and its properties
 * 
 * @return  Zero on no error, else if an error occurred if it was
 * suppose to save with autosave set, then it returns a string
 * which needs freeing which contains the errormsg.
 */
gint 
bonddb_free(BonddbClass *obj)
{
	gint retval = 0;
	if (obj == NULL)
		return -1;
	retval = bonddb_clear(obj);
	bonddb_parsesql_free(obj);
	bonddb_primarykey_free(obj);
	if (obj->fieldhash)
		g_hash_table_destroy(obj->fieldhash);
	if (obj->birthhash)
		g_hash_table_destroy(obj->birthhash);
	bonddb_ref_clear(obj);

	bonddb_setfirsttable(obj,NULL);
	if (obj->name)
		mem_free(obj->name);
	if (obj->query)
		mem_free(obj->query);
	if (g_list_length(obj->bonddb->alloclist) == 1)
	{
		g_list_free(obj->bonddb->alloclist);
		obj->bonddb->alloclist = NULL;
	}
	else
		obj->bonddb->alloclist = g_list_remove(
			obj->bonddb->alloclist,obj);
	/* Clean up if a one of connection has been used */
	if(obj->conn_name)
		{
		BonddbConnection *bcon = g_hash_table_lookup(obj->bonddb->oo_conns, obj->conn_name);
		if(bcon)
			{
			if(bonddb_connection_unref(bcon))
				g_hash_table_remove(obj->bonddb->oo_conns, obj->conn_name);
			}
		else
			warningmsg("Connection %s cannot be located to close.", obj->conn_name);
		mem_free(obj->conn_name);
		}
	mem_free(obj);
	return retval;
}

gint 
bonddb_parsesql(BonddbClass *obj)
{
	g_assert(obj);
	g_assert(obj->query);
	bonddb_parsesql_free(obj);
	
	bonddb_parse_tablelist(obj);
	if (obj->tablelist)
	{
		bonddb_setfirsttable(obj,obj->tablelist->data);
		return 0;
	}
	return -1;
}
/**
 * bonddb_parsesql_proper()
 * @param obj :
 * @param addoid : 
 *
 * Parses the sql query.
 * 
 * @return  0 on success
 */
gint 
bonddb_parsesql_proper(BonddbClass *obj, gboolean addoid)
{
	g_assert(obj);
	g_assert(obj->query);
	bonddb_parsesql_free(obj);
	g_assert(obj->query);
	obj->statement = sql_parse(obj->query);
	if (obj->statement)
	{
		obj->sqlgood = TRUE;
	}
	else
	{
		errormsg("\nSQL parse failed for query %s",obj->query);
		return -1;
	}
	obj->tablelist = sql_statement_get_tables(obj->statement);
	if (obj->tablelist)
		bonddb_setfirsttable(obj,obj->tablelist->data);
	return 0;
}

static void
bonddb_add_applybirth(void *key, void *value, void *ptr)
{
	gchar *leftfield = key, *rightvalue = value;
	gchar *tablename, *fieldname;
	BonddbClass *obj = ptr;
	gint i, j, slen;
	slen = strlen(leftfield);
	/* break tablename.fieldname up to tablename and fieldname */
	tablename = leftfield;
	fieldname = (gchar*)mem_alloc(slen+1);
	for (i=0;i<slen;i++)
		if (tablename[i] == '.')
		{
			tablename[i] = 0;
			break;
		}
	i = i+1;
	for (j=i;j<slen;j++)
		fieldname[j-i] = leftfield[j];
	fieldname[j-i] = 0;
	
	debugmsg("applying default of %s.%s=%s",tablename,fieldname,(char*)value); 
	bonddb_setvalue(obj, tablename, fieldname,rightvalue,FALSE);
	/* Fix what breakage we did */
	leftfield[i-1] = '.';
	mem_free(fieldname);
}

/**
 * bonddb_add()
 * @param obj :
 * @param tablename :
 *
 * Adds another record onto the end of a database object.  
 * 
 * @return  non-zero on failure
 */
gint 
bonddb_add(BonddbClass *obj, gchar *tablename)
{
	g_assert(obj);
	g_assert(tablename);

	if (obj->state == BONDDB_INVALID)
		return -1;
	if (obj->fieldhash == NULL && obj->name == NULL)
	{
		/* A bonddb_add is called on a fresh unknown object,
		 * make yourself known to it like you would in 
		 * bonddb_new_basetable(); */
		bonddb_setfirsttable(obj,tablename);
		bonddb_setname(obj,tablename);
		bonddb_field_populatebytablename(obj, tablename, 0);
	}
	g_assert(obj->fieldhash);

	obj->mode = BONDDB_INSERT;
	obj->currentrow = obj->numrecord;
	obj->numrecord += 1;

	obj->currentrecord = bonddb_record_populate(obj,obj->currentrow);
	bonddb_field_default(obj,tablename);
	/* Get the ID for the new record */
	/*
	obj->currentrecord->id = bonddb_id_get(obj, obj->firsttable, obj->currentrow);
	obj->currentid = obj->currentrecord->id;
	*/

	if (obj->birthhash)
		g_hash_table_foreach(obj->birthhash,bonddb_add_applybirth,obj);
	return 0;
}

/* bonddb_moveto_id:
 * @param obj : database object
 * @param id : BonddbId to try to move to
 *
 * Try to move to the row containing a record with the same id as 
 * @param id :.
 *
 * @return  non-zero on failure
 */
gint bonddb_moveto_id(BonddbClass *obj, BonddbId *id)
	{
	gint i;

	g_assert(obj);
	g_assert(id);

	if(obj->record)
		{
		/* Go through the records, check for a match */
		for(i = 0; i < obj->numrecord; i++)
			{
			/* The getvalue functions operate on the current row, so in order to
			 * populate this stuff, we need to move to it first :-/ */
			obj->currentrow = i;
			if(!(obj->record[i] && obj->record[i]->id) && obj->state >= BONDDB_READ_RAW)
				{
				obj->mode = BONDDB_READ;
				bonddb_record_populate(obj, i);
				if(obj->firsttable)
					obj->record[i]->id = bonddb_id_get(obj, obj->firsttable, i);
				}
			if(obj->record[i] && obj->record[i]->id && bonddb_id_is_equal(obj->record[i]->id, id))
				{
				/* We've found the right record - move here */
				obj->currentrecord = obj->record[i];
				obj->mode = obj->currentrecord->mode;
				obj->currentid = obj->currentrecord->id;
				return 0;
				}
			}
		}
	return -1;
	}

/**
 * bonddb_moveto()
 * @param obj : database object
 * @param row : new row to move to.
 *
 * Move to a new row position in the database object recordset.  This will set 
 * the obj->currentrow as well as update everything else in the object to 
 * point to  the new row.  
 *
 * You can move to records yet to be written as well as back to 
 * prevously added records.  If the @obj has been filtered previously this 
 * function will look up the correct corresponding row for its filtered 
 * position and move to that row.
 *
 * @return  non-zero on failure.
 */
gint 
bonddb_moveto(BonddbClass *obj, gint row)
{
	g_assert(obj);
	/* ignoreassert is used to ignore asserts and assume everything is ok */
	if (obj->ignoreassert == FALSE)
	{
		if (row < 0 || row >= obj->numrecord)
		{
			errormsg("Attempting to move outside of bounds "
				"(0-%d) %d", obj->numrecord,row);
			return -1;		
		}
		/* Error messages handled by assert function */
		if (bonddb_assert_moveto(obj))
			return -2;
	}
	/* Sometimes results can be filtered. */
	if (obj->filterhash)
	{
		row = bonddb_filter_moveto(obj, row);
		if (row < 0)
			return -3;
	}
	if (obj->currentrow != row && obj->tmpid)
		bonddb_id_release_tmp(obj);
	obj->currentrow = row;
	/* record already visited */
	if (obj->record && obj->record[row] != NULL)
	{
		obj->currentrecord = obj->record[row];
		obj->mode = obj->currentrecord->mode;
		obj->currentid = obj->currentrecord->id;
/*		if (obj->currentid)
		{
		m.essage("moving id to %d. %s\n%s",row,
			obj->currentid->primarykeys[0],obj->query);
		} */
	}
	else
	if (obj->state >= BONDDB_READ_RAW)
	{
		obj->mode = BONDDB_READ;
		obj->currentrecord = 
			bonddb_record_populate(obj,obj->currentrow);
		if (obj->firsttable)
			obj->currentrecord->id = bonddb_id_get(obj, obj->firsttable, obj->currentrow);
		obj->currentid = obj->currentrecord->id;
	}
	else
	{
		errormsg("State does not allow navigation on %s", obj->name);
		return -3;
	}
	return 0;
}	

/**
 * bondb_numrecord()
 * @param obj : Database Object
 *
 * Returns the number of records in a obj
 *
 * @return  the integer number of rows in a obj dataset
 */
gint 
bonddb_numrecord(BonddbClass *obj)
{
	g_assert(obj);
	if (obj->filterhash)
		return bonddb_filter_numrecord(obj);
	return obj->numrecord;
}

/**
 * bondb_setprimarykey()
 * @param obj : Database Object
 * @param tablename :
 * @param primarykey : A GList* of fields which will be the primary key.
 *
 * Sets the primary key of table @tablename to @primarykey
 *
 * @return  the primary key for table @tablename
 */
gint 
bonddb_setprimarykey(BonddbClass *obj, gchar *tablename, GList *primarykey)
{
	g_assert(obj);
	g_assert(tablename);
	return bonddb_primarykey_add(obj, tablename, primarykey);
}

/**
 * bondb_getvalue_assert()
 * @param obj : Database Object
 * @param fieldname : 
 *
 * Checks whether the obj is INVALID,in the correct position for inserting
 * or in the insert mode. Assertion fails if one of these is true
 *
 * @return  non-zero on failure
 */
static gint
bonddb_getvalue_assert(BonddbClass *obj, gchar *fieldname)
{
	gint retval;
	g_assert(obj);
	g_assert(fieldname);

	if (obj->state == BONDDB_INVALID)
		return -1;
	if ((retval = bonddb_assert_position(obj, obj->currentrow)) != 0)
		return retval - 10;
	if (obj->mode < BONDDB_INSERT)
		if (bonddb_assert_recordset(obj))
			return -2;
	if (fieldname == NULL)
		return -3;
	if (obj->currentrecord == NULL)
	{
		errormsg("No current record is assosciated with this record.");
		return -4;
	}
	return 0;
}


gint 
bonddb_getvaluebyindex(BonddbClass *obj, gint index,gchar **value)
	{
	*value = NULL;
	g_assert(obj);

	*value = obj->bonddb->l->_db_get_value(obj->res,obj->currentrow, 
			index);
	if(*value)
		g_strstrip(*value); 
	return !value;
}

/**
 * bonddb_getvalue()
 * @param obj : Database object
 * @param tablename :  A class to look up in, or NULL if the current class.
 * @param fieldname : Field name.
 * @param value : Where to set the string after i've got the value.
 *
 * Retreives information from the database or cache.
 *
 * @return  Non-zero on failure. Value should not be freed.
 */

gint 
bonddb_getvalue(BonddbClass *obj, gchar *tablename, gchar *fieldname, gchar **value)
{
	gint retval;
	BonddbField *f;

	*value = NULL;
	g_assert(obj);
	if (obj->ignoreassert == FALSE || obj->currentrow == 0)
		if ((retval = bonddb_getvalue_assert(obj, fieldname)))
			return retval;
	/* Get field position */
	f = bonddb_field_get(obj, tablename, fieldname);
	if (!f)
		f = bonddb_field_get(obj, NULL, fieldname);

	if (f == NULL)
	{
		bonddb_field_debug(obj);
		errormsg("Unable to find %s.%s in the object with query\n%s",
			tablename, fieldname, obj->query);
		return -3;
	}

	/* Check cache to see if value exists */
	retval = bonddb_record_read(obj, f->fieldposinsql, obj->currentrow, value);
	/* If insert or insert_saved mode, return successfully always
	 * as no record set present */
	/* Message successfully return the value */
	if (retval == 0 && (*value != NULL || obj->mode >= BONDDB_INSERT))
		{
		return 0;
		}
	
	if (obj->ignoreassert == FALSE)
	{
		/* If in insert mode and cache extraction failed then 
		 * bad stuff */
		if (retval != 0 && obj->mode >= BONDDB_INSERT)
		{
			errormsg("Unable to get value from insert, error %d "
				"returned.", retval);
			return retval - 20;
		}

		/* Extracting now from the sql result. */
		if (obj->res == NULL)
		{
			errormsg("Unable to get value. No database result "
				"set on %s.", obj->query);
			return -4;
		}
	}
	*value = obj->bonddb->l->_db_get_value(obj->res,obj->currentrow, 
			f->fieldposinsql);
	if (*value){ /* these functions modify the value in-place */
			g_strstrip(*value);
		   }
	if ((retval = bonddb_record_write(obj, f->fieldposinsql, 
			obj->currentrow, *value, value)))
		return retval - 30;
	
	return 0;
}

/**
 * bonddb_setvalue()
 * @param obj : Database object
 * @param tablename :  A class to look up in, or NULL if the current class.
 * @param fieldname : Field name of record to save it.
 * @param value : Where to set the string after i've got the value.
 * @param mark : Mark should normally set to %TRUE. Set to %FALSE if you are
 * writing default or temporary data that you only want saved back
 * if other things change.
 *
 * Toplevel function. Writes information to the database
 *
 * @return  non-zero on failure.
 */
gint 
bonddb_setvalue(BonddbClass *obj, gchar *tablename, gchar *fieldname, 
		gchar *value, gboolean mark)
{
	gint retval=0;
	gchar *tmpstr;
	BonddbField *f;
	g_assert(fieldname);
	if ((retval = bonddb_getvalue_assert(obj, fieldname)))
		return retval;
	/* Get field position */
	f = bonddb_field_get(obj, tablename, fieldname);
	if (f == NULL)
	{
		errormsg("Unable to find %s.%s in the object with query\n%s",
			tablename, fieldname, obj->query);
		return -3;
	}

	
	if (value)
		{
		/* Write to the cache */
		if ((retval = bonddb_record_write(obj, f->fieldposinsql, obj->currentrow, value, &tmpstr)))
			{
			/* error occurred */
			return retval - 20;
			}
		}
	if (obj->mode == BONDDB_READ)
		obj->mode = BONDDB_UPDATE;
	g_assert(obj->currentrecord);
	obj->currentrecord->mode = obj->mode;
	/* get the id */
	if (obj->currentrecord->id == NULL && obj->state >= BONDDB_READ_RAW
		&& obj->mode != BONDDB_INSERT)
		if ((obj->currentrecord->id = bonddb_id_get(obj, 
				tablename, obj->currentrow)) == NULL)
		{
			warningmsg("Failed to find a primary key for table %s. "
				"Changes to table wont be saved.",tablename);
			return -5;
		}
	obj->currentid = obj->currentrecord->id;
	
	/* Add write log */
	bonddb_write_add(obj, tablename, fieldname, value, mark);
	
	return 0;
}
/**
 * bonddb_save()
 * @param obj : Database object to save.
 * @param errormsg : if there is any error on update or insert this is it.
 *
 * Saves @obj and does any inserts, update or deletes that are needed.
 * Any errors are returned in @errormsg.
 * If ermmsg come back not NULL it should be freed (using mem_free).
 *
 * @return  Non-zero on error.
 */
gint 
bonddb_save(BonddbClass *obj, gchar **errormsg)
{
	gint retval = 0;
	
	g_assert(obj);
	g_assert(errormsg);
	*errormsg = NULL;

	retval = bonddb_write_all(obj,errormsg);
	if (retval == -10)
	{
		return -1;
	}
	return 0;
}
/* allows you to save records without modifying this first */
gint 
bonddb_forcemark(BonddbClass *obj)
{
	GList *walk;
	BonddbWrite *w;
	g_assert(obj);
	for (walk=g_list_first(obj->writequeue);walk!=NULL;walk=walk->next)
	{
		w = walk->data;
		w->mark = TRUE;
	}
	return 0;
}
gint 
bonddb_abortsave(BonddbClass *obj)
{
	g_assert(obj);
	bonddb_write_freelist(obj);
	return 0;
}

gint 
bonddb_setautosave(BonddbClass *obj, gboolean autosave)
{
	g_assert(obj);
	obj->autosave = autosave;
	return 0;
}

/**
 * db_loadall()
 * @param obj : Database object to load results into
 * @param errmsg : location to store sql error messages
 *
 * Load all fields in a table into an object. This is very similar to
 * bonddb_sql_runnow(). 
 *
 * @return  non-zero on failure
 */
gint 
bonddb_loadall(BonddbClass *obj, gchar **errmsg)
{
	gint retval;
	g_assert(obj);
	g_assert(obj->query);
	if (obj->res != NULL)
	{
		bonddb_record_freeall(obj);
		obj->bonddb->l->_db_clear(obj->res);
		obj->res = NULL;
	}
		
	retval = bonddb_sql_internal(obj,TRUE,FALSE,errmsg);
	return retval;
}


/*
gint bonddb_validate(BonddbClass *obj, GList **failures);
gint bonddb_delete(BonddbClass *obj, gboolean recusive);
*/


/*
BonddbClassState bonddb_getstate(BonddbClass *obj);
BonddbClassMode bonddb_getmode(BonddbClass *obj);
*/


/**
 * bonddb_assert_moveto()
 * @param obj : Database object
 *
 * Checks that we are moving to the correct position, if not abort.
 *
 * @return  non-zero on failure.
 */
gint 
bonddb_assert_moveto(BonddbClass *obj)
{
	if (obj->numcolumn <= 0)
		{
		errormsg("BonddbClass %s failed assert on moveto-> "
			"numcolumn <= 0",obj->name);
		return -1;
		}
	else
	if (obj->state == BONDDB_RAW || obj->state == BONDDB_INVALID)
		{
		errormsg("BonddbClass %s failed assert on moveto-> "
			"state is invalid or raw",obj->name);
		}
	return 0;
}

/**
 * bonddb_assert_recordset()
 * @param obj : Database object
 *
 * Checks that the state is RAW or the record set is %NULL, if so abort.
 *
 * @return  non-zero on failure.
 */
gint
bonddb_assert_recordset(BonddbClass *obj)
{
	if (obj->state <= BONDDB_RAW)
		{
		errormsg("Invalid class state of %d",obj->state);
		return -1;
		}
	if (obj->res == NULL)
		{
		errormsg("Record set is null");
		return -2;
		}
	return 0;
}

gboolean
bonddb_isrecordset(BonddbClass *obj)
{
	if (obj->state > BONDDB_RAW && obj->res)
		return TRUE;
	return FALSE;
}

/**
 * bonddb_assert_position()
 * @param obj : Database object
 *
 * Checks the position of the row. If there are no rows or the number of rows
 * is out of bounds, abort.
 *
 * @return  non-zero on failure.
 */
gint
bonddb_assert_position(BonddbClass *obj, gint row)
{
	if (obj->numrecord <= 0)
	{
		errormsg("No rows so can't get value. %s",obj->query);
		return -1;
	}
	if (row < 0 || row >= obj->numrecord)
	{
		errormsg("Row is outside of bounds, 0..%d %d",obj->numrecord,
				row);
		return -2;
	}
	return 0;
}

/**
 * bonddb_delete()
 * @param obj : 
 * @param recusive : If set to %TRUE it will look for forign references and 
 * delete them also.
 *
 */
gint 
bonddb_delete(BonddbClass *obj, gchar *tablename, gboolean recusive)
{
	gint retval=0;
	gchar *errormsg;
	g_assert(obj);
	bonddb_write_delete(obj, tablename, recusive);
	retval = bonddb_save(obj,&errormsg);
	if (errormsg)
		warningmsg("Failed to delete record. \n%s",errormsg);
	return retval;
}
/* Returns real row ignoring filters */
gint 
bonddb_realrow(BonddbClass *obj)
{
	return obj->currentrow;
}

gint 
bonddb_currentrow(BonddbClass *obj)
{
	g_assert(obj);
	if (obj->filterhash)
		return bonddb_filter_currentrow(obj);
	return obj->currentrow;
}

/**
 * bonddb_blank()
 * 
 * This is similar to bonddb_clear but leaves the query intact and
 * just empties all the results.
 */
gint
bonddb_blank(BonddbClass *obj,gchar **errmsg)
{
	gint retval;
	gchar *query;
	if (!obj)
		return -1;
	if (!obj->query)
		return -2;
	query = mem_strdup(obj->query);
	bonddb_clear(obj);
	retval = bonddb_sql(obj, query, FALSE, errmsg);
	mem_free(query);
	return retval;
}

void
bonddb_debug(BonddbClass *res)
{
	g_assert(res);
	debugmsg("%d: %s\n",res->objectinstance, res->query);
	bonddb_record_debug(res);
}

/**
 * bonddb_jumpto_id()
 * @param idval : id value to jump to in @jumpto
 * @param jumpto : object to move to the above id
 *
 * returns: 0 on success, non-zero otherwise
 */
gint 
bonddb_jumpto_id(gchar *idval, BonddbClass *jumpto)
	{
	gint i, j, fieldnum = 0, status = 0, numcolumn, numrow, oldrow;
	gint *fieldpos;
	GList *walk;
	BonddbId *id = jumpto->currentid;
	gchar *value;

	g_assert(jumpto);
	g_assert(idval);

	/* Try to get an ID if we don't have one */
	if(!id)
		id = bonddb_id_get_currentrow(jumpto, jumpto->firsttable);

	if(!id)
		{
		errormsg("Unable to determine the id to jump to; aborting.");
		return -1;
		}

	numcolumn = jumpto->bonddb->l->_db_numcolumn(jumpto->res);
	fieldpos = mem_calloc(id->num * sizeof(gint));
	walk = g_list_first(id->fieldlist);
	for (j=0;j<id->num;j++)
		{
		status = 1;
		for (i=0;i<numcolumn && status;i++)
			{
			g_assert(walk);
			value = jumpto->bonddb->l->_db_fieldname(jumpto->res, i);
			g_assert(value);
			if (strcmp(value, (gchar*)(walk->data)) == 0)
				{
				fieldpos[fieldnum++] = i;
				status = 0;
				}
			}
		if (status == 1)
			{
			errormsg("Field %s not found", (gchar*)(walk->data));
			mem_free(fieldpos);
			return -1;
			}
		walk = walk->next;
		}

	/*find the result whith the id*/
	numrow = bonddb_numrecord(jumpto);
	/* for each row in the result */
	oldrow = jumpto->currentrow;
	status = 1;
	for (i=0;i<numrow;i++)
		{
		bonddb_moveto(jumpto, i);
		/* for each primary field in id */
		for (j=0; j<id->num && status; j++)
			{
			bonddb_record_read(jumpto, fieldpos[j], i, &value);
			if (!value)
				value = jumpto->bonddb->l->_db_get_value(jumpto->res,i,fieldpos[j]);
			if (value)
				{
				if (value)
					g_strstrip(value);
				if (strcmp(value, idval) == 0) 
					{
					mem_free(fieldpos);
					return 0;
					}
				}
			}
		}
	/* Go back to where we were */
	bonddb_moveto(jumpto, oldrow);
	mem_free(fieldpos);
	return -1;
	}

/**
 * bonddb_jumpto: 
 * @param jumpfrom :
 * @param jumpto : 
 *
 * get the current row in jumpfrom and set as current row for jumpto
 * 
 * @return  0 on success -1 on error
 */
gint
bonddb_jumpto(BonddbClass *jumpfrom, BonddbClass *jumpto)
{
	gint row;	
	g_assert(jumpfrom);
	g_assert(jumpto);
	if (jumpto->firsttable == NULL)
	{
		gchar *tmpstring;
		g_assert(jumpto->query);
		tmpstring = bonddb_parse_getfirsttable(jumpto->query);
		bonddb_setfirsttable(jumpto,tmpstring);
		mem_free(tmpstring);
	}

	if (jumpfrom->firsttable == NULL || jumpto->firsttable == NULL)
	{
		errormsg("jumpto or jumpfrom first table is null. "
		 "Make a more friendlier sql statement");
		return -1;
	}
/*	g_assert(jumpfrom->firsttable);
	g_assert(jumpto->firsttable); */
	
	if (jumpfrom->numrecord==0) return -1;
	
	jumpfrom->currentid = bonddb_id_get_currentrow(jumpfrom, jumpfrom->firsttable);
	if (!jumpfrom->currentid)
	{
		errormsg("Jumpto failed - no source id  \n\n**DOES THE TABLE HAVE A PRIMARY KEY?**\nFrom: %s\nTo: %s",jumpfrom->query,jumpto->query);
		return -2;
	}
	row = bonddb_id_get_row_from_id(jumpto, jumpfrom->currentid, jumpto->firsttable);
	if (row < 0) {
		errormsg("Jumpto failed - no matching row number");
		return row;		
	}
/*	debuglog(95,"seeking row %d",row); */
	return bonddb_moveto(jumpto, row);
}
/**
 * bonddb_undo()
 * 
 * Undoes the current objects in the db.
 */
gint 
bonddb_undo(BonddbClass *obj)
{
	gint retval;
	retval = bonddb_record_undo(obj, obj->currentrow);
	return retval;
}

gint
bonddb_settable(BonddbClass *obj, gchar *firsttable)
{
	if (obj->firsttable)
	{
		if (strcmp(obj->firsttable, firsttable) != 0)
			warningmsg("First table is already set to %s", obj->firsttable);
		return -1;
	}
	if (obj->tablelist)
	{
		errormsg("Table list is already set. Can't reset");
		return -2;
	}
	obj->tablelist = g_list_append(obj->tablelist,mem_strdup(firsttable));
	bonddb_setfirsttable(obj,firsttable);
	return 0;
}

GList *
bonddb_fieldlist(BonddbClass *obj)
{
	GList *retlist = NULL;
	gchar *dummy;
	if (!obj->fieldhash)
	{
		warningmsg("No field hash defined");
		return NULL;
	}
	dummy = mem_strdup("dummy-ignore");
	retlist = g_list_append(retlist,dummy);
	g_hash_table_foreach(obj->fieldhash, bonddb_field_addtolist, retlist);
	retlist = g_list_remove(retlist, dummy);
	mem_free(dummy);
	if (!retlist)
		warningmsg("No fields found in resultset");
	return retlist;

}
