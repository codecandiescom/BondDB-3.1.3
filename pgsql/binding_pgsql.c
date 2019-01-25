#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <bcmem.h>

/* this is how postgresql format RAISE NOTICE signals */
#define SIGNAL_PREFIX "NOTICE:  SIGNAL:"

/* use local copies of headers */
#include "../bonddb/bonddbplugin.h"
#include "../bondcommon/bc.h"
#include "../bonddb/bonddbglobal.h"
 
#ifdef _PGSQL

#define PG_SEPERATOR "\\000"

/* Postgresql specific headers to be included. */
#include <libpq-fe.h>

#define check_notify(x)   /* unimplemented */


	
#ifdef WIN32
#define SOCKVERSION (0x0202) /* version of winsock to request - I think 2.2 is apropriate */
#include <winsock2.h>
#include <windows.h>
#define sleep(x) Sleep( 1000 *(x))
#define usleep(x) Sleep((x)/1000)
#else
#define SOCKET int           /* use posix sockets */
#define INVALID_SOCKET (-1)
#endif

gboolean _db_user_has_role(BonddbPlugin *l, void *conn, gchar *username, gchar *role);

/** _db_check_notify()
 * @param conn Posgresql database connection
 *
 * Check for notifications from the Postgres server
 *
 * @return BonddbNotify* object, which should be freed by the caller
 */
BonddbNotify* _db_check_notify(void* conn, gboolean all_nofities)
	{

	PGnotify   *notify;
    
	notify = PQnotifies(conn);
	if(!notify )
		{
		fd_set          input_mask;
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		SOCKET sock = (SOCKET) PQsocket(conn);
		
		FD_ZERO(&input_mask);
		FD_SET(sock, &input_mask);
		if (sock == INVALID_SOCKET) 
			{ 
			errormsg("no connection");
			return NULL;
			}
		
		if (select(sock + 1, &input_mask, NULL, NULL, &timeout) < 0)
			{ 
			errormsg("can't listen"); 
			return NULL ;
			}
			
		/* Now check for input */
		if( FD_ISSET(sock,&input_mask))	
			{
			PQconsumeInput(conn);
			notify = PQnotifies(conn);
			}
		}

	if(notify)
		{
		/* If we are only listening for notifies triggered by our client connection,
		 * then we compare our backend process id value with that of the notify; if
		 * they do not match, then someone else triggered the notify and we should
		 * ignore it.  --Liam
		 */
		gint be_pid = PQbackendPID(conn);
		BonddbNotify *note = mem_alloc(sizeof(BonddbNotify));
		note->name = mem_strdup(notify->relname);
		note->local = (be_pid == notify->be_pid);
		PQfreemem(notify);
		return note;
		}

	return NULL;
	}

/*=============================================================================
 * Connection based functions.
 *=============================================================================
 */

gchar*
_db_type()
{
	static gchar retstr[]="pgsql";
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
	if (l->referencehash)
		g_hash_table_destroy(l->referencehash);
	l->referencehash = NULL;			
	return 0;
}

/**
 * _db_notice_processor()
 * @param arg : pointer to a GList of messages
 * @param message : notice message to be handled
 *
 * Override the default notice handler, allowing messages to be passed
 * back into bonddb and there to bond for handling.
 *
 * The messages are appended to a list in the plugin object.  These are
 * then extracted in the _db_checkerror() function and passed back to Bond.
 */
static void _db_notice_processor(void *arg, const char *message)
	{
	GList **l = (GList **)arg;

	/* Messages prefixed with NOTICE:  SIGNAL: are treated as messages to
	 * Bond and get recorded; the others are printed to stderr */
	if(l && 0==strncmp(message, SIGNAL_PREFIX, strlen(SIGNAL_PREFIX))) 
		{
		/* Chop of the prefix first, so that only the signal name comes through */
		gchar *m = g_strstrip(mem_strdup(message + strlen(SIGNAL_PREFIX)));
		/* Anything after the space is invalid */
		if(strchr(m, '\n')) 
			{
			gchar *tmp;
			gint size = ((strchr(m, '\n') - m));
			tmp = mem_alloc(size+1);
			strncpy(tmp, m, size );
			tmp[size] = 0;
			mem_free(m);
			m = tmp;
			}
		*l = g_list_append(*l, m);
		}
	else
		fprintf(stderr, message);
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
_db_connect_db(BonddbPlugin *l, gchar * connstring, gchar *provider, GList **list)
	{
	void *conn;
	g_assert(connstring);
	conn = PQconnectdb(connstring);
	if(conn)
		{
		l->notices = list;
		PQsetNoticeProcessor(conn, (PQnoticeProcessor)_db_notice_processor, l->notices);
		}

	return conn;
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
	if (conn == NULL)
		return -1;
	PQfinish(conn);
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
	gchar *tmpstr;
	*errormsg = NULL;
	g_assert(conn);
	check_notify(conn);		
	tmpstr = PQerrorMessage(conn);
	if (tmpstr && strlen(tmpstr) > 0)
		{
		*errormsg = tmpstr;
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
	check_notify(conn);		
	PQreset(conn);
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
_db_numtuples(void * result)
	{
	g_assert(result);
	return PQntuples(result);
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
	PQclear(result);
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

	g_assert(conn);
	check_notify(conn);
	g_assert(query);
	/* debugmsg("_db_exec database : %i\n",conn); */
	/* m.essage("Running %s",query); */
	res = PQexec(conn, query);
	/* Francis: Added the query result check */
	if (!res)
		{
		errormsg("According to postgresql documentation NULL "
			"value return for PQexec is a fatal error:\n%s\n",
			 PQerrorMessage(conn));
		g_assert(NULL);
		return NULL;
		}
	/* m.essage("Query %s",query); */
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

	g_assert(res);
	retval = PQgetvalue(res, row, field);
	if (retval == NULL)
		{
		errormsg("NULL returned for extraction at point %d, %s\n", 
			 field, PQresultErrorMessage(res));
		g_assert(NULL);
		return NULL;
		}
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
	return PQnfields(result);
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
	g_assert(result);
	return PQfname(result, fieldpos);
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
	oid = PQoidValue(result);
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
_db_checkerror(BonddbPlugin *l, void * conn, void * result, char **errmsg)
	{
	int err;
	check_notify(conn);
	if(errmsg)*errmsg = NULL;

	g_assert(result);
	if (result == NULL)
		{
		errormsg("Result is NULL");
		return -1;
		}


	err = PQresultStatus(result);
	if (err == PGRES_BAD_RESPONSE || err == PGRES_FATAL_ERROR)
		{
		*errmsg = mem_strdup(PQresultErrorMessage(result));
		return -2;
		}
	return 0;
	}

int
_db_tuples_ok(void *q, void *res)
{
	ExecStatusType status;
	check_notify(conn);
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
}

static void
_db_free(void *ptr)
{
	mem_free(ptr);
}

static gchar *
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

static void
_db_cachetableinsert(BonddbPlugin *l, long oid, gchar *retstr)
{
	long *a;
	
	if (l->tablehash == NULL)
		l->tablehash = g_hash_table_new_full(g_int_hash, 
				g_int_equal, _db_free, _db_free);
	a = mem_alloc(sizeof(long));
	*a = oid;
	
	g_hash_table_insert(l->tablehash,a,mem_strdup(retstr));
}
		
/* Must free afterwards */
int
_db_tablename(BonddbPlugin *l, void *conn, void *res, int pos, char **retstr)
{
	long oid;
	void *internalres;
	gchar *sql, *name;
	check_notify(conn);
	*retstr = NULL;
	oid = PQftable(res,pos);
	if (oid <= 0)
		return -2;
	if (l->cache_dictonary == TRUE)
	{
		*retstr = _db_cachetable(l, oid);
		if (*retstr)
			return 0;
	}
	
	sql = mem_strdup_printf("SELECT oid,relname FROM pg_class WHERE "
			"oid = '%ld'",oid);
	internalres = PQexec(conn, sql);
	mem_free(sql);
	g_assert(internalres);
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
	warningmsg("Can't find table oid %ld",oid);
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

gint
_db_fieldlist(BonddbPlugin *l, void *conn, gchar *tablename, GList **retlist)
{
	gint num, j, retval=0;
	gchar *query, *tmpstr, *errormsg;
	void *result;
	check_notify(conn);
	*retlist = NULL;
	if (l->cache_dictonary == TRUE)
	{
		*retlist = _db_cachefieldlist(l, tablename);
		if (*retlist)
			return 0;
	}

	/* select * from pg_attributes; */
        query = mem_strdup_printf("select attnum,attname,typname,attlen,"
		"attnotnull,atttypmod,pg_roles.rolname as usename,pg_roles.oid as usesysid,pg_class.oid,"
		"attrelid,relpages,reltuples,relhaspkey,relhasrules,relacl "
		"from pg_class,pg_roles,pg_attribute,pg_type where "
		"(pg_class.relname='%s') and "
		"(pg_class.oid=pg_attribute.attrelid) and "
		"(pg_class.relowner=pg_roles.oid) "
		"and (pg_attribute.atttypid=pg_type.oid) and "
		"(attnum > 0) order by attnum", tablename);
	result = _db_exec(conn, query);
	if (_db_checkerror(l, conn, result, &errormsg))
	{
		debugmsg("failed query with %s",errormsg);
		_db_clear(result);
		mem_free(query);
		return -1;
	}
	num = _db_numtuples(result);
	for (j = 0; j < num; j++)
	{
		tmpstr = PQgetvalue(result, j, 1);
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
	
	mem_free(query);
	_db_clear(result);
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
_db_primarykeylist(BonddbPlugin *l, void *conn, gchar *tablename, 
		GList **retlist)
	{
	void *res;
	gchar *query, *value, *errormsg, *fieldn;
	gchar **arr, **ptr;
	GList *fieldlist;
	gint num, i, columnn, numfield, retval=0;
	gboolean primary = FALSE, unique = FALSE;
	
	*retlist = NULL;
	if (l->cache_dictonary == TRUE)
	{
		*retlist = _db_cacheprimarylist(l, tablename);
		if (*retlist)
			return 0;
	}

	query = mem_strdup_printf("SELECT i.indkey, i.indisprimary, "
		"i.indisunique FROM pg_catalog.pg_class c, "
		"pg_catalog.pg_class c2, pg_catalog.pg_index i "
		"WHERE c.relname = '%s' "
		"AND c.oid = i.indrelid "
		"AND i.indexrelid = c2.oid "
		"AND pg_catalog.pg_table_is_visible(c.oid)", tablename);
	res = _db_exec(conn, query);
	if (_db_checkerror(l, conn,res,&errormsg) != 0)
		{
		_db_clear(res);
		mem_free(query);
		return -1;
		}
	mem_free(query);
	
	if (_db_fieldlist(l, conn, tablename, &fieldlist))
	{
		errormsg("Can't find primary keys due to field list failure");
		return -2;
	}
	numfield = g_list_length(fieldlist);
	
	num = _db_numtuples(res);
	for (i=0;i<num;i++)
		{
		primary = FALSE;
		unique = FALSE;
		value = PQgetvalue(res, i, 0);
		if (!value)
			continue;
		arr = g_strsplit (value, " ", 0);
		if (arr == NULL || arr[0][0] == '\0') 
			continue;
		
		value = PQgetvalue (res, i, 1);
		if (value && *value=='t')
			primary = TRUE;
		value = PQgetvalue (res, i, 2);
		if (value && *value=='t')
			unique = TRUE;
		/* Ignore non-primary keys */
		if (primary == FALSE)
			continue;
		
		ptr = arr;
		while (*ptr!=NULL)
			{
			columnn = atoi(*ptr);
			if (columnn > 0 && columnn-1 < numfield)
				{
				/* cause i start at 0 and pg_indx starts 
				 * at 1 */
				columnn--;
				fieldn = _db_primarykeylist_item(fieldlist,
						columnn);
				*retlist = g_list_append(*retlist, fieldn);
				/*db->field[columnn]->primary = primary;
				db->field[columnn]->unique = unique; */
				/* debugmsg("Primary key on %s.%s\n",
				 db->name,db->field[columnn]->name); */
				}
			else
				warningmsg("Cant find primary key at offset "
					"%d (%d fields for %s)",columnn,
					numfield, tablename);
			ptr ++;
			}
		g_strfreev(arr);
		}
	if (l->cache_dictonary && *retlist)
		_db_cacheprimarylistinsert(l, tablename, *retlist);
	else
		retval = 1;
	
	_db_clear(res);
	
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
	if (c->func)
		mem_free(c->func);
	mem_free(ptr);
}

static gchar *
_db_cachedefaultvalue_run(BonddbPlugin *l, void *conn, 
		void *conn_remote_write, gchar *sql)
{
	gchar *retstr, *query;
	gchar *errormsg;
	void *res;
	query = mem_strdup_printf("SELECT %s",sql);
	if (strncmp(sql, "nextval", 7) != 0){
		/* debug for remote write */
		debugmsg("slave : SELECT %s\n",sql);
		res = _db_exec(conn,query);
	}else{
		/* run the function nextval on the master
		 * to upgrade the sequence
		 */
		if (conn_remote_write == conn)
			debugmsg("slave :+ SELECT %s\n",sql);
		else
			debugmsg("master : SELECT %s\n",sql);
		res = _db_exec(conn_remote_write, query);
	}		
	if (_db_checkerror(l, conn,res,&errormsg) != 0)
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
	retstr = mem_strdup(PQgetvalue(res,0,0));
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
 * @value1: the pg_attrdef.adsrc value from the database
 * @value2: the pg_attrdef.adbin value from the database
 *
 * Find out if the default value is actually a function to call from db
 */
static gchar*
_db_default_isitafunction(gchar * value1, gchar * value2)
	{
	gint i, num/*, matchcount = 0, cont = 0*/, slen;
	gint retval = 0;
	gchar *retstr = NULL,*s;
 	gchar compare[10] = "FUNC";

	retval=NULL!=(s=strchr(value1,'('));
	retval=retval && strchr(s,')');
	retval=retval && (strstr(value2,compare) != NULL);

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
		 * at moment. excuess me fr	retval=NULL != strchr(value1,')');
		 * o any really bad coding styles I may 
		 * while in this state */ 
		
	/* dude! you aint kidding, all the didling with retval, and the 
	 * reimplementation of strstr :) */

	/* Yeah, this is pretty weird right here... */

	/* debugmsg("returning %d . yeah. for %s",retval,value1); */
	return retstr;
	}

/* Returns: free after use */
char *
_db_defaultvalue(BonddbPlugin *l, void *conn, 
		void *conn_remote_write, char *tablename, 
		char *fieldname)
{
	gint num;
	static gchar blankstr[] = "";
	gchar *query, *tmpstr, *othertmpstr, *key, *funcname = NULL;
	gchar *errormsg,*retstr = NULL;
	struct BonddbDefaultValueCache *c;
	void *res;

	g_assert(l);
	g_assert(conn);
	check_notify(conn);

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
			if (c->func != NULL){
				return _db_cachedefaultvalue_run(
					l, conn, conn_remote_write, c->func);
			}
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
	if (_db_checkerror(l, conn,res,&errormsg) != 0)
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
		_db_cachedefaultvalue(l, key, blankstr, NULL);
		return NULL;
	}
	tmpstr = _db_get_value(res, 0, 2);
	if (tmpstr && strlen(tmpstr) > 0)
	{
		othertmpstr = _db_get_value(res, 0, 1);
		/* Run the default value function to find what it is */
		if (othertmpstr != NULL && strlen(othertmpstr) >= 0)
			funcname = _db_default_isitafunction(tmpstr, 
					othertmpstr); 
		_db_cachedefaultvalue(l, key, tmpstr, funcname);
		if (funcname)
		{
			retstr = _db_cachedefaultvalue_run(l, conn, 
					conn_remote_write, funcname);
			mem_free(funcname);
		}
		else
			retstr = mem_strdup(tmpstr);
		_db_clear(res);
	}
	else
	{
		_db_clear(res);
		_db_cachedefaultvalue(l, key, blankstr, NULL);
	}
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

static gint
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

static void
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
static gint
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

	if (_db_checkerror(l, conn,res,&errormsg) != 0)
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

static gchar *
_db_findgroups_query(BonddbPlugin *l, void *conn, gchar *sql)
{
        void *res;
	gchar *value, *errormsg;
	gint num;

	g_assert(conn);
	res = _db_exec(conn, sql);
	if (_db_checkerror(l, conn,res,&errormsg) != 0)
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
        void *res;
	GList *retlist = NULL;
	gchar *query, *value, *errormsg;
	gint uid=0, i, num;

	g_assert(conn);
	query = mem_strdup_printf(
		"SELECT oid FROM pg_roles WHERE rolname='%s'", username);
	value = _db_findgroups_query(l, conn, query);
	mem_free(query);
	
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
	if (_db_checkerror(l, conn,res,&errormsg) != 0)
	{
		errormsg("%s\n%s",errormsg,query);
		mem_free(errormsg);
		_db_clear(res);
		mem_free(query);
		return NULL;
	}
	mem_free(query);
	num = _db_numtuples(res);
	for (i=0;i<num;i++)
	{
		value = _db_get_value(res, i, 0);
		if (!value)
			continue;
		retlist = g_list_append(retlist, mem_strdup(value));
	}
	_db_clear(res);
	return retlist;
}

/* Determine the current database user */
gchar *_db_get_user(void *conn)
  {
  return PQuser(conn);
  }

gboolean _db_conn_has_role(BonddbPlugin *l, void *conn_admin, void *conn, gchar *role)
	{
	gchar *username = _db_get_user(conn);
	return _db_user_has_role(l, conn_admin, username, role);
	}

gboolean _db_user_has_role(BonddbPlugin *l, void *conn, gchar *username, gchar *role)
	{
	gchar *query;
	void *res;
	gboolean retval = FALSE;
	gchar *errmsg = NULL;

	g_assert(username);
	g_assert(role);

	/* Save some effort if the username and role are the same */
	if(strcasecmp(username, role) == 0)
		return TRUE;

	query = mem_strdup_printf("\
		SELECT\
		  u.rolname AS username,\
		  r.rolname AS rolename\
		FROM\
		  pg_roles u\
		  INNER JOIN pg_auth_members p ON (u.oid = p.member)\
		  INNER JOIN pg_roles r ON (p.roleid = r.oid)\
		WHERE\
		  u.rolname = '%s'\
		  AND u.rolinherit\
		  AND r.rolname = '%s'\
	", username, role);
	
	res = _db_exec(conn, query);
	if(_db_checkerror(l, conn,res,&errmsg) != 0)
		{
		errormsg("%s\n%s",errmsg,query);
		mem_free(errmsg);
		_db_clear(res);
		mem_free(query);
		return FALSE;
		}
		
	mem_free(query);
	if( _db_numtuples(res) > 0)
		retval = TRUE;
	else
		/* User is not directly a member of the group, but may inherit
		 * through other roles that it has been granted. */
		{
		gint i, num;

		/* Get all groups the user belongs to - but only if
		 * the user can inherit */
		query = mem_strdup_printf("\
			SELECT\
				g.rolname\
			FROM\
				pg_roles u\
				INNER JOIN pg_auth_members ON (u.oid = member)\
				INNER JOIN pg_roles g ON (roleid = g.oid)\
			WHERE\
				u.rolname = '%s'\
				AND u.rolinherit\
			", username);
		res = _db_exec(conn, query);
		if(_db_checkerror(l, conn,res,&errmsg) != 0)
			{
			errormsg("%s\n%s",errmsg,query);
			mem_free(errmsg);
			_db_clear(res);
			}
		mem_free(query);
		
		num = _db_numtuples(res);
		/* For each group the role is a member of, check if that group,
		 * or its children, are members of the required group. */
		for (i=0;i<num;i++)
			{
			gchar *value = _db_get_value(res, i, 0);
			if (!value)
				continue;
			retval = _db_user_has_role(l, conn, value, role);
			if(retval)
				return retval;
			}
		retval = FALSE;
		}
		
	_db_clear(res);
	return retval;
	}

#endif
