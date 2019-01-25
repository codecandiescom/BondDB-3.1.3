#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bonddbplugin.h"
#include "bc.h"
 
#ifdef _MYSQL
#define PG_SEPERATOR "\\000"

/* Postgresql specific headers to be included. */
#include <mysql/mysql.h>

/*=============================================================================
 * Connection based functions.
 *=============================================================================
 */

gchar*
_db_type()
{
	static gchar retstr[]="mysql";
	return retstr;
}

/* It this one usefull????*/
gint
_db_init(BonddbPlugin *l)
{
	return 0;
}

/* shoul be the same as for postgres */
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
	if (l->referencehash)
		g_hash_table_destroy(l->referencehash);
	l->referencehash = NULL;			
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
	g_assert(connstring);
	/* will take care of the string after */
	/* the connection */
	gint i;
	gint len;
	gchar* dbname = NULL;
	gchar* user = NULL;
	MYSQL* mysql = NULL;
	/* allocate the size need it if free in bonddb 
	 * otherwith mysql_ini will do it but it will be free by mysql_close*/
	/*mysql = malloc(sizeof(MYSQL));*/

	/* nasty code to get the dbname */
	debugmsg("dbname string search in %s", &connstring[0]);
	len = strlen(connstring);
	for (i = 0; i < len; i++){
		if (connstring[i] == ' ') connstring[i] = '\0';
	}
	for (i = 0; i < len - strlen("dbname="); i++){
		if (strncmp(&connstring[i], "dbname=", strlen("dbname=")) == 0){
			dbname=&connstring[i] + strlen("dbname=");
			debugmsg("dbname string find in %s", &connstring[i]);
		}
		else if (strncmp(&connstring[i], "user=", strlen("user=")) == 0){
			user=&connstring[i] + strlen("user=");
			debugmsg("user string find in %s", &connstring[i]);
		}
	}
	g_assert(dbname);
	/* initialise the stucture and allocate memory if mysql == NULL*/
	mysql = mysql_init(mysql);
	if (!mysql_real_connect(mysql,"127.0.0.1","bond","bond",dbname,3306,NULL,0))
		{
			debugmsg("Failed to connect to database: Error: %s\n",
		    mysql_error(mysql));
		}
	return (void*)mysql;
	}

/**
 * _db_disconnect_db:
 * @conn: Database Connection
 * 
 * Direct wrapper for PQfinish(@conn).
 */
int
_db_disconnect_db(BonddbPlugin *l, void * conn)
	{
	g_assert(conn);
	mysql_close(conn);
	return 0;
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
_db_checkconnerror(void *conn, gchar **errormsg)
	{
	const char *tmpstr;
	*errormsg = NULL;
	g_assert(conn);
	tmpstr = mysql_error(conn);
	if (tmpstr && strlen(tmpstr) > 0)
		{
		*errormsg = (gchar*)tmpstr;
		return -1;
		}
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
_db_reset(void * conn)
	{
	g_assert(conn);
	/* mai be same as 
	 * It is preferable to use mysql_query() to issue an SQL FLUSH PRIVILEGES statement instead. */
	return 0;
	}
/**
 * _db_numtuples:
 * @result: Result from query
 * 
 * Direct wrapper for db_dbnumrows(@result);.
 *
 * Returns: Number of rows in a SELECT result.
 * 0 or NULL if error or no row;
 */
gint
_db_numtuples(void * result)
	{
	if (result == NULL){
		return 0;
	}
	g_assert(result);
	return mysql_num_rows(result);
	}

/**
 * _db_clear:
 * @result: Query result
 * 
 * Direct wrapper for PQclear(@result).
 */
int
_db_clear(void * result)
	{
	if (result == NULL)
		return -1;
	g_assert(result);
	mysql_free_result(result);
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
_db_exec(void * conn, gchar * query)
	{
	void *res;
	unsigned long len;
	g_assert(conn);
	g_assert(query);
	len = strlen(query);
	/* debugmsg("_db_exec database : %i\n",conn); */
	debugmsg("Running '%s'",query); 
	/* using mysql_real_query instead of depreciated mysql_query
	 * can be usefull one day for binary data */
	if (mysql_real_query(conn, query, len))
		{
		errormsg("Failed to query database: %s Error: %s\n",
			  query, mysql_error(conn));
		//g_assert(NULL);
		return NULL;
		}
	/* can be usefull one day 
	 * return the last id of indered row 
	 * lastid = mysql_insert_id(mysql);*/

	/*debugmsg("last id =%d", mysql_insert_id(conn));*/
	
	/* m.essage("Query %s",query); */
	/* if (!mysql_field_count(mysql)){
	 * 		return NULL;
	 * } */
	
	
	
		res = mysql_store_result(conn);
		/* if here is no result after INSER DELETE OR UPDATE 
		 * ceate an empty one*/
		
		if (res == NULL)
			if (mysql_affected_rows(conn) > 0){
				res = mysql_list_tables(conn, " ");
			}
		
		return res;
	}


/**
 * _db_get_value:
 * @result: Result from query
 * @row: Position in recordset
 * @field: Field position
 * 
 * Direct wrapper for PQgetvalue(@result, @row, @field);.
 *
 * Returns: String of field
 */
char *
_db_get_value(void * res, int row, int field)
	{
	char *retval;
	MYSQL_ROW_OFFSET save_row;
	MYSQL_ROW my_row;
	g_assert(res);
	/* Dont now if usefull */
	save_row = mysql_row_tell(res);

	/* select the row  form 0*/
	mysql_data_seek(res, row);
	
	if (field >= mysql_num_fields(res)){
		errormsg("The fild number %d is greater than the result fild number", field);
		g_assert(NULL);
		return NULL;
	}
	my_row = mysql_fetch_row(res);
	retval = (char*)my_row[field];
	
	if (retval == NULL) 
		return NULL;
	if (strlen(retval) == 0)
		return NULL;
	return retval;
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
_db_numcolumn(void * result)
	{
	g_assert(result);
	return mysql_num_fields(result);
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
_db_fieldname(void * result, gint fieldpos)
	{
	MYSQL_FIELD *field;
	g_assert(result);
	field = mysql_fetch_field_direct(result, fieldpos);
	return field->name;
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
_db_uniqueid(void *conn, void * result)
	{
	guint32 oid;

	g_assert(result);
	/* need to be done, may want to change the return type */
	/* try to make oid : tablenum id */
	
	oid = 0;
	if (oid <= 0)
		return 0;
	return oid;
	}



/**
 * _db_checkerror:
 * @result: Query result
 * 
 * Checks if any errors occured when running a query to postgresql, and if so return the
 * print the error messages and free any associated objects.
 *
 * Possibly a wrapper for PQresultStatus
 * 
 * Returns: non-zero on error
 */
int
_db_checkerror(void * conn, void * result, char **errmsg)
	{
	/*int err;*/
	
	/*
	*errmsg = NULL;

	
	g_assert(result);
	if (result == NULL)
		{
		errormsg("Result is NULL");
		return -1;
		}
	debugmsg("nedd error check here\n");
	*/
		/*
	err = PQresultStatus(result);
	if (err == PGRES_BAD_RESPONSE || err == PGRES_NONFATAL_ERROR
	        || err == PGRES_FATAL_ERROR)
		{
		*errmsg = mem_strdup(PQresultErrorMessage(result));
		return -2;
		}
	*/
	return _db_checkconnerror(conn, errmsg);
	}


int
_db_tuples_ok(void *conn, void *res)
{
	/*
	ExecStatusType status;
	g_assert(res);
	status = PQresultStatus(res);
	if (status == PGRES_EMPTY_QUERY)
		warningmsg("Invalid query sent");
	if (status == PGRES_COMMAND_OK)
		return 0;
	else
	if (status == PGRES_TUPLES_OK)
		return 1;
	return -1;
	*/
	/* should be if now row affected */ 
	if (res == NULL){
		return 0;
	}
	
	debugmsg("_db_tuples_ok need to be coded may need to return 0 for command and 1 for tuples");
	return 1;
}


/* was static*/
void
_db_free(void *ptr)
{
	mem_free(ptr);
}

/* was static*/
gchar *
_db_cachetable(BonddbPlugin *l, long oid)
{
	gchar *retstr;
	long *a;
	a = &oid;
	if (l->tablehash == NULL)
		return NULL;
	retstr =  g_hash_table_lookup(l->tablehash,a);
	if (retstr)
		return mem_strdup(retstr);
	return NULL;
}

/* was static*/
void
_db_cachetableinsert(BonddbPlugin *l, long oid, gchar *retstr)
{
	long *a;
	
	if (l->tablehash == NULL)
		l->tablehash = g_hash_table_new_full(g_int_hash, 
				g_int_equal, _db_free, _db_free);
	a = mem_alloc(sizeof(long));
	*a = oid;
	
	g_hash_table_insert(l->tablehash,a,mem_strdup(retstr));
	debugmsg("will see the cache after");
}


/* Must free afterwards */
int
_db_tablename(BonddbPlugin *l, void *conn, void *res, int pos, char **retstr)
{
	/*long oid;*/
	/*void *internalres;*/
	/*gchar *sql, *name;*/
	MYSQL_FIELD* field;
	
	/*debugmsg("_db_tablename must be coded\n");*/
	if (res == NULL){
		debugmsg("res=NULL\n");
	}
	/**retstr = NULL;*/
	field = mysql_fetch_field_direct(res, pos);
	*retstr = mem_strdup(field->table);
	/*
	oid = PQftable(res,pos);
	*/
	/*
	if (oid <= 0)
		return -2;
	*/
	/*
	if (l->cache_dictonary == TRUE)
	{
		*retstr = _db_cachetable(l, oid);
		if (*retstr)
			return 0;
	}
	*/
	/*sql = mem_strdup_printf("SELECT oid,relname FROM pg_class WHERE "
			"oid = '%ld'",oid);
	internalres = PQexec(conn, sql);
	mem_free(sql);
	g_assert(internalres);
	*/
	/*
	if (PQresultStatus(internalres) == PGRES_TUPLES_OK)
		{
		if (PQntuples(internalres) == 1)
			{
			name = PQgetvalue(internalres, 0, 1);
			*retstr = mem_strdup(name);
			PQclear(internalres);
			if (l->cache_dictonary == TRUE)
				_db_cachetableinsert(l, oid, *retstr);
			return 0;
			}
		}
	PQclear(internalres);
	*/
	/*debugmsg("Can't find table oid %ld",oid);*/
	return 0;	
}

void
_db_freefieldlist(void *ptr)
{
	GList *walk;
	for (walk=g_list_first(ptr);walk!=NULL;walk=walk->next)
		mem_free(walk->data);
	g_list_free(ptr);
}

/* was static*/
GList* 
_db_cachefieldlist(BonddbPlugin *l, gchar *tablename)
{
	if (l->fieldlisthash == NULL)
		return NULL;
	return g_hash_table_lookup(l->fieldlisthash,tablename);
}

/* was static */
void
_db_cachefieldlistinsert(BonddbPlugin *l,  gchar *tablename, GList *list)
{
	g_assert(list);
	if (l->fieldlisthash == NULL)
		l->fieldlisthash = g_hash_table_new_full(g_str_hash, 
				g_str_equal, _db_free, _db_freefieldlist);
	g_hash_table_insert(l->fieldlisthash,mem_strdup(tablename),list);
}

gint
_db_fieldlist(BonddbPlugin *l, void *conn, gchar *tablename, GList **retlist)
{
	gint num, j, retval=0;
	gchar /**query,*/ *tmpstr, *errormsg;
	void *result;
	MYSQL_FIELD* field;
	
	*retlist = NULL;
	if (l->cache_dictonary == TRUE)
	{
		*retlist = _db_cachefieldlist(l, tablename);
		if (*retlist)
			return 0;
	}

	/* select * from pg_attributes; */
        /*
		query = mem_strdup_printf("select attnum,attname,typname,attlen,"
		"attnotnull,atttypmod,usename,usesysid,pg_class.oid,"
		"attrelid,relpages,reltuples,relhaspkey,relhasrules,relacl "
		"from pg_class,pg_user,pg_attribute,pg_type where "
		"(pg_class.relname='%s') and "
		"(pg_class.oid=pg_attribute.attrelid) and "
		"(pg_class.relowner=pg_user.usesysid) "
		"and (pg_attribute.atttypid=pg_type.oid) and "
		"(attnum > 0) order by attnum", tablename);
	result = _db_exec(conn, query);
	*/
	result = mysql_list_fields(conn, tablename, NULL);
	if (result == NULL){
		warningmsg("failed query list field for table %s",tablename);
	}
	if (_db_checkerror(conn, result, &errormsg))
	{
		debugmsg("failed query with %s",errormsg);
		_db_clear(result);
		return -1;
	}
	num = _db_numcolumn(result);
	for (j = 0; j < num; j++)
	{
		/*tmpstr = PQgetvalue(result, j, 1);*/
		field = mysql_fetch_field_direct(result, j);
		tmpstr = field->name;
		/* m.essage("Got %s. (%s) %d",tmpstr,tablename,j); */
		*retlist = g_list_prepend(*retlist,mem_strdup(tmpstr));
	}
	if (*retlist)
	{
		*retlist = g_list_reverse(*retlist);

		if (l->cache_dictonary)
			_db_cachefieldlistinsert(l, tablename, *retlist);
		else
			retval = 1;
	}
	
	_db_clear(result);
	return retval;
	}

/* was static*/
gchar *
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
	g_list_free(ptr);
}

/* was static*/
GList* 
_db_cacheprimarylist(BonddbPlugin *l, gchar *tablename)
{
	if (l->primarykeyhash == NULL)
		return NULL;
	return g_hash_table_lookup(l->primarykeyhash,tablename);
}

/* was static*/
void
_db_cacheprimarylistinsert(BonddbPlugin *l,  gchar *tablename, GList *list)
{
	g_assert(list);
	if (l->primarykeyhash== NULL)
		l->primarykeyhash = g_hash_table_new_full(g_str_hash, 
				g_str_equal, _db_free, _db_freeprimarylist);
	g_hash_table_insert(l->primarykeyhash,mem_strdup(tablename),list);
}


gint
_db_primarykeylist(BonddbPlugin *l, void *conn, gchar *tablename, 
		GList **retlist)
	{
	void *result;
	gchar /**value,*/ *errormsg, *fieldn;
	/* gchar **arr, **ptr; */
	GList *fieldlist;
	gint num, i, /* columnn, */ numfield, retval=0;
	gboolean primary = FALSE, unique = FALSE;
	MYSQL_FIELD* field;
	
	debugmsg("search primary key\n");
	*retlist = NULL;
	/*
	if (l->cache_dictonary == TRUE)
	{
		*retlist = _db_cacheprimarylist(l, tablename);
		if (*retlist)
			return 0;
	}
	*/
	debugmsg("steel here\n");

	/*
	query = mem_strdup_printf("SELECT i.indkey, i.indisprimary, "
		"i.indisunique FROM pg_catalog.pg_class c, "
		"pg_catalog.pg_class c2, pg_catalog.pg_index i "
		"WHERE c.relname = '%s' "
		"AND c.oid = i.indrelid "
		"AND i.indexrelid = c2.oid "
		"AND pg_catalog.pg_table_is_visible(c.oid)", tablename);
	res = _db_exec(conn, query);
	*/
	/*MYSQL_RES *mysql_list_fields(MYSQL *mysql, const char *table, const char *wild)*/
	/* getting all teh fields */
	result = mysql_list_fields(conn, tablename, NULL);
	if (result == NULL){
		warningmsg("failed query list field for table %s",tablename);
	}
	if (_db_checkerror(conn,result,&errormsg) != 0)
		{
		_db_clear(result);
		return -1;
		}
	
	if (_db_fieldlist(l, conn, tablename, &fieldlist))
	{
		errormsg("Can't find primary keys due to field list failure");
		return -2;
	}
	numfield = g_list_length(fieldlist);
	
	num = _db_numcolumn(result);
	
	for (i=0;i<num;i++) 
		{
		primary = FALSE;
		unique = FALSE;
		
		field = mysql_fetch_field_direct(result, i);
		
		/*value = PQgetvalue(res, i, 0);*/
		/*
		value = "!test2";
		if (!value)
			continue;
		arr = g_strsplit (value, " ", 0);
		if (arr == NULL || arr[0][0] == '\0') 
			continue;
		*/	
		/*value = PQgetvalue (res, i, 1);*/
		
		if (IS_PRI_KEY(field->flags))
			primary = TRUE;
		
		/*value = PQgetvalue (res, i, 2);*/
		
		if ((field->flags) & UNIQUE_KEY_FLAG)
			unique = TRUE;
		/* Ignore non-primary keys */
		if (primary == FALSE)
			continue;
	    
		/*	
		ptr = arr;
		while (*ptr!=NULL)
			{
			columnn = atoi(*ptr);
			if (columnn > 0 && columnn-1 < numfield)
				{
				*/
				/* cause i start at 0 and pg_indx starts 
				 * at 1 */
				/*
				columnn--;
				fieldn = _db_primarykeylist_item(fieldlist,
						columnn);
				*retlist = g_list_append(*retlist, fieldn);
				*/
				/*db->field[columnn]->primary = primary;
				db->field[columnn]->unique = unique; */
				/* debugmsg("Primary key on %s.%s\n",
				 db->name,db->field[columnn]->name); */
		/*
				}
			else
				warningmsg("Cant find primary key at offset "
					"%d (%d fields)",columnn,numfield);
			ptr ++;
			}
		g_strfreev(arr);
		*/
			/* hope this work */
			debugmsg("find primary key %i", i);
			fieldn = _db_primarykeylist_item(fieldlist,
					i);
			*retlist = g_list_append(*retlist, fieldn);
		}
	if (l->cache_dictonary && *retlist)
		_db_cacheprimarylistinsert(l, tablename, *retlist);
	else{
		debugmsg("not find???");
		retval = 1;
	}
	
	_db_clear(result);
	
	return retval;
	}

struct BonddbDefaultValueCache
{
	gchar *value;
	gchar *func;
};

/* was static */
void
_db_cachedefaultvalue_free(void *ptr)
{
	struct BonddbDefaultValueCache *c;
	c = ptr;
	mem_free(c->value);
	if (c->func)
		mem_free(c->func);
	mem_free(ptr);
}

/* was static */
gchar *
_db_cachedefaultvalue_run(void *conn, 
		void *conn_remote_write, gchar *sql)
{
	gchar *retstr, *query;
	gchar *errormsg;
	void *res;
	query = mem_strdup_printf("SELECT %s",sql);
	if (strncmp(sql, "nextval", 7) != 0){
		/*debug for remote write*/
		debugmsg("slave : SELECT %s\n",sql);
		res = _db_exec(conn,query);
	}else{
		/*run the function nextval on the master
		 * to upgrade the sequence
		 */
		debugmsg("master : SELECT %s\n",sql);
		res = _db_exec(conn_remote_write, query);
	}		
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
	/*retstr = mem_strdup(PQgetvalue(res,0,0));*/
	retstr = "!test4";
	_db_clear(res);
	return retstr;
}

/* was static */
void
_db_cachedefaultvalue(BonddbPlugin *l,  gchar *key, gchar *value, gchar *func)
{
	struct BonddbDefaultValueCache *c;

	if (l->cache_dictonary == FALSE)
		return ;
	c = mem_calloc(sizeof(struct BonddbDefaultValueCache));
	c->value = mem_strdup(value);
	if (func && strlen(func) > 0)
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

/* was static */
gchar*
_db_default_isitafunction(gchar * value1, gchar * value2)
        {
        gint i, num, matchcount = 0, cont = 0, slen;
        gint retval = 0;
	gchar *retstr = NULL;
        gchar compare[10] = "FUNC";

	/* NOTE: This function was copied directly from src/dbgather.c
	 * in its orginal form including comments. */
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
_db_defaultvalue(BonddbPlugin *l, void *conn, 
		void *conn_remote_write, char *tablename, 
		char *fieldname)
{
	gint num, i;
	/*static*/ gchar blankstr[] = "";
	gchar *query, *tmpstr/*, *othertmpstr*/, *key/*, *funcname = NULL*/;
	gchar *errormsg,*newvalue,*retstr = NULL;
	struct BonddbDefaultValueCache *c;
	void *result, *result2;
	MYSQL_FIELD *field, *field2;
	MYSQL_ROW row;

	g_assert(l);
	g_assert(conn);

	key = mem_strdup_printf("%s.%s",tablename,fieldname);
	/* if dictionary is on remeber the value*/
	if (l->cache_dictonary && l->defaulthash)
	{
		c = g_hash_table_lookup(l->defaulthash, key);
		if (c)
			mem_free(key);
		if (c && c->value[0] == 0)
			return NULL;
		else if (c)
		{
			if (c->func != NULL){
				return _db_cachedefaultvalue_run(
					conn, conn_remote_write, c->func);
			}
			return mem_strdup(c->value);
		}
	}
	/* select * from pg_attributes; */
	/*
	query = mem_strdup_printf("select pg_attribute.attname, "
		"pg_attrdef.adbin, pg_attrdef.adsrc "
		"from pg_attrdef, pg_attribute, pg_class "
		"where pg_attrdef.adnum = pg_attribute.attnum and "
		"pg_attrdef.adrelid = pg_attribute.attrelid "
		"and pg_class.oid=pg_attribute.attrelid and "
		"pg_class.relname='%s' and attname='%s'", tablename,
		fieldname);

	res = _db_exec(conn, query);
	*/
	result = mysql_list_fields(conn, tablename, NULL);
	if (result == NULL){
		warningmsg("failed query list field for table %s",tablename);
	}

	if (_db_checkerror(conn,result,&errormsg) != 0)
	{
		errormsg("%s\n",errormsg);
		mem_free(errormsg);
		_db_clear(result);
		mem_free(key);
		return NULL;
	}

	num = _db_numcolumn(result);
	for (i=0;i<num;i++) 
		{
			field = mysql_fetch_field_direct(result, i);
			if (strcmp(field->name, fieldname) == 0) 
				break;
		}
	if (i == num){
		_db_clear(result);
		_db_cachedefaultvalue(l, key, blankstr, NULL);
		mem_free(key);
		return NULL;
	}
	
	/* test if autoincrement */
	if (field->flags & AUTO_INCREMENT_FLAG){
		query = mem_strdup_printf("SHOW TABLE STATUS LIKE '%s'", tablename);
		result2 = _db_exec(conn, query);	
		mem_free(query);
		
		num = _db_numcolumn(result2);
		for (i=0;i<num;i++) 
		{
			field2 = mysql_fetch_field_direct(result2, i);
			if (strcmp(field2->name, "Auto_increment") == 0) 
				break;
		}
		if (i == num){
			errormsg("Autoincrement not found in SHOW TABLE STATUS\n");
			_db_clear(result);
			_db_clear(result2);
			mem_free(key);
			return NULL;
		}
		row = mysql_fetch_row(result2);
		if (row == NULL){
			errormsg("No result, table '%s' not fond???\n", tablename);
			_db_clear(result);
			_db_clear(result2);
			mem_free(key);
			return NULL;
		}
		retstr = mem_strdup(row[i]);	
		_db_clear(result2);
	
		
		/* update autoincrement to reserve the value */
		newvalue = mem_strdup_printf("%lld", atoll(row[i]) + 1);
		query = mem_strdup_printf("ALTER TABLE %s AUTO_INCREMENT=%s", tablename, newvalue);
		result2 = _db_exec(conn, query);	
		_db_clear(result2);
		mem_free(query);
		mem_free(newvalue);
		mem_free(key);
	}else if ((tmpstr = field->def) && strlen(tmpstr) > 0){
		/*debugmsg("Getting defaut value %s, %s\n", field->name, field->def);*/
		/*othertmpstr = _db_get_value(res, 0, 1);*/
		/* Run the default value function to find what it is */
		/*
		if (othertmpstr != NULL && strlen(othertmpstr) >= 0)
			funcname = _db_default_isitafunction(tmpstr, 
					othertmpstr); 
		_db_cachedefaultvalue(l, key, tmpstr, funcname);
		*/
		/* not sure but should work */
		_db_cachedefaultvalue(l, key, tmpstr, NULL);
		/*
		if (funcname)
		{
			retstr = _db_cachedefaultvalue_run(conn, 
					conn_remote_write, funcname);
			mem_free(funcname);
		}
		else*/
			retstr = mem_strdup(tmpstr);
	}
	else
	{
		/*debugmsg("no defaut value for %s\n", field->name);*/
		_db_cachedefaultvalue(l, key, blankstr, NULL);
	}
	_db_clear(result);
	debugmsg("Default value fond for %s.%s=%s", tablename, fieldname, retstr);
	return retstr;
}

/* Copied from bonddbloadby.h. */
typedef struct _BonddbReference BonddbReference;
struct _BonddbReference
{
        gchar *fromtable;
        gchar *fromfield;
        gchar *totable;
        gchar *tofield;
        gboolean onetomany;
	gint fromcount;
	gint tocount;
};


/* was static */
gint
_db_constraint_valid(BonddbPlugin *l, BonddbReference *r)
{
	GList *list;
	BonddbReference *rwalk;
	if (r->tofield == NULL || r->totable == NULL || 
		r->fromfield == NULL || r->fromtable == NULL)
		return -1;
	list = g_hash_table_lookup(l->referencehash, r->fromtable);
	/* no list means not added yet */
	if (list == NULL)
		return 0;
	/* check for dupes */
	for (list = g_list_first(list); list != NULL; list = list->next)
	{
		rwalk = list->data;
		if (strcmp(rwalk->totable, r->totable) == 0 &&
			strcmp(rwalk->tofield, r->tofield) == 0)
			return -3;
	}

	return 0;
}

/* was static */
void
_db_referencevalue_free(void *ptr)
{
	GList *list;
	BonddbReference *r;

	for (list = g_list_first(ptr);list!=NULL;list=list->next)
	{
		r = list->data;
		g_assert(r);
		mem_free(r->fromtable);
		mem_free(r->totable);
		mem_free(r->tofield);
		mem_free(r->fromfield);
		mem_free(r);
	}
	g_list_free(ptr);
}

/* Find all the references in the database and add them to the cache
 */
/* was static */
gint
_db_constraint_init(BonddbPlugin *l, void *conn)
{
	gchar query[] = "select tgargs from pg_trigger";
        void *res;
        BonddbReference *r;
	gchar *item;
        gchar *tablename, *errormsg;
	GList *tablelist;
        gint i;
        g_assert(conn);
	g_assert(l->referencehash == NULL);

	l->referencehash = g_hash_table_new_full(g_str_hash, 
				g_str_equal, _db_free, 
				_db_referencevalue_free);
	/* This function was mostly copied from orginal src/dbconstraint.c
	 * code */
        /* for (i = 0; i < db->numtable; i++) 
	{ printf("Getting constraints (at start) for %d: ", i); 
	printf("%s\n", db->table[i]->name); } */
        res = _db_exec(conn, query);

	if (_db_checkerror(conn,res,&errormsg) != 0)
	{
		errormsg("%s\n%s",errormsg,query);
		mem_free(errormsg);
		_db_clear(res);
		return -1;
	}

        for (i = 0; i < _db_numtuples(res); i++)
	{
                /*printf("In loop %d of %d.\n", i, _db_numtuples(res));  */
                item = _db_get_value(res, i, 0);
				if(!item) continue;	
		item = strtok(item, PG_SEPERATOR);

                r = (BonddbReference *) mem_calloc(sizeof(BonddbReference));
		g_assert(r);
		r->tocount = -1;
		r->fromcount = -1;

		item = strtok(NULL, PG_SEPERATOR);
                r->fromtable = mem_strdup(item);
		item = strtok(NULL, PG_SEPERATOR);
                r->totable = mem_strdup(item);
                item = strtok(NULL, PG_SEPERATOR);
                r->fromfield = mem_strdup(item = strtok(NULL, PG_SEPERATOR));
                r->tofield = mem_strdup(item = strtok(NULL, PG_SEPERATOR));
		tablename = r->fromtable;

		if (_db_constraint_valid(l,r) != 0)
                {
			/* printf("Not valid .. for some reason %s\n",
			 r->fromtable); */
			mem_free(r->fromtable);
			mem_free(r->totable);
			mem_free(r->tofield);
			mem_free(r->fromfield);
			mem_free(r);
                }
                else
                {	
                        /* printf("Constraint being appended.\n%s.%s->%s.%s\n",
				r->fromtable,r->fromfield, 
				r->totable,r->tofield); */
			tablelist = g_hash_table_lookup(
				l->referencehash,tablename);

			if (tablelist == NULL)
			{
				tablelist = g_list_append(NULL,r);
				g_hash_table_insert(l->referencehash,
					mem_strdup(tablename),tablelist);
			}		
			else
				tablelist = g_list_append(tablelist,r);
                }
	}

        _db_clear(res);
        return 0;
}

/* All handled by the hash list so dont have to do anything */
void
_db_freereference(GList *list)
{
	return ;
}

GList *
_db_findreference(BonddbPlugin *l, void *conn, gchar *tablename)
{
	GList *retlist = NULL;

	if (l->referencehash == NULL)
		_db_constraint_init(l, conn);

	if (l->referencehash == NULL)
	{	
		debugmsg("No forign references in the database");
		return NULL;
	}

	retlist = g_hash_table_lookup(l->referencehash, tablename);
	return retlist;
}

/* was static*/
gchar *
_db_findgroups_query(void *conn, gchar *sql)
{
        void *res;
	gchar *value, *errormsg;
	gint num;

	g_assert(conn);
	res = _db_exec(conn, sql);
	if (_db_checkerror(conn,res,&errormsg) != 0)
	{
		errormsg("%s\n%s",errormsg,sql);
		mem_free(errormsg);
		_db_clear(res);
		return NULL;
	}
	num = _db_numtuples(res);
	if (num == 0)
	{
		_db_clear(res);
		return NULL;
	}
	value = mem_strdup(_db_get_value(res, 0, 0));
	_db_clear(res);
	return value;
}

GList *
_db_findgroups(BonddbPlugin *l, void *conn, gchar *username)
{
    /*
	void *res;
	GList *retlist = NULL;
	gchar *query, *value, *errormsg;
	gint uid=0, i, num;

	g_assert(conn);
	query = mem_strdup_printf(
		"SELECT usesysid FROM pg_user WHERE usename='%s'", username);
	value = _db_findgroups_query(conn, query);
	mem_free(query);
	
	if (value == NULL)
	{
		query = mem_strdup_printf(
			"SELECT oid FROM pg_rules WHERE rolname='%s'", 
			username);
		value = _db_findgroups_query(conn, query);
		mem_free(query);
	}
	if (value)
		uid = atoi(value);
	mem_free(value);
	if (uid == 0)
	{
		errormsg("Failed to get userid for %s", username);
		return NULL;
	}
	query = mem_strdup_printf(
		"SELECT groname FROM pg_group WHERE "
		"'%d' = ANY (grolist)", uid);
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
	num = 0;
	for (i=0;i<num;i++)
	{
		value = _db_get_value(res, i, 0);
		if (!value)
			continue;
		retlist = g_list_append(retlist, mem_strdup(value));
	}
	_db_clear(res);
	return retlist;*/
	debugmsg("find group not coded");
	return NULL;
}

#endif
