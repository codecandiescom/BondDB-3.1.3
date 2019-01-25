#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bonddbpurify.h"
#include "bonddbwrite.h"
#include "bonddbobj.h"
#include "bonddbid.h"
#include "bc.h"

/* allocating memory for a BonddbWrite properties in a database object  */
static BonddbWrite *
bonddb_write_new(BonddbClass *obj)
{
	BonddbWrite *w;
	g_assert(obj);
	w = mem_calloc(sizeof(BonddbWrite));
	w->parent = obj;
	return w;
}

/* free the memory allocated for BonddbWrite object w */
static void 
bonddb_write_free(BonddbWrite *w)
{	
	g_assert(w);
	if (w->tablename)
		mem_free(w->tablename);
	if (w->fieldname)
		mem_free(w->fieldname);
	if (w->value)
		mem_free(w->value);
	if (w->hashkey)
		mem_free(w->hashkey);
	if (w->id && w->freeid)
		bonddb_id_free(w->id);
	mem_free(w);
}
/*
static void
bonddb_write_freeitem(BonddbClass *obj, BonddbWrite *worig)
{
	BonddbWrite *w1, *w2;
	GList *walk, *newlist = NULL;
	g_assert(obj);

	if (!obj->writequeuehash)
		return ;
	for (walk=g_list_first(obj->writequeue); walk!=NULL; walk=walk->next)
	{
		w1 = walk->data;
		if (w1->row == worig->row && strcmp(w1->tablename, worig->tablename)==0)
		{
			w2 = g_hash_table_lookup(obj->writequeuehash,w1->hashkey);
			if (w2)
			{
				w2->row = -10;
			}
		}
	}
	for (walk=g_list_first(obj->writequeue); walk!=NULL; walk=walk->next)
	{
		w1 = walk->data;
		if (w1->row != -10)
			newlist = g_list_append(newlist, w1);
		else
		{
			bonddb_write_free(w1);
			g_hash_table_remove(obj->writequeuehash,w1->hashkey);
		}
	}
	g_list_free(obj->writequeue);
	obj->writequeue = newlist;

	if (g_list_length(obj->writequeue) == 0)
	{
		g_list_free(obj->writequeue);
		obj->writequeue = NULL;
		if (obj->writequeuehash)
			g_hash_table_destroy(obj->writequeuehash);
		obj->writequeuehash = NULL;
	}
}*/

static void
bonddb_write_freelist_special(BonddbClass *obj, GList *dontfree)
{
	gboolean match = FALSE;
	BonddbWrite *w1, *w2;
	GList *walk1, *walk2, *savelist = NULL;
	debugmsg("Dealing with special case where a mark fails to save and "
		"needs to creativly free. %s",obj->query);
	for(walk1 = g_list_first(dontfree);walk1!=NULL;walk1=walk1->next)
	{
		w1 = walk1->data;
		for (walk2=g_list_first(obj->writequeue); walk2!=NULL; 
			walk2 = walk2->next)
		{
			w2 = walk2->data;
			if ((w2->row == w1->row && 
				strcmp(w2->tablename,w1->tablename) == 0) 
				|| match == TRUE)
			{
				w2->dontfree = TRUE;
				match = TRUE;
				savelist = g_list_append(savelist, w2);
			}
		}
	}
	if (savelist == NULL)
	{
		errormsg("Failed to create save list.");
		return ;
	}
	
	bonddb_write_freelist(obj);
	g_assert(!obj->writequeuehash);
	obj->writequeuehash = g_hash_table_new_full(g_str_hash,
			g_str_equal, NULL, NULL);

	for (walk1=g_list_first(savelist);walk1!=NULL;walk1=walk1->next)
	{
		w1 = walk1->data;
		if (w1->hashkey)
			g_hash_table_insert(obj->writequeuehash,
				w1->hashkey,w1);
		obj->writequeue = g_list_prepend(obj->writequeue,w1);
	}
	g_list_free(savelist);
}

/**
 * bonddb_write_freelist()
 * @param obj : Database object with write queue present.
 *
 * Free the memory allocated for the list of writes
 * 
 */
void 
bonddb_write_freelist(BonddbClass *obj)
{
	GList *walk;
	BonddbWrite *w;
	g_assert(obj);

	if (obj->writequeue == NULL)
		return ;
	for (walk=g_list_first(obj->writequeue); walk!=NULL; walk = walk->next)
	{
		w = walk->data;
		if (w->dontfree == FALSE)
			bonddb_write_free(walk->data); 
		else
			w->dontfree = FALSE;
	}
	g_list_free(obj->writequeue);
	obj->writequeue = NULL;
	if (obj->writequeuehash)
		g_hash_table_destroy(obj->writequeuehash);
	obj->writequeuehash = NULL;
}

/**
 * bonddb_write_add()
 * @param obj : Database object with write queue present.
 * @param tablename :
 * @param fieldname :
 * @param mark :
 *	
 * Add properties of BonddbWrite to the database object.
 * 
 */
void 
bonddb_write_add(BonddbClass *obj, gchar *tablename, gchar *fieldname,
		 gchar *value, gboolean mark)
{
	gchar *key;
	BonddbWrite *w;
	g_assert(obj);
	g_assert(tablename);

	if (obj->writequeuehash == NULL)
		obj->writequeuehash = g_hash_table_new_full(g_str_hash,
				g_str_equal, NULL, NULL);
	
	key = mem_strdup_printf("%d-%s.%s",obj->currentrow, tablename,
			fieldname);
	w = g_hash_table_lookup(obj->writequeuehash,key);
	/* m.essage("Looking up key %s returned %p",key,w); */
	if (w)
	{
		/* stop loss of data */
		if (w->mark == TRUE)
			mark = TRUE;
		obj->writequeue = g_list_remove(obj->writequeue,w);
		g_hash_table_remove(obj->writequeuehash,key);
		bonddb_write_free(w);
	}
	w = bonddb_write_new(obj);
	w->mark = mark;
	w->row = obj->currentrow;
	w->tablename = mem_strdup(tablename);
	w->fieldname = mem_strdup(fieldname);
	if (obj->currentid)
	{
		w->id = bonddb_id_get(obj, tablename, w->row);
		w->freeid = TRUE;
	}
	/* w->id = obj->currentid;  */
	w->parent = obj;
	if (value == NULL || value[0] == 0)
		w->value = NULL;
	else
		w->value = bonddb_purify(value);
	w->hashkey = key;
	if (obj->mode == BONDDB_INSERT)
		w->type = BONDDB_WRITE_INSERT;
	else
	if (obj->mode == BONDDB_UPDATE || obj->mode == BONDDB_INSERT_SAVED)
		w->type = BONDDB_WRITE_UPDATE;
	else
		errormsg("This shouldn't occur.  Object mode invalid.");

	g_hash_table_replace(obj->writequeuehash,key,w);
	obj->writequeue = g_list_prepend(obj->writequeue,w);
}

static gint
bonddb_write_delete_checkqueue(BonddbClass *obj, BonddbWrite *delw)
{
	gint retval=0;
	BonddbWrite *w;
	GList *walk, *freelist = NULL;

	for (walk=g_list_first(obj->writequeue);walk!=NULL;walk=walk->next)
	{
		w = walk->data;
		if (w->row == delw->row && w->type == BONDDB_WRITE_INSERT)
		{
			freelist = g_list_prepend(freelist,w);
		}
	}
	
	for (walk=g_list_first(freelist);walk!=NULL;walk=walk->next)
	{
		w = walk->data;
		obj->writequeue = g_list_remove(obj->writequeue,w);
		if (w->hashkey)
			g_hash_table_remove(obj->writequeuehash, w->hashkey);
		bonddb_write_free(w);
		retval = 1;
	}
	return retval;
}

/**
 * bonddb_write_delete()
 * @param obj : Database object with write queue present.
 * @param tablename :
 * @param recursive :
 *	
 * Delete BonddbWrite properties for database object obj.
 * 
 */
void 
bonddb_write_delete(BonddbClass *obj, gchar *tablename, gboolean recusive)
{
	BonddbWrite *w;
	g_assert(obj);
	g_assert(tablename);
	w = bonddb_write_new(obj);
	w->parent = obj;
	w->recusive = recusive;
	w->row = obj->currentrow;
	w->id = obj->currentid;
	w->type = BONDDB_WRITE_DELETE;
	w->tablename = mem_strdup(tablename);
	if (recusive)
		warningmsg("Recusive deleting is no longer supported. "
			"Postgresql supports cascading deletes and can "
			"handle this instead.");
	/* Sometimes you have to save other things before you delete things */
	if (bonddb_write_delete_checkqueue(obj, w) != 1)
	{
		obj->writequeue = g_list_prepend(obj->writequeue,w);
	}
	else
	{
		bonddb_record_delete(obj, w->row, w->tablename);
		bonddb_moveto(obj,obj->currentrow);
		bonddb_write_free(w);
	}
}

static gint
bonddb_write_handleinsert(BonddbClass *obj, BonddbWrite *w, guint32 oid)
{
	g_assert(obj);
	g_assert(w);
	g_assert(w->type == BONDDB_WRITE_INSERT);
	g_assert(obj->record);
	g_assert(obj->record[w->row]);
	g_assert(obj->record[w->row]->id == NULL);

	obj->record[w->row]->mode = BONDDB_INSERT_SAVED;
	obj->record[w->row]->id = bonddb_id_get(obj, w->tablename, w->row);

	if (oid != 0 && obj->record[w->row]->id)
		obj->record[w->row]->id->pg_oid = oid;
	if (obj->currentrow == w->row)
	{
		obj->mode = BONDDB_INSERT_SAVED;
		obj->currentid = obj->record[w->row]->id;
	}
		
	return 0;
}

/* drop the ' marks from a string */
static gchar *
bonddb_sqlparse_dropquation(gchar * query)
{
	return mem_strdup(query);
/* not used 	
	gint i, start = 0, len, j;
	gchar *retstr;

	retstr = mem_alloc((strlen(query) * sizeof(gchar) * 2) + 3);
	retstr[0] = 0;
	len = strlen(query);
	if (query[0] == 39)
                start = 1;
        if (len > 0 && query[len - 1] == 39)
                len--;
        j = 0;
        for (i = start; i < len; i++)
                {
                if (query[i] != 39)
                        retstr[j++] = query[i];
                else
                        {
                        retstr[j++] = 92;
                        retstr[j++] = 39;
                        }

                }
        retstr[j] = 0;
        return retstr;
*/
}

/* Allocate memory to write an insert sql query, using a field and values */
static void
bonddb_write_sql_insert(gchar **field, gchar **value, BonddbWrite *w)
{
	gchar *_field, *_value;
	gchar *quotedval = NULL;
	
	if (w->value)
		quotedval = bonddb_sqlparse_dropquation(w->value);
	
	if (*field == NULL)
		_field = mem_strdup_printf("\"%s\"",w->fieldname);
	else
	{
		_field = mem_strdup_printf("%s,\"%s\"",*field,w->fieldname);
		mem_free(*field);
	}
	if (*value == NULL)
	{
		if (quotedval)
			_value = mem_strdup_printf("'%s'",quotedval);
		else
			_value = mem_strdup("NULL");
	}
	else
	{
		if (quotedval)
			_value = mem_strdup_printf("%s,'%s'",*value,quotedval);
		else
			_value = mem_strdup_printf("%s,NULL",*value);
		mem_free(*value);
	}
	if (quotedval)
		mem_free(quotedval);
	*field = _field;
	*value = _value;
}

/*
static gboolean
bonddb_write_checkdup(BonddbWrite *w)
{
	BonddbField *f;
	gchar *value;
	
	f = bonddb_field_get(w->parent, w->tablename, w->fieldname);
	if (!f)
		return FALSE;
	bonddb_record_read(w->parent, f->fieldposinsql, w->row, &value);
	if (value == NULL && w->value == NULL)
		return TRUE;
	else
	if (value && w->value && strcmp(value,w->value) == 0)
		return TRUE;
	return FALSE;
} */

/* Allocate memory to write an update sql query, using the field */
static gint
bonddb_write_sql_update(gchar **field, BonddbWrite *w)
{
	gchar *retstr;
	gchar *quotedval = NULL;

	/* code does bad things */
	/* if (bonddb_write_checkdup(w) == TRUE)
		return -1; */
	
	if (w->value)
		quotedval = bonddb_sqlparse_dropquation(w->value);

	if (*field == NULL)
	{
		if (w->value)
			retstr = mem_strdup_printf("\"%s\"='%s'",
					w->fieldname,quotedval);
		else
			retstr = mem_strdup_printf("\"%s\"=NULL",
					w->fieldname);
	}
	else
	{
		if (w->value)
			retstr = mem_strdup_printf("%s,\"%s\"='%s'",*field,
				w->fieldname,quotedval);
		else
			retstr = mem_strdup_printf("%s,\"%s\"=NULL",*field,
					w->fieldname);
		mem_free(*field);
	}
	if (quotedval)
		mem_free(quotedval);
	*field = retstr;
	return 0;
}

/* Allocate memory to write the where clause in a sql query, using 
 * the tablename */
static gchar *
bonddb_write_sql_where(BonddbClass *obj, BonddbWrite *w, gchar *tablename)
{
	if (w->id == NULL)
		if (obj->record && (obj->currentrow >= 0 || 
			obj->currentrow < obj->numrecord))
		{
			if (obj->record[w->row]->id)
				w->id = obj->record[w->row]->id;
			else
				w->id = bonddb_id_get_currentrow(obj, 
						tablename);
		}
	if (w->id == NULL)
	{
		errormsg("No ID is present, can not save record "
			"for update.\n%s",obj->query);
		return NULL;
	}
	mem_verify(w->id);
	return bonddb_id_get_str(obj, w->id, tablename);
}

/* Allocate memory to write the sql query, using the tablename and row */
static gint
bonddb_write_sql(BonddbClass *obj, gint row, gchar *tablename, gchar **sql)
{
	gchar *field = NULL, *value = NULL;
	gchar *where;
	GList *walk;
	gint retval = 0;
	BonddbWrite *w, *lastw = NULL;
	
	g_assert(obj);
	g_assert(tablename);
	*sql = NULL;
	
	for(walk=g_list_first(obj->writequeue);walk!=NULL;walk=walk->next)
	{
		w = walk->data;
		if (w->row != row)
			continue;
		if (strcmp(tablename,w->tablename) != 0)
			continue;
		if (w->type == BONDDB_WRITE_INSERT)
			bonddb_write_sql_insert(&field,&value,w);
		else
		if (w->type == BONDDB_WRITE_UPDATE)
			bonddb_write_sql_update(&field,w);
		/* Only save the record if a field that is requested to be
		 * saved is in there. If only non-marked fields dont save
		 */
		 if (w->mark == TRUE) 
			lastw = w;
	}
	if (lastw != NULL)
	{
		if (lastw->type == BONDDB_WRITE_INSERT)
		{
			*sql = mem_strdup_printf("INSERT INTO %s (%s) "
				"VALUES (%s)",tablename,field,value);
			retval = 1;
			mem_free(field);
			mem_free(value);
		}
		else
		if (lastw->type == BONDDB_WRITE_UPDATE)
		{
			if (field == NULL)
				return 0;
			where = bonddb_write_sql_where(obj,lastw,tablename);
			if (where)
				*sql = mem_strdup_printf("UPDATE %s SET %s "
					"WHERE %s",tablename,field,where);
			retval = 2;
			mem_free(where);
			mem_free(field);
		}
	}
	else
	{
		/* Nothing to save just yet */
		if (field)
			mem_free(field);
		if (value)
			mem_free(value);
		return -1;
	}
	/*debugmsg("Sql : %s\n",sql); */
	return retval;
}

static gint
bonddb_write_sql_delete(BonddbClass *obj, BonddbWrite *w, gchar **sqlerror)
{
	gchar *sql, *where;
	void *res, *conn;
	
	g_assert(obj);
	g_assert(w);
	g_assert(w->tablename);
	where = bonddb_write_sql_where(obj, w, w->tablename);
	if (where == NULL)
	{
		errormsg("Can not delete record, unable to generate "
			"where in SQL delete statement.");
		return -1;
	}
	bonddb_record_delete(obj, w->row, w->tablename);

	sql = mem_strdup_printf("DELETE FROM %s WHERE %s",w->tablename,
			where);
	mem_free(where);
	if(obj->conn_name)
		conn = obj->conn;
	else
		conn = obj->bonddb->conn_remote_write;

	res = obj->bonddb->l->_db_exec(conn, sql);
	if (obj->bonddb->conn_remote_write == obj->bonddb->conn)
		debuglog(5,"Exec DELETE :\n%s",sql);
	else
		debuglog(5,"master DELETE :\n%s",sql);
	if (obj->bonddb->l->_db_checkerror(obj->bonddb->l, conn, res, sqlerror) != 0)
		{
		errormsg("SQL Failed with %s\nOn: %s",*sqlerror,sql);
		obj->bonddb->l->_db_clear(res);
		mem_free(sql);
		return -10;
		}
	obj->bonddb->l->_db_clear(res);
	mem_free(sql);
	return 0;
}

/* Allocate memory to write the exec function with the sql query*/
static guint32
bonddb_write_execsql(BonddbClass *obj, gchar *sql, gint type, gchar **sqlerror)
{
	guint32 retval=0, remote=0;
	void *result;
	void *conn = NULL, *localconn = NULL;
	if(obj->conn_name)
		conn = obj->conn;
	else if(obj->admin)
		{
		conn = obj->bonddb->conn_remote_write_admin;
		localconn = obj->bonddb->conn_admin;
		}
	if(NULL == conn)
		conn = obj->bonddb->conn_remote_write;
	if(NULL == localconn)
		localconn = obj->bonddb->conn;

	result = obj->bonddb->l->_db_exec(conn, sql);

	/* check for errors */
	if (obj->bonddb->conn_remote_write == obj->bonddb->conn)
		debuglog(5,"Exec :\n%s%s%s",DEBUG_ALTCOLOUR,sql,DEBUG_RESETCOLOUR); 
	else
		debuglog(5,"Master :\n%s",sql); 

	if (obj->bonddb->l->_db_checkerror(obj->bonddb->l, conn, 
			result,	sqlerror) != 0)
	{
		warningmsg("Write error %s",*sqlerror);
		/* If have a different write host than local host and
		 * write to remote database fails, trying writing to local
		 * database. This is a hack for temporary tables 
		 * and non-replicated tables that need local write access */
		/* Examples of this are temporary tables like ap_import,
 		 * email sending, and local_entryexit and local_idcard */
		/* TODO - Fix this so it does not happen. Need to not
 		 * have this code here. */
		if (conn != localconn)
		{
			remote = 1;
			warningmsg("Writing locally cause of error %s.",*sqlerror);
			obj->bonddb->l->_db_clear(result);
			result = obj->bonddb->l->_db_exec(localconn, 
				sql);
			if (obj->bonddb->l->_db_checkerror(obj->bonddb->l, localconn, 
				result,	sqlerror) != 0)
				retval = -10;
		}
		else retval = -10;
		
		if (retval == -10)
		{
			errormsg("SQL Failed with %s\nOn: %s",*sqlerror,sql);
			obj->bonddb->l->_db_clear(result);
			return retval;
		}
	}
	if (type == 1)
		{
		/* So you know how to do updates if the user modifies
		 * the insert record just after inserting. Need something
		 * for a where clause, and oid is the case here */
		retval = obj->bonddb->l->_db_uniqueid(
			conn, result);
		if(!retval)errormsg("Counldn't get OID from sql exec:%s.\n"
			"HINT: Make sure table is created with 'WITH OIDS'.",sql);
		}
	obj->bonddb->l->_db_clear(result);
	return retval;
}



/**
 * bonddb_write_all()
 * @param obj : Database object with write queue present.
 *
 * Writes all the current write queue back to the database backend
 * by generating the nessary SQL statements.
 *
 * @return  non-zero on error.
 */
gint
bonddb_write_all(BonddbClass *obj, gchar **sqlerror)
{
	gboolean psqlres_corrupt = FALSE;
	gint type, retval=0;
	guint32 oid;
	gchar *tmpstr, *sql;
	GHashTable *wrotehash;
	GList *walk, *dontfree=NULL;
	BonddbWrite *w;
	g_assert(obj);
	*sqlerror = NULL;

	/* all done before you start */
	if (obj->writequeue == NULL)
		return 0;
	if (obj->numrecord == 0)
	{
		warningmsg("Object has no records but have a write queue.");
		return -1;
	}
	wrotehash = g_hash_table_new_full(g_str_hash,g_str_equal,
			(GDestroyNotify)mem_free_raw,NULL);
	/* its created in reverse */
	obj->writequeue = g_list_reverse(obj->writequeue);
	for (walk=g_list_first(obj->writequeue); walk!=NULL;walk=walk->next)
	{
		w = walk->data;

		if (w->type == BONDDB_WRITE_DELETE)
		{
			/* postgresql pointer to results table gets corrupted
			 * if you delete the record its pointing to,
			 * so after a delete you need to refresh your data.
			 */
			if (bonddb_write_sql_delete(obj,w,sqlerror) == 0)
				psqlres_corrupt = TRUE;
			if (*sqlerror)
			{
				retval = -10;
				break;
			}
			continue;
		}
		
		tmpstr = mem_strdup_printf("%d-%s",w->row,w->tablename);
		debug_output("WRITE: %d. %s-> %s %d\n",w->row,w->fieldname,w->value,w->mark);
	/*	debug_output("WRITE: %s%d. %s-> %s %d%s\n",DEBUG_ATTCOLOUR,w->row,w->fieldname,w->value,w->mark,DEBUG_RESETCOLOUR); */

		if (g_hash_table_lookup(wrotehash,tmpstr) != NULL)
		{
			/* This table and row has already been processed. */
			mem_free(tmpstr);
			continue;
		}
		/* Table and row havn't been processed. Process and mark
		 * as such */
		type = bonddb_write_sql(obj, w->row, w->tablename, &sql);
		g_hash_table_insert(wrotehash,tmpstr,w);
		if (sql == NULL || type <= 0)
		{
			/* Need to abort */
			/* Commented out cause doesnt work to well 2006-08 */
/*			if (type == -1)
				dontfree = g_list_append(dontfree, w);
			if (sql)
				mem_free(sql);
			break; */
			continue;
		}
		if ((oid = bonddb_write_execsql(obj, sql, type, sqlerror)) != -10)
		{
			/* insert happened update id's etc for future
			 * updates */
			if (type == 1)
				bonddb_write_handleinsert(obj, w, oid);
			mem_free(sql);
		}
		else
		{
			/* SQL Failure. Running sql statement didn't work. */
			retval = -10;
			dontfree = g_list_append(dontfree, w);
			mem_free(sql);
			break;
		}
	}
	/* Hash table of what was written successfully */
	g_hash_table_destroy(wrotehash);
	if (dontfree)
	{
		bonddb_write_freelist_special(obj, dontfree);
		g_list_free(dontfree);
	}
	else
		bonddb_write_freelist(obj);
	/* refresh postgresql query */
	if (psqlres_corrupt == TRUE && obj->res && obj->query)
	{
		
		obj->bonddb->l->_db_clear(obj->res);
		obj->res = obj->bonddb->l->_db_exec(
			obj->bonddb->conn, obj->query);
		debuglog(5,"Exec :\n%s",obj->query);
		if (obj->bonddb->l->_db_checkerror(obj->bonddb->l,
			obj->bonddb->conn, obj->query, sqlerror) != 0)
			{
			errormsg("SQL Failed with %s\nOn: %s",*sqlerror,
					obj->query);
			obj->bonddb->l->_db_clear(obj->res);
			obj->res = NULL;
			retval = -21;
			}
		bonddb_moveto(obj,obj->currentrow);
	}
	return retval;
}
