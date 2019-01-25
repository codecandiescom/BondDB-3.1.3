#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include "dbfilter.h"
#include "db.h"
#include "dbtoliet.h"
#include "dbfield.h"

static void
db_filter_freerequest(GList * requestlist)
	{
	DbFilterAppend *fa;
	GList *walk;

	for (walk = g_list_first(requestlist); walk != NULL; walk = walk->next)
		{
		fa = walk->data;
		mem_free(fa->fieldname);
		mem_free(fa->tablename);
		mem_free(fa->value);
		mem_free(fa);
		}

	g_list_free(requestlist);
	}

/**
 * db_filter_free:
 *
 * Free filter element in the cache.
 */

gint
db_filter_free(DbFilter * filter)
	{
	mem_free(filter);
	return 0;
	}

/**
 * db_filter_create:
 *
 * Create a filter.
 */

DbFilter *
db_filter_create(gboolean active, gint sortedpos)
	{
	DbFilter *f;

	f = mem_alloc(sizeof(DbFilter));
	f->active = active;
	f->sortedpos = sortedpos;
	return f;
	}

/**
 * db_filter_createpos:
 * @obj: 
 *
 * Goes though and puts the raw posistion in the list, based on if the widget 
 * is active or not.
 *
 * Returns: 0 if success.
 */
gint
db_filter_createpos(Object * obj)
	{
	int i, count = 0;

	g_assert(obj);

	for (i = 0; i < obj->numcache; i++)
		{
		if (obj->cache[i] == NULL)
			continue;
		if (obj->cache[i]->filter->active == TRUE)
			{
			obj->cache[i]->filter->sortedpos = count;
			count++;
			}
		else
			obj->cache[i]->filter->sortedpos = -1;
		}
	return 0;
	}

/**
 * db_filter_sortbypos:
 * @obj: The object to be sorted
 * @fieldname: The name of the field to be sorted
 * @tablename: The name of the table that has the field to be sorted
 * @order_ascending: TRUE if ascending, FALSE if descending
 *
 * This sort function is for an object which cache has been loaded.
 * In other word, use this on an object after you called db_filter_load() on
 * the object just like when you db_filter_filterbyvalue().
 * The reason is that db_filter_load() does cacheing stuff for you.
 * NOTE: At this moment, the function can only sort on simple numeric integer
 * field.
 * Written by Francis
 *
 * Returns: %0 if successful. %-3 if failed to sort (in detail, if this failed
 * to move very first row
 **/
gint
db_filter_sortbypos(Object * obj, gchar * fieldname, 	/* Sort by this field */
                    gchar * tablename, 	/* Which table the field belong */
                    gboolean order_ascending)
	{
	gint i, j;
	gint pos;
	gint count = 0;
	int validcachecount;
	int invalidcachecount;

	gint *idx_array;

	static int comp(const void *a, const void *b);

	/* Sort index by its cache array's specified position value */
	static int comp(const void *a, const void *b)
		{
		gint aa, bb;

		aa = *((gint *) a);
		bb = *((gint *) b);
		/* printf ("comp (%d, %d)\n", aa, bb); */
		return (order_ascending) ?
		       atoi(obj->cache[aa]->value[pos]) -
		       atoi(obj->cache[bb]->value[pos]) : atoi(obj->cache[bb]->value[pos]) -
		       atoi(obj->cache[aa]->value[pos]);
		}

	/* Assertions */
	g_assert(obj);
	g_assert(fieldname);
	g_assert(tablename);

	g_assert(obj->numcache == obj->num);

	pos = db_field_getpos(obj, fieldname, tablename);

	puts("***********************************************");
	printf("Field '%s' position in table '%s'= %dth\n", fieldname, tablename,
	       pos + 1);
	puts("***********************************************");

	puts("***********************************************");
	printf("Num of cache: %d\n", obj->numcache);
	puts("***********************************************");

	/* Set the array to be sorted */

	/* printf ("Number of object cache: %d\n", obj->numcache); */
	idx_array = (gint *) mem_alloc(sizeof(gint) * obj->numcache);

	/* Francis: Ok, here is the story of why I am using valid cache and chekcing
	   the validity of a cache. Basically, this mechanism is required when you
	   add a new row. Then suddenly one invalid row (because this new row hasn't 
	   got proper field values yet) will be sitting at the last row (could be
	   in the middle, hmm). Thus, we need to skip this row. Example: => should
	   change to Validrow with value 2 Validrow with value 1 Invalidrow(1) with
	   value ? Validrow with value 2 Validrow with value 3 Validrow with value 3 
	   Validrow with value 1 Invalidrow(1) with value ? Invalidrow(2) with valu
	   ? Invalidrow(2) with value ? As you can see above, invalid row should all 
	   go to the back of the rows. */

	/* Init the array */
	invalidcachecount = validcachecount = 0;
	for (i = 0; i < obj->numcache; i++)
		{
		/* printf ("cache num[%d]: state(0:READ,1:EDITREAD,2:NEW,3:EDITNEW) =>
		   %d, changed => %s\n", i, obj->cache[i]->state,
		   (obj->cache[i]->changed)?"TRUE":"FALSE"); */
		/* If the state of the cache is just OBJ_READ (cache just has been
		   read) or OBJ_EDITREAD (cache has been read and then edited), I
		   assume the sortting field values already exist with a valid value. */
		if ((obj->cache[i]->state == OBJ_READ ||
		        obj->cache[i]->state == OBJ_EDITREAD)
		        && obj->cache[i]->value[pos] != NULL)
			{

			idx_array[validcachecount++] = i;
			}
		else
			{
			/* Farncis: Fill invalid cache row from the last index. I am
			   trying to be clever at this moment. I don't want another loop 
			   only to sort out these invalid cache row. All this silly
			   thing was derived from my assumption there could be more than 
			   one invalid row. However, it could be useful it the row
			   allows NULL value */
			int lastindex;

			lastindex = obj->numcache - 1;	/* Just to clearify */
			idx_array[lastindex - invalidcachecount++] = i;
			}

		/* printf ("%d's value = %s\n", i, obj->cache[i]->value[pos]); */

		}

	/* puts(""); */

	debugmsg("Valid number of cache rows: %d", validcachecount);
	qsort(idx_array, validcachecount, sizeof(gint), comp);

	/* For each row which has a valid cache field to sort */
	for (i = 0; i < validcachecount; i++)
		{
		/* Remember that the index array element from 0 to validcachecount-1
		   contain sorted indice */
		obj->cache[idx_array[i]]->filter->active = TRUE;
		obj->cache[idx_array[i]]->filter->sortedpos = i;
		count++;
		}

	/* For each row... */
	for (i = validcachecount, j = obj->numcache - 1; i < obj->numcache; i++, j--)
		{
		obj->cache[idx_array[j]]->filter->sortedpos = i;
		count++;
		}

	for (i = 0; i < obj->numcache; i++)
		{
/*		debugmsg("row id: %s, sortedpos: %d\n", obj->cache[i]->value[0],
		         obj->cache[i]->filter->sortedpos); */
		}

	/*
	   if (db_moveto(obj, 0) != 0) { warningmsg("Unable to move to the orginal
	   record."); return -3; } */

	db_filter_checkvisible(obj);

	return 0;
	}

/**
 * db_filter_filterbyvalue:
 * @roof: ehhh
 * @fieldname: ahhh
 * @fieldvalue: eeerrr
 * 
 * given a roof, filter another roof with that filter. i had a whole lot of comments
 * about how all this worked but i got bored of reading and writing it so i deleted it.
 * sorry guys.read the code isntead
 *
 * Did you know that the above comment has no relation at all to this code? Just making
 * sure you knew that.  Ohhhh the madness.
 *
 * Returns: A Number!
 */
gint
db_filter_filterbyvalue(Object * obj, gchar * fieldname, gchar * tablename,
                        gchar * fieldvalue)
	{
	gint i, c = 0;
	gint datapos = -1;
	gchar *value;

	g_assert(obj);
	g_assert(fieldname);
	g_assert(fieldvalue);
	obj->currentcache = NULL;

	debugmsg("Filtering list %s, field %s for value %s", obj->basetable,
	         fieldname, fieldvalue);

	if (obj->cache == NULL || obj->numcache <= 0)
		{
		errormsg("filter is not set up so can not filter.");
		return -1;
		}

	if (db_iscacheloaded(obj) == FALSE)
		{
		errormsg
		("The cache isn't loaded, you need to load all your records first");
		return -2;
		}
	db_filter_applied_addfilter(obj, fieldname, tablename, fieldvalue);

	datapos = db_field_getpos(obj, fieldname, tablename);

	/* Ohh no its not show, so go load it. Assumes filter and object are in sync
	 */
	if (obj->numcache > 0 && datapos >= 0
	        && obj->cache[0]->flags[datapos] == FILTER_EMPTY)
		{
		debugmsg("loading an extra field so i can filter here");
		for (i = 0; i < obj->num; i++)
			{
			db_moveto(obj, i);
			if (db_getvalue(obj, fieldname, tablename, &value) != 0)
				{
				warningmsg("failed to extract field %s", fieldname);
				break;
				}
			obj->cache[i]->flags[datapos] = FILTER_HIDDEN;
			}
		if (obj->num <= 0)
			debugmsg("zero length list, arr, so can't really filter this");
		}

	for (i = 0; i < obj->numcache; i++)
		{
		if (obj->cache[i] == NULL || obj->cache[i]->value[datapos] == NULL)
			continue;
		if (strcmp(obj->cache[i]->value[datapos], fieldvalue) == 0)
			{
			obj->cache[i]->filter->active = TRUE;
			c++;
			}
		else
			{
			obj->cache[i]->filter->active = FALSE;
			}
		}
	db_filter_createpos(obj);
	db_filter_checkvisible(obj);

	debugmsg("filter has now been finished, with %d of %d items showing", c,
	         obj->num);

	return 0;
	}

/**
 * db_filter_addrequestlist:
 * 
 * Use in the format retlist = db_filter_addrequest(retlist,"fieldname","tablename");
 * Will add a desired field to the cache, which could be filtered on.  This is
 * useful for speeding up load times.
 */
GList *
db_filter_addrequestlist(GList * requestlist, gchar * fieldname,
                         gchar * tablename)
	{
	DbFilterAppend *request;

	request = (DbFilterAppend *) mem_alloc(sizeof(DbFilterAppend));
	memset(request, 0, sizeof(DbFilterAppend));
	request->tablename = mem_strdup(tablename);
	request->fieldname = mem_strdup(fieldname);
	request->flags = FILTER_SHOW;
	request->fieldpos = -1;
	requestlist = g_list_append(requestlist, request);
	return requestlist;
	}

/**
 * db_filter_addrequestlist:
 * 
 * 
 * 
 */
GList *
db_filter_addrequestlist_hidden(GList * requestlist, gchar * fieldname,
                                gchar * tablename)
	{
	DbFilterAppend *request;

	request = (DbFilterAppend *) mem_alloc(sizeof(DbFilterAppend));
	memset(request, 0, sizeof(DbFilterAppend));
	request->tablename = mem_strdup(tablename);
	request->fieldname = mem_strdup(fieldname);
	request->flags = FILTER_HIDDEN;
	request->fieldpos = -1;
	requestlist = g_list_append(requestlist, request);
	return requestlist;
	}

/**
 * db_filter_clear:
 *
 * Reshow all variables and unfilter everything.
 * 
 */

gint
db_filter_clear(Object * obj)
	{
	int i;

	for (i = 0; i < obj->numcache; i++)
		if (obj->cache[i] != NULL)
			{
			obj->cache[i]->filter->sortedpos = obj->cache[i]->origrow;
			obj->cache[i]->filter->active = TRUE;
			}
	return 0;
	}

/**
 * db_filter_movepos:
 * @obj: 
 * 
 * Works out what position to move to
 */

gint
db_filter_moveto(Object * obj, gint row)
	{
	gint i;

	g_assert(obj);

	/* g_assert(row >=0 && row < obj->num); */
	/* Francis: Make more harsh check */
	/* removed by dru cause it breaks stuff */
	/* g_assert(obj->filtered == TRUE && row >= 0 && row < obj->num); */

	if (obj->filtered == FALSE)
		{
		debugmsg("no filter present");
		return row;
		}
	if (row < 0)
		{
		warningmsg("invalid row to move to");
		return -1;
		}
	for (i = 0; i < obj->numcache; i++)
		{
		if (obj->cache[i]->filter->sortedpos == row)
			{
			obj->currentcache = obj->cache[i];
			/* debugwin ("For asking row %d, true cache pos: %d. Origrow:
			   %d", row, i, obj->cache[i]->origrow) */
			/* m.essage("returning pos %d for row %d,
			   i=%d",obj->cache[i]->filter->sortedpos,row,i); */ 
			return obj->cache[i]->origrow;
			}
		}
	warningmsg("Unable to move to row %d in basetable recordset %s", row,
	           obj->basetable);
	db_cache_debug(obj);
	errormsg("I'm going to die now as an act of cruelness");
	g_assert(NULL);
	return -2;
	}

gint
db_filter_num(Object * obj)
	{
	gint i, num = 0;

	g_assert(obj);

	if (obj->filtered == FALSE)
		return obj->num;
	for (i = 0; i < obj->numcache; i++)
		if (obj->cache[i]->filter->active == TRUE)
			num++;
	return num;
	}

/**
 * db_filter_removefilter:
 * 
 * Removes filtering completely from the list.  
 */
void
db_filter_removefilter(Object * obj)
	{
	obj->filtered = FALSE;

	}

/**
 * db_filter_getatpos:
 * @obj:
 * @pos:
 * 
 * 
 */
gchar *
db_filter_getatpos(Object * obj, gint pos)
	{
	g_assert(obj);
	if (obj->currentcache == NULL)
		return NULL;
	if (pos < 0 || pos >= obj->currentcache->num)
		{
		errormsg("invalid pos of %d", pos);
		return NULL;
		}
	return obj->currentcache->value[pos];
	}

/**
 * db_filter_checkvisible:
 *
 * Not written yet.
 */
gint
db_filter_checkvisible(Object * obj)
	{
	return 0;
	}

/**
 * db_filter_setfilterstate:
 * 
 * set the state on the object filtering
 */
void
db_filter_setfilterstate(Object * obj, gboolean state)
	{
	g_assert(obj);
	obj->filtered = state;
	}

gint
db_filter_resort(Object * obj, gint pos)
	{
	gint retval = 0;

	if (obj->filtered == TRUE)
		db_filter_setfilterstate(obj, FALSE);
	if (obj->filterapplied == NULL)
		{
		warningmsg("no filter applied. can't do this");
		return -1;
		}
	obj->filterapplied->internalaction = 1;
	db_filter_load(obj, obj->filterapplied->requestlist);
	db_filter_createpos(obj);

	/*	for (walk = g_list_first(obj->filterapplied->filters); walk != NULL; walk = walk->next)
			{
			fa = walk->data;
			retval -= db_filter_filterbyvalue(obj, fa->fieldname, fa->tablename, fa->value);
			}*/
	obj->filterapplied->internalaction = 0;
	return retval;
	}

/**
 * db_filter_freeapplied:
 * 
 * Free the applied filters list on an object.
 *  
 */
void
db_filter_freeapplied(Object * obj)
	{
	DbFilterAppend *fa;
	GList *walk;

	if (obj->filterapplied != NULL)
		{
		/* free filters */
		for (walk = g_list_first((GList*)obj->filterapplied->filters); 
			walk != NULL; walk = walk->next)
			{
			fa = walk->data;
			mem_free(fa->fieldname);
			mem_free(fa->tablename);
			mem_free(fa->value);
			mem_free(fa);
			}
		g_list_free(obj->filterapplied->filters);
		/* free list of request fields to display */
		db_filter_freerequest(g_list_first(obj->filterapplied->requestlist));
		mem_free(obj->filterapplied);
		}
	obj->filterapplied = NULL;
	}

/**
 * db_filter_applied_addrequest:
 * 
 * Copy the requestlist onto an object for future reference
 * 
 */
void
db_filter_applied_addrequest(Object * obj, GList * requestlist)
	{
	g_assert(obj);
	if (obj->filterapplied == NULL)
		{
		obj->filterapplied = mem_calloc(sizeof(DbFilterApplied));
		}
	if (obj->filterapplied->internalaction == 1)
		return ;

	if (requestlist == obj->filterapplied->requestlist)
		return ;
	else if (obj->filterapplied->requestlist != NULL)
		db_filter_freerequest(g_list_first(obj->filterapplied->requestlist));

	obj->filterapplied->requestlist = requestlist;
	}

/**
 * db_filter_applied_addfilter:
 * @obj: database object
 * @field: 
 * @table:
 * @value:
 *
 * 
 */
void
db_filter_applied_addfilter(Object * obj, gchar * field, gchar * table,
                            gchar * value)
	{
	DbFilterAppend *fa;

	g_assert(obj);
	if (obj->filterapplied == NULL)
		{
		obj->filterapplied = mem_alloc(sizeof(DbFilterApplied));
		memset(obj->filterapplied, 0, sizeof(DbFilterApplied));
		}
	if (obj->filterapplied->internalaction == 1)
		return ;

	fa = (DbFilterAppend *) mem_alloc(sizeof(DbFilterAppend));
	memset(fa, 0, sizeof(DbFilterAppend));
	fa->fieldname = mem_strdup(field);
	fa->tablename = mem_strdup(table);
	fa->value = mem_strdup(value);
	obj->filterapplied->filters = g_list_append(obj->filterapplied->filters, fa);
	}

/**
 * db_filter_getrowinfilter:
 * 
 * Work out what position you are in a filtered object.  
 * 
 */
gint
db_filter_getrowposinfilter(Object * obj, gint * row)
	{
	gint i, count = 0;

	g_assert(obj);
	*row = -1;
	for (i = 0; i < obj->numcache; i++)
		{
		if (obj->cache[i]->filter->active == FALSE)
			continue;
		if (obj->row == obj->cache[i]->origrow)
			{
			*row = count;
			return 0;
			}
		count++;
		}
	return -1;
	}

/**
 * db_filter_deletedrow:
 * @obj: Database object
 * @row: Old row pos that was deleted, and filter needs to be updated around it.
 * 
 * Resort the cache list to accommidate a deleted record.
 */
gint
db_filter_deletedrow(Object * obj, gint row)
	{
	gint i, oldpos = 0;

	g_assert(obj);
	if (obj->filtered == FALSE)
		return 1;
	if (row >= 0 && row <= obj->numcache)
		oldpos = obj->cache[row]->filter->sortedpos;
	else
		return -1;
	/* m.essage("deleting row %d, oldpos %d",row,oldpos); */
	for (i = 0; i < obj->numcache; i++)
		if (obj->cache[i] != NULL)
			if (obj->cache[i]->filter->sortedpos >= oldpos)
				obj->cache[i]->filter->sortedpos =
				    obj->cache[i]->filter->sortedpos - 1;
	obj->cache[row]->filter->sortedpos = -1;
	obj->cache[row]->filter->active = FALSE;
	return 0;
	}

static gboolean
db_filter_apply_append(Object * obj, DbFilterAppend * append, gchar * value)
	{
	glong a = 0, b = 0;

	/* Handle empty data */
	if (value == NULL || append->value == NULL)
		{
		if (append->operation == SQL_eq && append->value == value)
			return TRUE;
		else if (append->operation == SQL_not && append->value != value)
			return TRUE;
		return FALSE;
		}
	/* operation can be: OprEqual, OprNotEqual, OprLike, OprGreater, OprLess,
	   OprGreaterEqual, OprLessEqual */
	if (append->operation > 1)
		{
		a = atoi(value);
		b = atoi(append->value);
		}

	switch (append->operation)
		{
	case SQL_eq:
		if (strcmp(append->value, value) == 0)
			return TRUE;
		break;
	case SQL_not:
		if (strcmp(append->value, value) != 0)
			return TRUE;
		break;
	case SQL_like:
		if (strstr(value, append->value))
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

/**
 * db_filter_apply:
 * Object: database object which already has a filterrequest loaded for it.
 * 
 * Applies the filter stored in @obj to its current dataset.
 * 
 * Returns: non-zero on error.
 */

gint
db_filter_apply(Object * obj)
	{
	DbFilterAppend *append;
	GList *walk;
	gint i, numrules = 0, count;

	g_assert(obj);
	if (!obj->filterapplied || !obj->filterapplied->requestlist)
		{
		errormsg("Can not apply filter because no filter is specified");
		return -1;
		}
	obj->filterapplied->requestlist =
	    g_list_first(obj->filterapplied->requestlist);
	for (walk = obj->filterapplied->requestlist; walk != NULL; walk = walk->next)
		{
		append = walk->data;
		if (append->fieldpos < 0 || append->fieldpos > obj->numfield)
			{
			errormsg("Fieldpos is invalid, can not filter");
			return -1;
			}
		numrules++;
		}

	for (i = 0; i < obj->numcache; i++)
		{
		if (obj->cache[i] == NULL)
			continue;
		count = 0;
		for (walk = obj->filterapplied->requestlist; walk != NULL;
		        walk = walk->next)
			{
			append = walk->data;
			if (db_filter_apply_append
			        (obj, append, obj->cache[i]->value[append->fieldpos]))
				count++;
			}
		if (count == numrules)
			obj->cache[i]->filter->active = TRUE;
		else
			obj->cache[i]->filter->active = FALSE;
		}

	/* Number the filtered rows */
	db_filter_createpos(obj);

	return 0;
	}

/**
 * db_filter_load:
 * @obj:
 * @requestlist:
 * 
 * Move though all records, extract the data as desired by @requestlist.
 * 
 */
gint
db_filter_load(Object * obj, GList * requestlist)
	{
	gint i, origrow;
	gchar *value;
	GList *walk;
	DbFilterAppend *request;

	g_assert(obj);

	db_filter_applied_addrequest(obj, requestlist);

	origrow = obj->row;
	if (obj->id == NULL)
		{
		warningmsg("no id is set for obj->id");
		return -1;
		}
	obj->currentcache = NULL;

	requestlist = g_list_first(requestlist);
	for (i = 0; i < obj->num; i++)
		{
		/* debugmsg("Pos %d of %d",i,obj->num); */
		obj->filtered = FALSE;
		if (db_moveto(obj, i) < 0)
			{
			warningmsg("Unable to move to record pos %d in %s. Error %d", i,
			           obj->basetable, db_moveto(obj, i));
			return -2;
			}
		obj->filtered = TRUE;
		for (walk = requestlist; walk != NULL; walk = walk->next)
			{
			request = walk->data;
			if (request->fieldpos == -1)
				request->fieldpos =
				    db_field_getpos(obj, request->fieldname,
				                    request->tablename);
			if (db_getvalue
			        (obj, request->fieldname, request->tablename, &value) != 0)
				{
				warningmsg("Failed to extract field %s of table %s",
				           request->fieldname, request->tablename);
				break;
				}
			if (request->fieldpos >= 0)
				obj->currentcache->flags[request->fieldpos] =
				    (gint) request->flags;
			if (request->value)
				{
				if (db_filter_apply_append(obj, request, value))
					obj->cache[i]->filter->active = TRUE;
				else
					obj->cache[i]->filter->active = FALSE;
				}
			}
		}
	obj->filtered = FALSE;
	if (db_moveto(obj, origrow) != 0)
		{
		warningmsg("Unable to move to the orginal record.");
		return -3;
		}
	obj->filtered = TRUE;

	return 0;
	}
