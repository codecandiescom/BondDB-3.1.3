#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bc.h"

#include "bonddbfilter.h"
#include "bonddbobj.h"
#include "bonddbrecord.h"
#include "bonddbarray.h"

static void
bonddb_filter_free_request(BonddbFilterRequest *r)
{
	mem_free(r->fieldname);
	mem_free(r->tablename);
	mem_free(r->value);
	mem_free(r);
}

static void
bonddb_filter_clear_list(BonddbClass *obj)
{
	BonddbFilterRequest *r;
	GList *walk;
	if (obj->filterrequest == NULL)
		return ;
	for (walk=obj->filterrequest;walk!=NULL;walk=walk->next)
	{
		r = walk->data;	
		bonddb_filter_free_request(r);
	}
	g_list_free(obj->filterrequest);
	obj->filterrequest = NULL;
}

gint
bonddb_filter_clear(BonddbClass *obj)
{
	bonddb_filter_clear_list(obj);
	if (obj->filterhash == NULL)
		return -1;
	g_hash_table_destroy(obj->filterhash);
	obj->filterhash = NULL;
	return 0;
}

gint
bonddb_filter_moveto(BonddbClass *obj, gint row)
{
	BonddbRecord *r;
	int *ikey;
	ikey = &row;
	r = g_hash_table_lookup(obj->filterhash, ikey);
	if (r == NULL)
		return -1;
	return r->row;
}

gint
bonddb_filter_numrecord(BonddbClass *obj)
{
	return g_hash_table_size(obj->filterhash);
}

struct _BonddbFindRow
{
	gint realrow;
	gint foundrow;
};

static void
bonddb_filter_delete_findrowkey(void *_key, void *_value, void *ptr)
{
	struct _BonddbFindRow *row = ptr;
	BonddbRecord *r;
	gint key = *(gint*)_key;
	r = (BonddbRecord*)_value;
	if (r->row == row->realrow)
	{
		row->foundrow = key;
		/* m.essage("Found row for %d->%d",key,row->realrow); */
	}
}

/* current row is the real row number */
gint
bonddb_filter_delete(BonddbClass *obj, gint currentrow)
{
	struct _BonddbFindRow *row;
	gint *tmpkey, retval=-2;
	g_assert(obj);
	g_assert(currentrow >= 0 && currentrow < obj->numrecord);
	if (!obj->filterhash)
		return -1;
	row = mem_alloc(sizeof(BonddbRecord));
	row->foundrow = -1;
	row->realrow = currentrow;
	g_hash_table_foreach(obj->filterhash,bonddb_filter_delete_findrowkey,
			row);
	if (row->foundrow != -1)
	{
		tmpkey = &row->foundrow;
		if (bonddb_filter_numrecord(obj) < 1)
		{
			g_hash_table_destroy(obj->filterhash);
			obj->filterhash = NULL;
		}
		else
			g_assert(g_hash_table_remove(obj->filterhash,
						tmpkey)==TRUE);
		retval = 0;
	}
	mem_free(row);
	return retval;
}

static BonddbFilterRequest *
bonddb_filter_add(BonddbClass *obj, gchar *tablename, gchar *fieldname, 
		gchar *fieldvalue, sql_condition_operator opr)
{
	BonddbFilterRequest *r;
	r = (BonddbFilterRequest*)mem_calloc(sizeof(BonddbFilterRequest));
	r->fieldname = mem_strdup(fieldname);
	r->tablename = mem_strdup(tablename);
	r->value = mem_strdup(fieldvalue);
	r->operation = opr;
	obj->filterrequest = g_list_append(obj->filterrequest, r);
	return r;
}

static gboolean
bonddb_filter_compare(GHashTable *hash, gchar *filtervalue, gchar *value,
		sql_condition_operator opr)
{
	glong a = 0, b = 0;

	/* Handle empty data */
	if (value == NULL || filtervalue == NULL)
		{
		if (opr == SQL_eq && filtervalue == value)
			return TRUE;
		else if (opr == SQL_not && filtervalue != value)
			return TRUE;
		return FALSE;
		}
	/* operation can be: OprEqual, OprNotEqual, OprLike, OprGreater, 
	 * OprLess, OprGreaterEqual, OprLessEqual */
	if (opr > 1)
		{
		a = atoi(value);
		b = atoi(filtervalue);
		}

	switch (opr)
		{
	case SQL_eq:
		if (strcmp(filtervalue, value) == 0)
			return TRUE;
		break;
	case SQL_not:
		if (strcmp(filtervalue, value) != 0)
			return TRUE;
		break;
	case SQL_like:
		if (strstr(value, filtervalue))
			return TRUE;
		break;
	case SQL_gt:
		if (a > b)
			return TRUE;
		break;
	case SQL_lt:
		if (a < b)
			return TRUE;
		break;
	case SQL_geq:
		if (a >= b)
			return TRUE;
		break;
	case SQL_leq:
		if (a <= b)
			return TRUE;
		break;
	default:
		errormsg("Unknown option, dont know how to handle this.");
		break;
		}
	return FALSE;	
}

/* This is the source of the problems where a list row is automatically
 * selected.  This screws things up with tutorials that want you to click
 * on a list.  A large part of the problem is that 0 and 1 are treated as
 * the same. */
static gint
bonddb_filter_byvalue_internal(BonddbClass *obj, gchar *tablename, 
		gchar *fieldname, gchar *fieldvalue, 
		sql_condition_operator opr)
{
	gint i, numrow, oldrow, newrow = -1;
	gint filtercount = 0;
	gint *iptr;
	gchar *value;
	GHashTable *hash;

	numrow = bonddb_numrecord(obj);
	oldrow = obj->currentrow;
	
	if (numrow <= 0)
		return -2;
	hash = g_hash_table_new_full(g_int_hash, g_int_equal,(GDestroyNotify)mem_free_raw,
		NULL);
	
	for (i=0;i<numrow;i++)
	{
		/* Get value on the hash */
		if (bonddb_moveto(obj, i) < 0)
		{
			errormsg("Failed to move to record position");
			g_hash_table_destroy(hash);
			return -4;
		}
		if (bonddb_getvalue(obj, tablename, fieldname, &value))
		{
			errormsg("Failed to extract %s.%s from %s",
				tablename,fieldname,obj->query);
			g_hash_table_destroy(hash);
			return -3;
		}
		/* message("Comparing %s to %s %d",value,fieldvalue,
				obj->currentrow); */
		if (bonddb_filter_compare(hash,fieldvalue,value,opr) == TRUE)
		{
			iptr = mem_calloc(sizeof(gint));
			memcpy(iptr,&filtercount,sizeof(gint));
			/* m.essage("Filtering record. %d -> %d",*iptr,
					obj->currentrecord->row); */
			g_hash_table_insert(hash,iptr,obj->currentrecord);
			/* For figuring out what row to moveto when done */
			if (newrow == -1 && obj->currentrow >= oldrow)
				newrow = filtercount;
			filtercount ++;
		}
			
		if (i==0)
			obj->ignoreassert = TRUE;
	}
	obj->ignoreassert = FALSE;
	if (obj->filterhash)
		g_hash_table_destroy(obj->filterhash);
	obj->filterhash = NULL;
	if (newrow == -1 && filtercount == 0)
	{
		bonddb_moveto(obj, oldrow);
		newrow = -2;
	}
	obj->filterhash = hash;
	/* Specifically, this piece here, while appropriate for forms and dropdowns
	 * is completely wrong for lists, which should default to no selection.
	 *
	 * newrow is set to 0 even when there are no rows, it should really be -1
	 * throughout.  Fixing this will be tedious and time consuming though.
	 */
	if (newrow < 0)
		bonddb_moveto(obj, 0);
	else
		bonddb_moveto(obj, newrow);
	return 0;

}

/**
 * bonddb_filter_byvalue()
 * @param obj : ehhh
 * @param tablename : mahh
 * @param fieldname : ahhh
 * @param fieldvalue : eeerrr
 * 
 * given a roof, filter another roof with that filter. i had a whole lot 
 * of comments about how all this worked but i got bored of reading and 
 * writing it so i deleted it. sorry guys.read the code isntead
 *
 * Did you know that the above comment has no relation at all to 
 * this code? Just making sure you knew that.  Ohhhh the madness.
 *
 * Yeah, hearing about "roof"s really brings back memories... disturbing
 * ones mainly... -- Liam
 *
 * @return  A Number!
 */
gint
bonddb_filter_byvalue(BonddbClass *obj, gchar *tablename, gchar *fieldname,
                        gchar *fieldvalue, sql_condition_operator opr)
{
	g_assert(obj);
	g_assert(tablename);
	g_assert(fieldname);

	debugmsg("Filtering - Field %s.%s for value %s", 
	         tablename, fieldname, fieldvalue); 
	
	if (bonddb_assert_recordset(obj) < 0)
		return -1;
	bonddb_filter_add(obj, tablename, fieldname, fieldvalue, opr);
	
	return bonddb_filter_byvalue_internal(obj, tablename, fieldname, 
			fieldvalue, opr);
}

gint
bonddb_filter_remove(BonddbClass *obj, gchar *tablename, gchar *fieldname)
{
	gboolean found = FALSE;
	BonddbFilterRequest *r;
	GList *walk;
	gint retval=0;
	/* shortcut if only one item */
	if (g_list_length(obj->filterrequest) == 1)
		bonddb_filter_clear(obj);
	/* find item in request list and remove it from list */
	for (walk=g_list_first(obj->filterrequest);walk!=NULL;walk=walk->next)
	{
		r = walk->data;
		if (strcmp(fieldname, r->fieldname) != 0)
			continue;
		if (strcmp(tablename, r->tablename) != 0)
			continue;
		/* m.essage("found filter to remove."); */
		obj->filterrequest = g_list_remove(obj->filterrequest, r);
		bonddb_filter_free_request(r);
		found = TRUE;
		break;
	}
	if (found == FALSE)
		return -1;
	g_hash_table_destroy(obj->filterhash);
	obj->filterhash = NULL;
	
	if (bonddb_assert_recordset(obj) < 0)
		return -1;
	for (walk=g_list_first(obj->filterrequest);walk!=NULL;walk=walk->next)
	{
		r = walk->data;
		/* m.essage("Refiltering %s.%s",r->tablename,r->fieldname); */
		retval -= bonddb_filter_byvalue_internal(obj, r->tablename, 
			r->fieldname, r->value, r->operation);
	}
	return retval;
}

gint
bonddb_filter_currentrow(BonddbClass *obj)
{
	struct _BonddbFindRow *row;
	gint retval=-1;
	row = mem_alloc(sizeof(BonddbRecord));
	row->foundrow = -1;
	row->realrow = obj->currentrow;
	g_hash_table_foreach(obj->filterhash,bonddb_filter_delete_findrowkey,
			row);
	if (row->foundrow != -1)
		retval = row->foundrow;
	mem_free(row);
	return retval;	
}

gint 
bonddb_filter_test(BonddbGlobal *bonddb, gint testnum)
{
	gint retval = 0;
	gint pass = 0;
	gchar *value;
	gchar *errmsg;
	BonddbClass *obj;

	switch (testnum)
	{
	case 50:
		test_announce(testnum,"Select then filter");
                obj = bonddb_new_sql(bonddb, 
				"SELECT * FROM person ORDER BY id", TRUE,  &errmsg);
                bonddb_filter_byvalue(obj, "person", "firstname",
                        "marika", SQL_eq);

                bonddb_getvalue(obj, "person", "firstname",&value);
                test_result(value,"marika",&pass);
                test_result_int(obj->currentrow,2,&pass);
                test_result_int(bonddb_numrecord(obj),3,&pass);
                bonddb_free(obj);
                retval = test_total(pass,3);
		break;
	case 51:
		test_announce(testnum,"Select then filter then delete");
                obj = bonddb_new_sql(bonddb, 
				"SELECT * FROM person ORDER BY id", TRUE,&errmsg);
		if (bonddb_add(obj, "person") != 0)
			errormsg("bonddb_add() failed.");
		bonddb_setvalue(obj, "person", "firstname", "marika",TRUE);
		bonddb_setvalue(obj, "person", "id", "12",TRUE);
		bonddb_setvalue(obj, "person", "lastname", "john",TRUE);
		bonddb_moveto(obj,1);
                bonddb_filter_byvalue(obj, "person", "firstname",
                        "marika", SQL_eq); 
                bonddb_getvalue(obj, "person", "firstname",&value);
                test_result(value,"marika",&pass);
                test_result_int(obj->currentrow,2,&pass);
                test_result_int(bonddb_numrecord(obj),4,&pass);
		bonddb_moveto(obj,bonddb_numrecord(obj)-1);
		bonddb_delete(obj,"person",FALSE);
                test_result_int(bonddb_numrecord(obj),3,&pass);
                bonddb_free(obj);
                retval = test_total(pass,4);
		break;
	case 52:
		test_announce(testnum,"Select then filter then filter again");
                obj = bonddb_new_sql(bonddb, 
				"SELECT * FROM person ORDER BY id", TRUE, &errmsg);
                bonddb_filter_byvalue(obj, "person", "firstname",
                        "marika", SQL_eq);
                test_result_int(bonddb_numrecord(obj),3,&pass);
                bonddb_filter_byvalue(obj, "person", "id",
                        "6", SQL_eq);

                bonddb_getvalue(obj, "person", "firstname",&value);
                test_result_int(obj->currentrow,3,&pass);
                test_result_int(bonddb_numrecord(obj),1,&pass);
                bonddb_free(obj);
                retval = test_total(pass,3);
		break;
	case 53:
		test_announce(testnum,"Select then filter then filter remove");
                obj = bonddb_new_sql(bonddb, 
				"SELECT * FROM person ORDER BY id", TRUE, &errmsg);
                bonddb_filter_byvalue(obj, "person", "firstname",
                        "marika", SQL_eq);
                bonddb_filter_byvalue(obj, "person", "id",
                        "7", SQL_eq);
                test_result_int(obj->currentrow,4,&pass);
		bonddb_add(obj,"person");
		bonddb_filter_remove(obj, "person", "id");
                bonddb_filter_byvalue(obj, "person", "id",
                        "5", SQL_eq);
                test_result_int(obj->currentrow, 2, &pass);
                bonddb_free(obj);
                retval = test_total(pass,2);
		break;
	default:
		break;
	}
	return retval;
}
