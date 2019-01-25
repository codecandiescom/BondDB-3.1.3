#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "bonddbkey.h"
#include "bc.h"

/* free str, no matter what the data type  */
static void
key_freestr(void *str)
{
	mem_free(str);
}

/* free the key structure pointed to by ptr  */
static void
key_freekeystruct(void *ptr)
{
	BonddbPrimaryKey *key;
	GList *walk;
	key = ptr;
	mem_free(key->tablename); 
	for (walk=g_list_first(key->fieldlist);walk!=NULL;walk=walk->next)
		mem_free(walk->data);
	g_list_free(key->fieldlist);
	mem_free(key);
}

/**
 * bonddb_primarykey_new()
 * @param tablename : name of table
 * @param list : list of fields
 * 
 * Create new BonddbPrimaryKey object conatining the @tablename and a 
 * list of fields.
 * 
 * @return  BonddbPrimaryKey object
 */
BonddbPrimaryKey *
bonddb_primarykey_new(gchar *tablename, GList *list)
{
	GList *walk;
	BonddbPrimaryKey *key;
	key = mem_calloc(sizeof(BonddbPrimaryKey));
	key->tablename = mem_strdup(tablename);
	for (walk=g_list_first(list);walk!=NULL;walk=walk->next)
		{
		key->fieldlist = g_list_append(key->fieldlist,
				mem_strdup(walk->data));
		}
	return key;
}

/**
 * bonddb_primarykey_add()
 * @param obj : Database Object
 * @param tablename : name of table
 * @param fieldlist : list of fields
 * 
 * If the database object has no primary key, 
 * use the tablename and the list of fields to get the pkey.
 * 
 * @return  non-zero on failure
 */
gint
bonddb_primarykey_add(BonddbClass *obj, gchar *tablename, GList *fieldlist)
{
	gint slen;
	BonddbPrimaryKey *value;
	g_assert(obj);
	g_assert(tablename);
	if (fieldlist == NULL)
		return -1;
	slen = strlen(tablename);
	if (slen <= 0 || slen > 128000)
		g_assert(NULL);
	if (obj->primarykey == NULL)
	{
		obj->primarykey = g_hash_table_new_full(g_str_hash,
			g_str_equal, key_freestr, key_freekeystruct);
	}
	/* Remove old entry */
	if ((value = g_hash_table_lookup(obj->primarykey, tablename)))
		g_hash_table_remove(obj->primarykey, tablename);
	value = bonddb_primarykey_new(tablename, fieldlist);
	g_hash_table_insert(obj->primarykey, mem_strdup(tablename), value);
	return 0;
}

/**
 * bonddb_primarykey_guess()
 * @param obj : Database Object
 * @param tablename : name of table
 * 
 * Guess what the primary key for a table is, if no primary key is specified.
 * 
 * @return  non-zero on failure
 */
gint
bonddb_primarykey_guess(BonddbClass *obj, gchar *tablename)
{
	gint retval;
	gboolean freefieldkey = FALSE;
	GList *fieldkeylist = NULL;
	
	g_assert(obj);
	g_assert(tablename);
	retval = obj->bonddb->l->_db_primarykeylist(obj->bonddb->l, 
		obj->conn, tablename, &fieldkeylist);
	if (retval == 1)
		freefieldkey = TRUE;
	else if (retval < 0)
	{
		errormsg("Failed to get primary keys for table %s",tablename);
		return retval;
	}
	if (fieldkeylist == NULL)
		return -10;
	/* debugmsg("%d keys found ",g_list_length (fieldkeylist)); */
	bonddb_primarykey_add(obj,tablename,fieldkeylist);
	
	if (freefieldkey)
		g_list_free(fieldkeylist);
	
	return 0;
}

/**
 * bonddb_primarykey_free()
 * @param obj : Database Object
 * 
 * Free the memory allocated for the objects primary key properties
 * 
 */
void 
bonddb_primarykey_free(BonddbClass *obj)
{
	g_assert(obj);
	if (obj->primarykey)
		g_hash_table_destroy(obj->primarykey);
}
