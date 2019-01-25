#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <glib.h>

#include "bonddbfield.h"
#include "bonddbobj.h"
#include "bc.h"

/* adds a new field with fieldname to tablename */
static BonddbField *
bonddb_field_new(const gchar * tablename, const gchar * fieldname,
                 const gint fieldpos, BonddbClass *parent)
	{
	BonddbField *f;

	if (fieldname == NULL)
		return NULL;

	f = mem_calloc(sizeof(BonddbField));
	f->parent = parent;
	f->tablename = (gchar *) tablename;
	f->fieldname = (gchar *) fieldname;
	f->fieldposinsql = fieldpos;
	return f;
	}

/* free the memory allocated for the feild pointer ptr */
static void
bonddb_field_free(void *ptr)
	{
	BonddbField *f;

	f = ptr;
	if (f->tablename)
		mem_free(f->tablename);
	if (f->fieldname)
		mem_free(f->fieldname);
	mem_free(f);
	}

/* free the memory allocated for list of fields  */
static void
bonddb_field_freelist(void *ptr)
	{
	GList *walk = (GList *) ptr;

	for (walk = g_list_first(walk); walk != NULL; walk = walk->next)
		bonddb_field_free(walk->data);
	g_list_free((GList *) ptr);
	}

/* create a new hash table  */
static void
bonddb_field_newhash(BonddbClass * obj)
	{
	obj->fieldhash = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                       (GDestroyNotify)mem_free_raw,
	                                       bonddb_field_freelist);
	}

/* append field f onto the hash table or
 * create a new hashtable and append field f onto it 
 */
static gint
bonddb_field_insert(BonddbClass * obj, BonddbField * f)
	{
	GList *list;

	g_assert(f);
	g_assert(obj);

	/* See if they key has already been registered. */
	list = g_hash_table_lookup(obj->fieldhash, f->fieldname);
	if (list == NULL)
		{
		/* if first time insert into the hash table a list */
		list = g_list_append(NULL, f);
		g_hash_table_insert(obj->fieldhash, mem_strdup(f->fieldname), 
				list);
		return 0;
		}
	else	/* Append to end of list of key */
		list = g_list_append(list, f);
	return 1;
	}

/* find the table for a field with name fname and a position  */
static gchar *
bonddb_field_findtable(BonddbClass * obj, gchar * fname, gint fieldpos)
	{
	errormsg("Not written!");
	return NULL;
	}

/**
 * db_field_populatebyresult()
 * @param obj : database object
 * 
 * Fill in all the information about that object which it doens't already 
 * know.  BonddbField, number of fields, what tables fields are from, and 
 * the name of table are added.  It hopes that there is a recordset present.
 * 
 * @return  non-zero on failure
 */
gint
bonddb_field_populatebyresult(BonddbClass * obj)
	{
	gchar *fname, *tname;
	BonddbField *f;
	gint i, numcolumn, success;

	g_assert(obj);
	if (bonddb_assert_recordset(obj))
		return -2;
	if (obj->fieldhash != NULL)
		{
		g_hash_table_destroy(obj->fieldhash);
		obj->fieldhash = NULL;
		}
	bonddb_field_newhash(obj);

	numcolumn = obj->bonddb->l->_db_numcolumn(obj->res);
	obj->numcolumn = numcolumn;
	for (i = 0; i < numcolumn; i++)
		{
		fname = mem_strdup(obj->bonddb->l->_db_fieldname(obj->res, i));
		/* get the table name */
		success = obj->bonddb->l->_db_tablename(obj->bonddb->l,
			obj->conn, obj->res, i,&tname);
		/* Some db engines can't get name correctly */
		if (tname == NULL && success == 0)
			tname = bonddb_field_findtable(obj, fname, i);
		f = bonddb_field_new((const gchar *)tname, 
				(const gchar *)fname, i, obj);
		if (f == NULL)
			{
			warningmsg("Failed to add field %s.%s",tname,fname);
			if (fname)
				mem_free(fname);
			if (tname)
				mem_free(tname);
			continue;
			}
		bonddb_field_insert(obj, f);
		}
	return 0;
	}

/**
 * db_field_populatebytablename()
 * @param obj : Database object 
 * @param tablename :
 * @param fieldpos :
 * 
 * Use the tablename and fieldpos to get the number of fields 
 * and fill in each dbfield for that single table.
 * 
 * @return  non-zero on error.
 */
gint
bonddb_field_populatebytablename(BonddbClass * obj, gchar * tablename,
                                 gint fieldpos)
	{
	gboolean freel = FALSE;
	gchar *fname, *tname;
	GList *fieldlist, *walk;
	BonddbField *f;

	g_assert(obj);
	g_assert(tablename);
	if (obj->fieldhash != NULL)
		{
		errormsg("Field is already hashed out for %s", obj->name);
		return -1;
		}
	bonddb_field_newhash(obj);
	obj->numcolumn = 0;

	if (obj->bonddb->l->_db_fieldlist(obj->bonddb->l, obj->conn,
	                                  tablename, &fieldlist) == 1)
		freel = TRUE;
	if (g_list_length(fieldlist) == 0)
		warningmsg("No fields returned for table %s",tablename);

	for (walk = g_list_first(fieldlist); walk != NULL; walk = walk->next)
		{
		tname = mem_strdup(tablename);
		fname = mem_strdup(walk->data);
		f = bonddb_field_new((const gchar *)tname, (const gchar *)fname,
		                     fieldpos, obj);
		fieldpos++;
		if (f == NULL)
			{
			if (fname)
				mem_free(fname);
			if (tname)
				mem_free(tname);
			continue;
			}
		bonddb_field_insert(obj, f);
		obj->numcolumn++;
		}

	if (freel)
		{
		obj->bonddb->l->_db_freefieldlist(fieldlist);
		/* debugmsg("freeing list"); */
		}

	return 0;
	}

/**
 * bonddb_field_setfirsttable()
 * @param obj : Database Object
 *
 * Set the first table to 0
 *
 */
gint
bonddb_field_setfirsttable(BonddbClass * obj)
	{
	/* errormsg("not written yet"); */
	return 0;
	}

/**
 * bonddb_field_get()
 * @param obj : Database Object
 * @param tablename : Tablename of the field.
 * @param fieldname : Field to look up
 *
 * Find the field defination 
 *
 * @return  The field defination if it exists, else null. 
 */
BonddbField *
bonddb_field_get(BonddbClass * obj, gchar * tablename, gchar * fieldname)
	{
	static GList *walk, *list;
	static BonddbField *f = NULL;

	g_assert(obj->fieldhash);
	list = g_hash_table_lookup(obj->fieldhash, fieldname);
	/* a simple answer */
	if (list && list->next == NULL)
		return list->data;
	/* assumes list is at first value for speed reasons */
	for (walk = list; walk != NULL; walk = walk->next)
		{
		f = walk->data;
		if (f->tablename == NULL && tablename == NULL)
			return f;
		if (f->tablename == NULL || tablename == NULL)
			continue;
		if (strcasecmp(f->tablename, tablename) == 0)
			return f;
		}

	return NULL;
	}

static gboolean _match_index(gpointer key,gpointer value,gpointer user_data)
	{
	GList *walk=((GList *)value);
		
/*	printf("start: %p %p %d\n",walk,user_data, *((gint *)user_data)); 	fflush(0); */

	for (;walk;walk=walk->next)
		{
/*		printf("step: %p %p %d\n",walk,walk->next,((BonddbField*)(walk->data))->fieldposinsql); fflush(0); */
		if(((BonddbField*)(walk->data))->fieldposinsql == ((gint*) user_data)[0])
			{
/*			printf("---  %s",((BonddbField*)(walk->data))->fieldname); fflush(0); */
			return TRUE;
			}
		}
	return FALSE;
	}
	
/* 
 * Returns 0 on success, updates fieldname and tablename to point to 
 * stings belonging to obj that should not be explicitly freed */
gint 
bonddb_field_get_name_by_index(BonddbClass * obj, gint fieldnumber, gchar** fieldname,gchar **tablename)
	{ 	
	static GList *walk;

	for (walk = g_hash_table_find(obj->fieldhash,_match_index,&fieldnumber);walk;walk=walk->next)
		{
		if(((BonddbField*)(walk->data))->fieldposinsql == fieldnumber)
			{
/*			printf("=== %s",((BonddbField*)(walk->data))->fieldname); fflush(0); */
			if(fieldname) *fieldname=((BonddbField*)(walk->data))->fieldname;
			if(tablename) *tablename=((BonddbField*)(walk->data))->tablename;
			return 0;
			}
		}
	return -1;
	}	

static void
bonddb_field_debug_each(void *key, void *value, void *ptr)
{
	BonddbField *f;
	GList *walk;
	if (key)
	if (!key || !value)
		return ;
	for (walk = g_list_first(value);walk!=NULL;walk=walk->next)
	{
		f = walk->data;
		g_assert(value);
		debuglog(85,"Field: spos%d %s.%s\n",f->fieldposinsql, 
			f->tablename, f->fieldname);
	}
}

void
bonddb_field_addtolist(void *key, void *value, void *ptr)
{
	BonddbField *f;
	GList *retlist = ptr;
	GList *walk;
	if (!key || !value)
		return ;
	retlist = g_list_last(retlist);
	for (walk = g_list_first(value);walk!=NULL;walk=walk->next)
	{
		f = walk->data;
		g_assert(value);
		retlist = g_list_append(retlist, mem_strdup(f->fieldname));
	}
}

gint
bonddb_field_debug(BonddbClass *obj)
{
	debug_output("Query: %s\n",obj->query);
	if (!obj->fieldhash)
	{
		warningmsg("No field hash defined");
		return -1;
	}
	g_hash_table_foreach(obj->fieldhash, bonddb_field_debug_each, NULL);
	return 0;
}

void
bonddb_field_default_each(void *fieldname, void *flist, void *tablename)
{
	BonddbClass *obj = NULL;
	BonddbField *f;
	GList *walk;
	gchar *value;
	for (walk=g_list_first(flist);walk!=NULL;walk=walk->next) 
	{
		f = walk->data;
		if (f->tablename == NULL)
			continue;
		if (strcmp(f->tablename,tablename) != 0)
			continue;
		if (obj == NULL)
			obj = f->parent;
		value = obj->bonddb->l->_db_defaultvalue(obj->bonddb->l,
				obj->conn, obj->bonddb->conn_remote_write, 
				f->tablename, f->fieldname);
		if (value)
		{
			/* m.essage ("%s.%s setvalue %s",f->tablename,
			 	f->fieldname,value); */
			bonddb_setvalue(obj,f->tablename,f->fieldname,
				value,FALSE);
			mem_free(value);
		}
	}
}

gint
bonddb_field_default(BonddbClass *obj, gchar *tablename)
{
	g_assert(obj);
	if (obj->currentrecord == NULL)
		return -1;
	if (obj->fieldhash == NULL)
		return -2;
	g_hash_table_foreach(obj->fieldhash,bonddb_field_default_each,
			(void*)tablename);
	return 0;
}
