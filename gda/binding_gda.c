#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bonddbplugin.h"
#include "bc.h"

#ifdef _GDA

/* Gnome-db specific headers, this is for supporting lots of databases */
#include <libgda/libgda.h>

typedef struct
	{
	GdaCommand *command;
	GdaDataModel *res;
	}
DbRecordSet;

typedef struct
	{
	GdaConnection *conn;
	GdaClient *client;
	gchar *provider;
	}
DbConnection;

/*=============================================================================
 * Connection based functions.
 *=============================================================================
 */

gchar*
_db_type()
{
	static gchar retstr[]="gda";
	return retstr;
}
gint
_db_init(BonddbPlugin *l)
{
	return 0;
}

gint
_db_cleanup(BonddbPlugin *l)
{

	if (l->tablehash)
		g_hash_table_destroy(l->tablehash); 
	l->tablehash = NULL;
	if (l->fieldlisthash)
		g_hash_table_destroy(l->fieldlisthash); 
	l->fieldlisthash = NULL;	
	if (l->primarykeyhash)
		g_hash_table_destroy(l->primarykeyhash); 
	l->primarykeyhash = NULL;
        if (l->defaulthash)
                g_hash_table_destroy(l->defaulthash);
        l->defaulthash = NULL;

	return 0;
}

void
gda_list_providers (void)
{
	GList *prov_list;
	GList *node;
	GdaProviderInfo *info;
              
	prov_list = gda_config_get_provider_list ();
	g_print ("Providers available;\n");

	for (node = g_list_first (prov_list); node != NULL;
	   node = g_list_next (node))
		{
		info = (GdaProviderInfo *) node->data;
		g_print ("ID: %s\n", info->id);
		}
              
	gda_config_free_provider_list (prov_list);
}

/**
 * _db_checkconnerror:
 * @conn: Database Connection
 * @errormsg: Returned error message string, dont free
 * 
 * Direct wrapper for PQerrorMessage(@conn).  
 *
 * Returns: non-zero on error.
 */
int
_db_checkconnerror(DbConnection *conn, gchar **errormsg)
	{
	GList *errors, *walk;
	gchar *retstr=NULL, *tmpbuf;
	GdaError *err;

	*errormsg = NULL;
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
	*errormsg = retstr;

	if (*errormsg)
		return -1;
	return 0;
	}

/**
 * _db_connect_db:
 * @initstring: Postgresql initilising string
 * 
 * Direct wrapper for PQconnectdb(@initstring).  
 * Provider is ignored when connecting directly to postgresql.
 * 
 * Returns: Connection to database.
 */
void *
_db_connect_db(BonddbPlugin *l, gchar * connstring, gchar *provider)
	{
	/* if connection established */
	static char gda_application_initalised = 0;
	DbConnection *ret;
	gchar *errormsg;
	gchar *newconnectstr;
	gchar providerdefault[] = "PostgreSQL";
	
	/* If no provider mentoned default to postgresql. */
	if (provider == NULL) 
		provider = providerdefault;
	/* Strip and tidy up the init string for database conncetion */
	g_assert(connstring);
	if (gda_application_initalised == 0)
		{
		gda_init("Bonddb","2.0",0,NULL);
	 	gda_application_initalised = 1;
		}
	
	newconnectstr = g_strdelimit(connstring, " \t\r\n", ' ');
	newconnectstr = g_strstrip(newconnectstr);
	
	ret = (DbConnection *) mem_alloc(sizeof(DbConnection));
	ret->client = gda_client_new();
	ret->provider = provider;
	/* libgda connection string format. */
	/* "HOST=localhost;DATABASE=testbond;USER=andru" */
	ret->conn = gda_client_open_connection_from_string(
					ret->client,provider,connstring,
					GDA_CONNECTION_OPTIONS_READ_ONLY);
	if (!GDA_IS_CONNECTION(ret->conn)) 
		{
		gda_list_providers ();
		_db_checkconnerror(ret, &errormsg);
        	errormsg("Could not open connection from string `%s', "
			"using provider `%s'\n%s",
                        newconnectstr, provider,errormsg);
		mem_free(errormsg);
		mem_free(ret);
 	        return NULL;
	        }
	return ret;
	}

/**
 * _db_disconnect_db:
 * @conn: Database Connection
 * 
 * Direct wrapper for PQfinish(@conn).
 */
int
_db_disconnect_db(BonddbPlugin *l, DbConnection * conn)
	{
	g_assert(conn);
	gda_client_close_all_connections (conn->client);
	g_object_unref(G_OBJECT(conn->client));
	mem_free(conn);
	/* gda_main_quit(); */
	return 0;
	}




/**
 * db_reset:
 * @conn: Database Connection
 * 
 * Direct wrapper for PQreset(@conn).  This function is not essential and 
 * i dont think it is even called anywhere. So no need to make use of it.
 */
gint
_db_reset(DbConnection * conn)
	{
	errormsg("Not implemented for libgda");
	return 0;
	}
	
/**
 * _db_numtuples:
 * @result: Result from query
 * 
 * Direct wrapper for db_dbnumrows(@result);.
 *
 * Returns: Number of rows.
 */
gint
_db_numtuples(DbRecordSet * result)
	{
	g_assert(result);
	g_assert(result->res);
	return gda_data_model_get_n_rows (result->res);
	}

/**
 * _db_clear:
 * @result: Query result
 * 
 * Direct wrapper for PQclear(@result).
 */
int
_db_clear(DbRecordSet * result)
	{
	if (result == NULL)
	{
		errormsg("Result set is null, can not free");
		return -1;
	}
	gda_command_free(result->command);
	g_object_unref(result->res);
	/* gda_recordset_free(result->res); */
	mem_free(result);
	result = NULL;
	return 0;
	}

/**
 * _db_exec:
 * @conn: Database connection
 * @query: Query string
 * 
 * Direct wrapper for PQexec(@conn, @query).
 *
 * Returns: RecordSet to database.
 */
void *
_db_exec(DbConnection* conn, gchar * query)
	{
	DbRecordSet *result;

	g_assert(conn);
	g_assert(query);
	result = (DbRecordSet *) mem_calloc(sizeof(DbRecordSet));
	/* build up command */
	result->command = gda_command_new(query,
		GDA_COMMAND_TYPE_SQL, 
		GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	/* Execute the query */
	result->res = gda_connection_execute_single_command(
		conn->conn, result->command, NULL);
	return result;
	}


/**
 * _db_get_value:
 * @result: Result from query
 * @row: Position in recordset
 * @field: Field position
 * 
 * Direct wrapper for PQgetvalue(@result, @row, @field);.
 *
 * Returns: String of field, g_free result.
 */
char *
_db_get_value(DbRecordSet * result, int row, int field)
	{
	GdaValue *value;

	value = (GdaValue *) gda_data_model_get_value_at (
		result->res, field, row);
	
	if (value == NULL)
		{
		errormsg("Tried to move to an invalid row (%d) or column "
				"(%d) in the recordset",row,field);
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
 * _db_numcolumn:
 * @result: Result from a query
 * 
 * Direct wrapper for PQnfields(@result);.
 *
 * Returns: Number of fields in the @result.
 */
gint
_db_numcolumn(DbRecordSet * result)
	{
	
	g_assert(result);
	g_assert(result->res);
	return gda_data_model_get_n_columns (result->res);	
	}

/**
 * _db_fieldname:
 * @result: Result from a query
 * @fieldpos: Field position
 * 
 * Direct wrapper for PQfname(@result,@fieldpos);.
 *
 * Returns: Name of a field in recordset.
 */
gchar *
_db_fieldname(DbRecordSet * result, gint fieldpos)
	{
#warning "Possible memory leak"
	return (gchar*)gda_data_model_get_column_title (result->res,fieldpos);
	}

/**
 * _db_uniqueid:
 * @result: Result from a insert
 * @fieldpos: Field position
 * 
 * Kinda a wrapper for PQoidValue(@result);
 * Assuming you run am insert query with db_dbexec(), this will return 
 * the unique id assocated with that record.  Note this works on just 
 * getting the oid value from postgresql, on other databases a bit of a 
 * work around will be needed.
 *
 * Returns: the pg OID value.
 */
guint32
_db_uniqueid(DbConnection *conn, DbRecordSet * result)
	{
	gchar *value;
	guint32 oid;
	
	g_assert(conn);
	g_assert(result);
	g_assert(result->res);	
	value = gda_connection_get_last_insert_id (conn->conn, result->res);
	oid = atol(value);
	g_free(value);
	return oid;
	}



/**
 * _db_checkerror:
 * @result: Query result
 * 
 * Checks if any errors occured when running a query to postgresql, and if 
 * so return the print the error messages and free any associated objects.
 *
 * Possibly a wrapper for PQresultStatus
 * 
 * Returns: non-zero on error
 */
int
_db_checkerror(DbConnection * conn, DbRecordSet* result, char **errormsg)
	{
	g_assert(result);
	*errormsg = NULL;
	if (result == NULL)
		{
		errormsg("Result is NULL");
		return -1;
		}
	return _db_checkconnerror(conn, errormsg);
	}

/**
 * _db_tuples_ok:
 * @res: Query result
 * 
 * Checks if any errors occured when running a query. If so abort.
 * 
 * Returns: non-zero on error
 */
int
_db_tuples_ok(DbConnection *conn, DbRecordSet *res)
{
	g_assert(res);
	g_assert(res->res);
	if (gda_data_model_get_n_rows (res->res) > 0)
		return 1;
	else
		return 0;
}

/* free memory allocated to pointer ptr */
static void
_db_free(void *ptr)
{
	mem_free(ptr);
}
	
/* Must free afterwards */
int
_db_tablename(BonddbPlugin *l, DbConnection *conn, DbRecordSet *res, 
		int pos, char **retstr)
{
	gchar *name;
	GdaColumn *col;

	*retstr = NULL;
	col = gda_data_model_describe_column(res->res,pos);
	if (col == NULL)
		return -2;
	name = (gchar*)gda_column_get_table(col);
	if (name)
		*retstr = mem_strdup(name);
	gda_column_free(col);
	
	return -1;
}

void
_db_freefieldlist(void *ptr)
{
	GList *walk;
	for (walk=g_list_first(ptr);walk!=NULL;walk=walk->next)
		mem_free(walk->data);
	g_list_free(ptr);
}

static GList* 
_db_cachefieldlist(BonddbPlugin *l, gchar *tablename)
{
	if (l->fieldlisthash == NULL)
		return NULL;
	return g_hash_table_lookup(l->fieldlisthash,tablename);
}

static void
_db_cachefieldlistinsert(BonddbPlugin *l,  gchar *tablename, GList *list)
{
	g_assert(list);
	if (l->fieldlisthash == NULL)
		l->fieldlisthash = g_hash_table_new_full(g_str_hash, 
				g_str_equal, _db_free, _db_freefieldlist);
	g_hash_table_insert(l->fieldlisthash,mem_strdup(tablename),list);
}

/* Lists all the fields for a specific database and returns them in 
 * @retlist. You must free retlist with _db_freefieldlist(). */
gint
_db_fieldlist(BonddbPlugin *l, DbConnection *conn, 
		gchar *tablename, GList **retlist)
{
	GdaParameter *p;
	GdaParameterList *plist;
	GdaDataModel *m;
	GdaValue *val;
	gint num, j, retval=0;
	gchar *tmpstr, *str;

	mem_verify(l);
	
	*retlist = NULL;
	if (l->cache_dictonary == TRUE)
	{
		*retlist = _db_cachefieldlist(l, tablename);
		if (*retlist)
			return 0;
	}
	p = gda_parameter_new_string("name",tablename);
	plist = gda_parameter_list_new();
	gda_parameter_list_add_parameter(plist,p);
	m = gda_connection_get_schema(conn->conn,
			GDA_CONNECTION_SCHEMA_FIELDS,plist);
	num = gda_data_model_get_n_rows(m);
	for (j=0; j < num; j ++)
	{
		val = (GdaValue*)gda_data_model_get_value_at(m,0,j);
		tmpstr = gda_value_stringify(val);
		str = mem_strdup(tmpstr);
		/* m.essage("adding %s",tmpstr); */
		*retlist = g_list_prepend(*retlist,str);
		g_free(tmpstr);
	}
	
	if (retlist)
	{
		*retlist = g_list_reverse(*retlist);

		if (l->cache_dictonary)
			_db_cachefieldlistinsert(l, tablename, *retlist);
		else
			retval = 1;
	} 
	
	gda_parameter_list_free(plist);
	g_object_unref(m);

	return retval;
	}

static gchar *
_db_primarykeylist_item(GList *fieldlist, gint column)
{
	gint counter = 0;
	GList *walk;
	for (walk=g_list_first(fieldlist);walk!=NULL;walk=walk->next)	
		{
		if (counter == column)
			return walk->data;
		counter ++;
		}
	return NULL;
}
void
_db_freeprimarylist(void *ptr)
{
	GList *walk;
	for (walk=ptr;walk!=NULL;walk=walk->next)
		mem_free(walk->data);
	
	g_list_free(ptr);
}

static GList* 
_db_cacheprimarylist(BonddbPlugin *l, gchar *tablename)
{
	if (l->primarykeyhash == NULL)
		return NULL;
	return g_hash_table_lookup(l->primarykeyhash,tablename);
}

static void
_db_cacheprimarylistinsert(BonddbPlugin *l,  gchar *tablename, GList *list)
{
	g_assert(list);
	if (l->primarykeyhash== NULL)
		l->primarykeyhash = g_hash_table_new_full(g_str_hash, 
				g_str_equal, _db_free, _db_freeprimarylist);
	g_hash_table_insert(l->primarykeyhash,mem_strdup(tablename),list);
}


gint
_db_primarykeylist(BonddbPlugin *l, DbConnection *conn, gchar *tablename, 
		GList **retlist)
	{
	GdaParameter *p;
	GdaParameterList *plist;
	GdaDataModel *m;
	GdaValue *val;	
	gint j, num;
      	gchar *tmpstr, *str;
	gint  retval=0;
	
	*retlist = NULL;
	if (l->cache_dictonary == TRUE)
	{
		*retlist = _db_cacheprimarylist(l, tablename);
		if (*retlist)
			return 0;
	}

	p = gda_parameter_new_string("name",tablename);
	plist = gda_parameter_list_new();
	gda_parameter_list_add_parameter(plist,p);
	m = gda_connection_get_schema(conn->conn,
			GDA_CONNECTION_SCHEMA_FIELDS,plist);
	num = gda_data_model_get_n_rows(m);
	for (j=0; j < num; j ++)
	{
		val = (GdaValue*)gda_data_model_get_value_at(m,4,j);
		/* Get if it is a primary key */
		if (gda_value_get_boolean(val) == TRUE)
		{
			val = (GdaValue*)gda_data_model_get_value_at(m,0,j);
			tmpstr = gda_value_stringify(val);
			str = mem_strdup(tmpstr);
			/* message("adding %s",tmpstr); */
			*retlist = g_list_prepend(*retlist,str);
			g_free(tmpstr);			
		}
	}
	
	if (l->cache_dictonary)
		_db_cacheprimarylistinsert(l, tablename, *retlist);
	else
		retval = 1;
	
	gda_parameter_list_free(plist);
	g_object_unref(m);

	return retval;
	}

struct BonddbDefaultValueCache
{
        gchar *value;
        gchar *func;
};

static void
_db_cachedefaultvalue_free(void *ptr)
{
        struct BonddbDefaultValueCache *c;
        c = ptr;
        mem_free(c->value);
        mem_free(c->func);
        mem_free(ptr);
}

static gchar *
_db_cachedefaultvalue_run(void *conn, gchar *sql)
{
        gchar *retstr, *query;
        gchar *errormsg;
        void *res;
        query = mem_strdup_printf("SELECT %s",sql);
       res = _db_exec(conn,query);
        if (_db_checkerror(conn,res,&errormsg) != 0)
        {
                errormsg("%s\n%s",errormsg,query);
                _db_clear(res);
                mem_free(errormsg);
                mem_free(query);
                return NULL;
        }
        mem_free(query);
        if (_db_numtuples(res) == 0)
        {
                _db_clear(res);
                return NULL;
        }
        retstr = mem_strdup(_db_get_value(res,0,0));
        _db_clear(res);
        return retstr;
}

static void
_db_cachedefaultvalue(BonddbPlugin *l,  gchar *key, gchar *value, gchar *func)
{
        struct BonddbDefaultValueCache *c;

        if (l->cache_dictonary == FALSE)
                return ;
        c = mem_calloc(sizeof(struct BonddbDefaultValueCache));
        c->value = mem_strdup(value);
        if (func)
                c->func = mem_strdup(func);

        if (l->defaulthash == NULL)
                l->defaulthash = g_hash_table_new_full(g_str_hash,
                                g_str_equal, _db_free,
                                _db_cachedefaultvalue_free);
        g_hash_table_insert(l->defaulthash,key,c);
}

/**
 * _db_default_isitafunction:
 *
 * Find out if the default value is actually a function to call from db
 * Not sure what value1 or value2 is, someone please fill us all in.
 */
static gchar*
_db_default_isitafunction(gchar * value1, gchar * value2)
        {
        gint i, num, matchcount = 0, cont = 0, slen;
        gint retval = 0;
        gchar *retstr = NULL;
        gchar compare[10] = "FUNC";

        /* debugmsg("got %s and %s",value1,value2); */
        /* HELLLLLOO WORLD! */
        g_assert(value1);
        g_assert(value2);
        num = strlen(value1);
        /* go through and find first bracket */
        for (i = 0; i < num; i++)
                if (value1[i] == '(')
                       retval = 1;
        /* find closing off bracket */
        if (retval == 1)
                {
                for (i = 0; i < num; i++)
                        if (value1[i] == ')')
                                retval = 2;
                        /* make it once less if u get what i mean */
                retval--;
                }
        /* ok now check value2, which is the adbin field in postgresql
         * for the clause FUNC */
        if (retval == 1)
                {
                num = strlen(value2);
                for (i = 0; i < num; i++)
                        {
                        if (value2[i] == compare[matchcount])
                                {
                                matchcount++;
                                cont = 1;
                             if (matchcount == strlen(compare))
                                        {
                                        /* finished here, we have a match */
                                        retval = 2;
                                        cont = 0;
                                        break;
                                        }
                                }
                        /* reset matchcount cause of non-consitent match */
                        else if (cont == 1)
                                {
                                cont = 0;
                                matchcount = 0;
                                }
                        }
                retval--;
                }
        if (retval == 0)
                {
                num = strlen(value1);
                /* looking for x::y in value1 */
                for (i = 1; i < num - 2; i++)
                    {
                        if (value1[i] == ':' && value1[i + 1] == ':')
                                {
                                retval = 1;
                                break;
                                }
                     }
                }
        if (retval == 1)
                {
                retstr = mem_strdup(value1);
                slen = strlen(retstr);
                /* ' character */
                if (retstr[0] == 39 && retstr[slen - 1] == 39)
                        {
                        for (i = 1; i < slen - 1; i++)
                                retstr[i - 1] = retstr[i];
                        retstr[i - 1] = 0;
                        }
                }
        /* ps. i'm really drunk at moment if you didnt notice and i am
         * having problems typing. drinking vodkaa. with friends
         * while watching exocides. expect 3 of 5 of us have fallen asleep
         * on flloor. So ummm. I'm here coding. Man I'm typing well
         * at moment. excuess me fro any really bad coding styles I may
         * while in this state */

        /* debugmsg("returning %d . yeah. for %s",retval,value1); */
        return retstr;
	}

/* Returns: free after use */
char *
_db_defaultvalue(BonddbPlugin *l, void *conn, char *tablename,
                char *fieldname)
{
#warning _db_defaultvalue only works with postgresql. libmysql probs
        gint num;
        static gchar blankstr[] = "";
        gchar *query, *tmpstr, *othertmpstr, *key, *funcname = NULL;
        gchar *errormsg,*retstr = NULL;
        struct BonddbDefaultValueCache *c;
        DbRecordSet *res;

        g_assert(l);
        g_assert(conn);

        key = mem_strdup_printf("%s.%s",tablename,fieldname);
        if (l->cache_dictonary && l->defaulthash)
        {
                c = g_hash_table_lookup(l->defaulthash, key);
                if (c)
                        mem_free(key);
                if (c && c->value[0] == 0)
                        return NULL;
                else if (c)
                {
                        if (c->func != NULL)
                                return _db_cachedefaultvalue_run(
                                        conn, c->func);
                        return mem_strdup(c->value);
                }
        }
       /* select * from pg_attributes; */
       query = mem_strdup_printf("select pg_attribute.attname, "
	        "pg_attrdef.adbin, pg_attrdef.adsrc "
                "from pg_attrdef, pg_attribute, pg_class "
                "where pg_attrdef.adnum = pg_attribute.attnum and "
                "pg_attrdef.adrelid = pg_attribute.attrelid "
                "and pg_class.oid=pg_attribute.attrelid and "
                "pg_class.relname='%s' and attname='%s'", tablename,
                fieldname);

        res = _db_exec(conn, query);
        if (_db_checkerror(conn,res,&errormsg) != 0)
        {
                errormsg("%s\n%s",errormsg,query);
                mem_free(errormsg);
                _db_clear(res);
                mem_free(query);
                return NULL;
        }
        mem_free(query);
        num = _db_numtuples(res);
        if (num <= 0)
        {
                _db_clear(res);
                _db_cachedefaultvalue(l,  key, blankstr, NULL);
                return NULL;
        }
	
        tmpstr = _db_get_value(res, 0, 2);
        if (tmpstr && strlen(tmpstr) > 0)
        {
                othertmpstr = _db_get_value(res, 0, 1);
                _db_clear(res);
                /* Run the default value function to find what it is */
                if (othertmpstr != NULL && strlen(othertmpstr) >= 0)
                        funcname = _db_default_isitafunction(tmpstr,
                                        othertmpstr);
                _db_cachedefaultvalue(l, key, tmpstr, funcname);
                if (funcname)
                {
                        retstr = _db_cachedefaultvalue_run(conn, funcname);
                        mem_free(funcname);
                }
                else
                        retstr = mem_strdup(tmpstr);
        }
        else
        {
                _db_clear(res);
                _db_cachedefaultvalue(l, key, blankstr, NULL);
        }
        return retstr;
}

/* This code is for finding out what relationships this table
 * has */

/* Copied from bonddbloadby.h. */
typedef struct _BonddbReference BonddbReference;
struct _BonddbReference
{
        gchar *fromtable;
        gchar *fromfield;
        gchar *totable;
        gchar *tofield;
        gboolean onetomany;
};

gint
_db_findreference_add(GList **retlist, gchar *refstr, gchar *tablename,
		gchar *fieldname)
{
	gint slen, i, j=0;
	gchar *curstr = NULL;
	BonddbReference *r;
	if (refstr == NULL)
		return -1;
	r = mem_calloc(sizeof(BonddbReference));
	slen = strlen(refstr+1);
	curstr = mem_calloc(slen);
	for (i=0;i<slen;i++)
	{
		if (refstr[i] == '.')
		{
			if (curstr[0] != 0 && r->totable == NULL)
			{
				r->totable = mem_strdup(curstr);
				memset(curstr,0,slen);
			}
		}
		else
		{
			curstr[j] = refstr[i];
			j++;
		}
	}
	if (curstr)
	{
		r->tofield = mem_strdup(curstr);
		r->fromtable = mem_strdup(tablename);
		r->fromfield = mem_strdup(fieldname);
	}
	else
	{
		errormsg("Failed to extract reference information from %s",
				refstr);
		mem_free(r);
		return -2;
	}
	if (curstr)
		mem_free(curstr);
	debugmsg("adding %s.%s -> %s.%s",r->fromtable,r->fromfield,
			r->totable,r->tofield);
	*retlist = g_list_prepend(*retlist,r);
	return 0;
}

void
_db_freereference(GList *list)
{
	GList *walk;
	BonddbReference *r;
	if (list == NULL)
		return ;
	for (walk=g_list_first(list);walk!=NULL;walk=walk->next)
	{
		r = walk->data;
		mem_free(r->fromtable);
		mem_free(r->totable);
		mem_free(r->tofield);
		mem_free(r->fromfield);
		mem_free(r);
	}
	g_list_free(list);
}

GList *
_db_findreference(BonddbPlugin *l, void *conn, gchar *tablename)
{
	gint num, numcol, i;
	GList *retlist = NULL;
	GdaColumn *col;
	gchar *sql, *errormsg, *fieldname;
	const gchar *ref;
	DbRecordSet *res;
	/* This code doesnt work. Appears to be a bug in GDA */
	
	/* m.essage("Finding references for %s",tablename); */
	sql = mem_strdup_printf("SELECT * FROM %s LIMIT 1",tablename);
        res = _db_exec(conn, sql);
        if (_db_checkerror(conn,res,&errormsg) != 0)
        {
		debugmsg("Trying to find all the references on table %s",
				tablename);
                errormsg("%s\n%s",errormsg,sql);
		mem_free(sql);
		mem_free(errormsg);
		_db_clear(res);
		return NULL;
	}
	mem_free(sql);
	num = _db_numtuples(res);
	if (num == 0)
	{
		_db_clear(res);
		errormsg("Unable to find references for fields in table %s\n"
			"This is because there are no records in this table "
			"and the gda engine requires a field present",
			tablename);
		return -2;
	}
	
	numcol = _db_numcolumn(res);
	for (i=0;i<numcol;i++)
	{
		col = gda_data_model_describe_column(res->res,i);
		if (col == NULL)
			continue;
		ref = gda_column_get_references(col);
		fieldname = _db_fieldname(res, i); 
		if (ref == NULL || ref[0] == 0)
			continue;
		debugmsg("getting references on col %d %s",i,fieldname);
		debugmsg("Ref is %s",ref);
		_db_findreference_add(&retlist, (gchar*)ref, 
				tablename, fieldname);
		gda_column_free(col);
	}
	
	_db_clear(res);
	
	return retlist;
}

#endif
