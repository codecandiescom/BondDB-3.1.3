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
#include <string.h>
#include <glib.h>

#include "bonddbobj.h"
#include "bonddbid.h"
#include "bonddbkey.h"
#include "bc.h"

/**
 * db_id_free()
 * @param id : Unique id
 *
 * Frees up the invidual @id, which is allcoed by a db_id_createblank.
 * This needs support for primary key cleanup at some point, you go baz go 
 * write that code
 *
 * @return  vpod
 */
void
bonddb_id_free(BonddbId * id)
	{
	mem_verify(id);
	/* Field list is in primary key so dont free it */
	if (id->primarykeys)
		mem_free(id->primarykeys);
	mem_free(id->tablename); 
	mem_free(id);
	}

/* For freeing deep copies; deep copies have copied the primarykeys
 * and the fieldlist */
void bonddb_id_free_copy(BonddbId *id)
	{
	gint x;
	GList *walk;

	mem_verify(id);
	for(walk = g_list_first(id->fieldlist); walk != NULL; walk = walk->next)
		mem_free(walk->data);
	g_list_free(id->fieldlist);
	for(x = 0; x < id->num; x++)
		mem_free(id->primarykeys[x]);
	mem_free(id->primarykeys);
	mem_free(id->tablename); 
	mem_free(id);
	}

/* Compare to BonddbIds and decide if they're equal */
gboolean bonddb_id_is_equal(BonddbId *a, BonddbId *b)
	{
	gint x;

	g_assert(a);
	g_assert(b);

	/* Check the easy things first */

	/* On an insert, often only the pg_oid field will be populated.
	 * This is enough to guarantee that the IDs match. */
	/* Also, if neither the pg_oid is not set, then don't compare them */
	if(a->pg_oid && b->pg_oid && a->pg_oid == b->pg_oid)
		return TRUE;

  if(a->num == b->num &&
    strcmp(a->tablename, b->tablename) == 0 &&
    g_list_length(a->fieldlist) == g_list_length(b->fieldlist))
		{
		/* Compare each item in the fieldlist against each other */
		for(x = g_list_length(a->fieldlist); x <= 0; --x)
			if(strcmp(g_list_nth_data(a->fieldlist, x), g_list_nth_data(b->fieldlist, x)) != 0)
				return FALSE;
		/* Compare the primary keys against each other */
		for(x = 0; x < a->num; x++)
			if(strcmp(a->primarykeys[x], b->primarykeys[x]) != 0)
				return FALSE;
		/* If we get here, then everything has matched */
		return TRUE;
		}

		return FALSE;
	}

/* Returns a deep copy of the srcid; all strings etc. are cloned
 * and must be freed by the caller. */
BonddbId * bonddb_id_copy(BonddbId *srcid)
	{
	BonddbId *destid;
	GList *walk;
	gint x;

	destid = mem_calloc(sizeof(BonddbId));
	destid->tablename = mem_strdup(srcid->tablename);
	destid->pg_oid = srcid->pg_oid;
	destid->num = srcid->num;
	destid->fieldlist = NULL;
	/* GList's g_list_copy only creates a shallow copy; we need a deep one. */
	/* The list is a list of gchar *s */
	for(walk = g_list_first(srcid->fieldlist); walk != NULL; walk = walk->next)
		destid->fieldlist = g_list_append(destid->fieldlist, mem_strdup(walk->data));
	/* Now clone the primary keys */
	if(destid->num > 0)
		{
		destid->primarykeys = mem_calloc((sizeof(gchar *)*destid->num));
		for(x = 0; x < destid->num; x++)
			destid->primarykeys[x] = mem_strdup(srcid->primarykeys[x]);
		}
	return destid;
	}

/* Checks for any problems -> returns string of error if there is */
static gchar *
bonddb_id_valid(BonddbClass *obj, gchar *tablename, gint row)
{
	/* mostly not needed */
	gchar *retstr = NULL;

	switch (obj->state)
	{
	case BONDDB_INVALID:
	case BONDDB_RAW:
		retstr = mem_strdup("BonddbClass* has invalid recordset so can not extract primary key");
	case BONDDB_NEW:
	case BONDDB_READ_RAW:
	case BONDDB_READ_ONLY:
	case BONDDB_READ_WRITE:
		break;
	default:
		retstr = mem_strdup("unknown option state\n");
	}
	return retstr;
}

/* extracts all the id's from the BonddbPrimaryKey object and stores them in
 * the BonddnId object
 */
static gint
bonddb_id_extract(BonddbClass *obj, BonddbId *id, BonddbPrimaryKey *key, 
		gint row)
{
	gchar *fieldname, *value;
	GList *walk;
	gint i, retval=0;
	
	g_assert(key);
	g_assert(!id->primarykeys);

	id->num = g_list_length(key->fieldlist);
	id->fieldlist = key->fieldlist;
	id->primarykeys = mem_calloc(sizeof(gchar*) * (id->num));
	
	walk = g_list_first(key->fieldlist);
	for (i=0;i<id->num;i++)
	{
		fieldname = walk->data;
		if ((retval = bonddb_getvalue(obj, key->tablename, 
					fieldname, &value)))
		{
			errormsg("Failed to extract primary key field "
				"%s.%s, %d returned. %s %d",key->tablename,
				fieldname,retval,value,obj->currentrow);
			return retval;
		}
		if (value == NULL)
		{
			errormsg("Value is null");
			return -31;
		}
		/* debugmsg("id %s.%s is set %s ",key->tablename, fieldname,value);
		if (atoi(value) == 0)
			errormsg("Id is 0"); */
		id->primarykeys[i] = value;
		walk = walk->next;
	}
	return retval;
}

BonddbId *
bonddb_id_get_currentrow(BonddbClass *obj, gchar *tablename)
{
	BonddbId *id;
	if (obj->currentid)
		return obj->currentid;
	if (obj->ignoreassert == FALSE)
		if (obj->currentrow < 0 || obj->currentrow >= obj->numrecord)
			return NULL;
	if (obj->mode >= BONDDB_INSERT)
		return NULL;
	id = bonddb_id_get(obj, tablename, obj->currentrow);
	if (id == NULL)
		return NULL;
	if (obj->tmpid)
		bonddb_id_release_tmp(obj);
	obj->record[obj->currentrow]->id = id;
	obj->currentid = id;
	return id;
}

/**
 * bonddb_id_get()
 * @param obj : Database Object
 * @param row : Row to remeber by.
 *
 * Similar to db_id_remeber() but allows you to specify the row you want to 
 * remeber the unique id for.  This will give a unique temporary id if one 
 * is not availiable for use.
 *
 * @return  NULL on failure else the uniqueid for the newly created row.
 */
BonddbId *
bonddb_id_get(BonddbClass *obj, gchar *tablename, gint row)
{
	BonddbPrimaryKey *key;
	BonddbId *id;
	gchar *errmsg;
	g_assert(obj);

	if (obj->noprimarykey == TRUE)
		return NULL;
	if (obj->primarykey == NULL || 
		g_hash_table_lookup(obj->primarykey, tablename) == NULL)
	{
		bonddb_primarykey_guess(obj, tablename);
	}
/*	if (tablename == NULL)
	{
		warningmsg("Tablename is null for %s",obj->query);
		return NULL;
	}
	if (obj->primarykey == NULL)
	{
		bonddb_primarykey_guess(obj, tablename);
	} */
	/* check second time after try and fix it */
	if (obj->primarykey == NULL)
	{
		obj->noprimarykey = TRUE;
		if(obj->query)
			warningmsg("No primary key set in query: %s", obj->query);
		return NULL;
	}
	
	key = g_hash_table_lookup(obj->primarykey, tablename);
	if (obj->ignoreassert == FALSE)
	{
		if (key == NULL)
		{
			errormsg("No primary keys are associated with table "
				"%s \non object recordset\n",tablename);
			obj->noprimarykey = TRUE;
			return NULL;
		}
		if ((errmsg = bonddb_id_valid(obj,tablename,row)) != NULL)
		{
			errormsg(errmsg);
			mem_free(errmsg);
			return NULL;
		}
	}
		
	id = mem_calloc(sizeof(BonddbId));
	id->tablename = mem_strdup(tablename);
	if (bonddb_id_extract(obj,id,key,row) != 0)
	{
		obj->noprimarykey = TRUE;
		bonddb_id_free(id);
		return NULL;
	}
	return id;
}

BonddbId *
bonddb_id_from_oid(BonddbClass *obj, gchar *tablename, gint row,
		guint32 oid)
{
	BonddbId *id;

	if (oid <= 0)
		return NULL;
	
	id = mem_calloc(sizeof(BonddbId));
	id->tablename = mem_strdup(tablename);
	id->pg_oid = oid;
			
	return id;
}

/**
 * bonddb_id_get_str()
 * @param obj : Database Object
 * @param id : Unique id
 * @param tablename : tablename of the unique id
 *
 * Allocates a string that you can put at the end of an SQL statement to get
 * the record specified by @id.  You will need to put a WHERE in your SQL
 * statement prior to appending this string onto it.
 *
 * Unlike most functions, the returned string here needs to be freed.  Use
 * mem_free(retstr), on successful running of this function.
 *
 * @return  newly created sql string, or %NULL on failure.
 */
gchar *
bonddb_id_get_str(BonddbClass *obj, BonddbId *id, gchar *tablename)
{
	GList *walk;
	gchar *retstr = NULL, *tmpstr;
	gchar **strlist;
	gint i;
	
	g_assert(obj);
	g_assert(tablename);
	g_assert(id);
	/* Actually, with variable table names for one object, this can happen,
	 * so I'm removing this...
	 * -- Liam 19/03/2008
	 */
	/*
	if (strcmp(tablename,id->tablename) != 0)
	{
		errormsg("Tablename %s != %s. This shouldnt occur.",
				tablename,id->tablename);
		return NULL;
	}
	*/

	walk = g_list_first(id->fieldlist);
	if (id->pg_oid != 0)
	{
		retstr = mem_strdup_printf("%s.oid=%d", 
				tablename, id->pg_oid);
		return retstr;
	}
	else if (id->num > 0)
	{
		strlist = (gchar **) mem_calloc(sizeof(gchar *) * 
				(id->num + 1));                
		
		for (i = 0; i < id->num; i++)
			{
			/* Length of fieldlist should be same as num */
			g_assert(walk);
			strlist[i] = mem_strdup_printf("%s.%s='%s'", 
				tablename, (gchar*)walk->data, 
				id->primarykeys[i]);
			walk = walk->next;
			}
		tmpstr = g_strjoinv(" AND ", strlist);
		retstr = mem_strdup(tmpstr);
		g_free(tmpstr); /* CHECKED: this should be g _ f r e e */
		for (i = 0; i < id->num; i++)
			mem_free(strlist[i]);
		mem_free(strlist);
		return retstr;	
	}
	else
		warningmsg("Not valid id");
	return retstr;
}

gint
bonddb_id_release_tmp(BonddbClass *obj)
{
	if (!obj->tmpid)
		return -1;
	bonddb_id_free(obj->tmpid);
	obj->tmpid = NULL;
	obj->bonddb->tmpid++;
	return 0;
}
/**
 *
 * Issues a temporary id to handle situations where you are creating
 * new records and need to be able to know some imaginary id for it.
 *
 */
BonddbId * 
bonddb_id_issue_tmp(BonddbClass *obj, gchar *tablename)
{
	g_assert(obj);
	g_assert(tablename);
	if (obj->tmpid)
		return obj->tmpid;
	
	obj->bonddb->tmpid --;
	obj->tmpid = mem_calloc(sizeof(BonddbId));
	obj->tmpid->tablename = mem_strdup(tablename);
	obj->tmpid->pg_oid = obj->bonddb->tmpid;
	return obj->tmpid;
}

gint
bonddb_id_get_row_from_id(BonddbClass *obj, 
		BonddbId *id, gchar *tablename)
{
	gint i, j, numrow, numcolumn, status, fieldnum = 0;
	char* value;
	gint* fieldpos;
	GList *walk;
	g_assert(obj);
	g_assert(obj->res);
	g_assert(tablename);
	g_assert(id);
	/* check the table name */
	if (strcmp(tablename,id->tablename) != 0) {
		errormsg("Tablename %s != %s. This shouldnt occur.",
				tablename,id->tablename);
		return -1;
	}

	/* find the num of primary fields in the object from name */
	/*int (*_db_numcolumn) (void *res);*/
	numcolumn = obj->bonddb->l->_db_numcolumn(obj->res);
	fieldpos = mem_calloc(id->num * sizeof(gint));
	/*get the first primary field name */
	walk = g_list_first(id->fieldlist);
	for (j=0;j<id->num;j++)
	{
		status = 1;
		for (i=0;i<numcolumn && status;i++)
		{
			g_assert(walk);
			/* char *(*_db_fieldname) (void *res, int pos); */
			value = obj->bonddb->l->_db_fieldname(obj->res, i);
			g_assert(value);
			if (strcmp(value, (gchar*)(walk->data)) == 0)
			{
				/*debugmsg("field %s, pos %d", (gchar*)(walk->data), i);*/
				fieldpos[fieldnum++] = i;
				status = 0;
			}
		}
		if (status == 1){
			errormsg("field %s.%s not found", tablename, (gchar*)(walk->data));
			mem_free(fieldpos);
			return -1;
		}
		walk = walk->next;
	}
	
	/*find the result whith the id*/
	numrow = bonddb_numrecord(obj);
	/* for eatch row in the result */
	for (i=0;i<numrow;i++)
	{
		status = 1;
		/* for each primary field in id */
		for (j=0; j<id->num && status; j++)
		{
			/*char *(*_db_get_value) (void *res, int row, int field);*/
			/* get the value in 'obj->res' at result line 'i' for the field 'fieldpos[j]'*/
			bonddb_record_read(obj, fieldpos[j],
				i, &value);
			if (!value)
				value = obj->bonddb->l->_db_get_value(
					obj->res,i,fieldpos[j]);
			if (value == NULL)
			{
				status = 0;
				warningmsg("value == NULL, "
						"this should not occure "
						"for a primary key field?");
			}
			else
			{
				/* to be compatible*/
				if (value)
				{ 
				/* these functions modify the value in-place */
					value = g_strstrip(value);
				}
				g_assert(id->primarykeys[j]);
				/*debugmsg("'%s'?='%s'", value, id->primarykeys[j]);*/
				if (strcmp(value, id->primarykeys[j]) != 0) 
					status = 0;
			}
		}
		if (status == 1)
		{
			mem_free(fieldpos);
			/*debugmsg("primary key find for row %d", i);*/
			return i;
		}
	}
	mem_free(fieldpos);
	return -1;
}
/*
static gchar *
bonddb_id_getstrhash(BonddbId *id)
{
	gint i;
	gchar *str=NULL;
	for (i=0;i<id->num;i++)
		str = mem_strdup_replace(str,"%s",id->primarykeys);
	if (str == NULL && id->pg_oid)
		str = mem_strdup_printf("%d",id->pg_oid);
	return str;
}

static void
bonddb_idlookup_freestr(gchar *str)
{
	mem_free(str);
}

gint
bonddb_idlookup_create(BonddbClass *obj)
{
	gint i;
	if (!obj->idlookup)
		return 0;
	obj->idlookup = g_hash_table_new_full(g_str_hash,g_str_equal,
		bonddb_idlookup_freestr,NULL);
	for (i=0;i<obj->numrecord;i++)
	{

	}
	return 0;
}

int
bonddb_idlookup_create(BonddbClass *obj)
{
	if (!obj->idlookup)
		return 0;
	g_hash_table_destroy(obj->idlookup);
	obj->idlookup = NULL;
} */
