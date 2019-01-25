#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "bonddbrecord.h"
#include "bonddbobj.h"
#include "bc.h"

/* create a new BonddbRecord object with @row number of rows */
static BonddbRecord *
bonddb_record_new(BonddbClass *obj, gint row)
{
	gint numcol;
	BonddbRecord *r;
	g_assert(obj);
	g_assert(obj->fieldhash);
	
	if (obj->numcolumn <= 0)
		numcol = g_hash_table_size(obj->fieldhash);
	else
		numcol = obj->numcolumn;
	
	r = mem_calloc(sizeof(BonddbRecord));
	r->visible = TRUE;
	r->parent = obj;
	r->row = row;
	r->mode = obj->mode;
	r->cache = mem_calloc(sizeof(void*) * (numcol+1));
	r->numcache = numcol; 
	return r;
}

/* free the memory allocated for a BonddbRecord object r */
static void
bonddb_record_free(BonddbRecord *r)
{
	gint i;
	if (r->id)
		bonddb_id_free(r->id);
	for (i=0;i<r->numcache;i++)
		if (r->cache[i])
			mem_free(r->cache[i]);
	mem_free(r->cache);
	mem_free(r);
}

/**
 * bonddb_record_freeall()
 * @param obj : Database object
 *
 * Free the memory allocated for each object associated with each
 * BonddbRecord object  
 *
 */
void
bonddb_record_freeall(BonddbClass *obj)
{
	gint i;
	if (obj->record == NULL)
		return;
	for (i=0;i<obj->numrecord;i++)
	{
		if (obj->record[i] == NULL)
			continue;
		bonddb_record_free(obj->record[i]);
	}
	mem_free(obj->record);
	obj->record = NULL;
	obj->currentrecord = NULL;
	obj->numrecord = 0;
}

/**
 * db_record_populate()
 * @param obj : database object
 * @param row : 
 * 
 * Creates a cache record item in the @obj for @row.  Any setvalue or 
 * getvalue calls will automatically go into this cache for faster
 * retival in future. This function is called as you navigate
 * though the record set in @obj.
 * 
 * @return  non-zero on failure
 */
BonddbRecord *
bonddb_record_populate(BonddbClass *obj, gint row)
{
	g_assert(obj);

	switch (obj->mode)
	{
	case BONDDB_INSERT:
	case BONDDB_INSERT_SAVED:
	case BONDDB_UPDATE:
	case BONDDB_READ:
		break;
	default:
		errormsg("invalid state in populate record.");
		return NULL;
	}

	g_assert(obj->fieldhash);

	/* Allocate the record buffer. This is incremented every 100
	 * records cached. */
	if (obj->record == NULL)
		obj->record = mem_calloc(sizeof(void*) * 
				(obj->numrecord + 102));
	else
	if (obj->numrecord % 100 == 0 && obj->mode == BONDDB_INSERT 
		&& obj->numrecord > 0)
	{
		obj->record = mem_realloc(obj->record,sizeof(void*) * 
				(obj->numrecord + 102));
		memset((void*)obj->record+(obj->numrecord*sizeof(void*)), 
					0, 100*sizeof(void*));
		/* m.essage("Expanding bonddb record array because max reached"); */
	}

	if (obj->numrecord == 0)
		return NULL;

	if (obj->record[row] == NULL)
		obj->record[row] = bonddb_record_new(obj, row);
	
	return obj->record[row];
}

/* Checks whether (row,fieldpos) is empty. Returns non-zero on failure.
 */
static gint
bonddb_record_assert(BonddbClass *obj, gint row, gint fieldpos)
{
	g_assert(obj);
	if (obj->record == NULL)
		return -1;
	g_assert(row >= 0 && row < obj->numrecord);
	g_assert(fieldpos >= 0 && fieldpos < obj->numcolumn);
  
	if (obj->record[row] == NULL)
		return -2;
	if (obj->state == BONDDB_INVALID)
		return -3;
	mem_verify(obj->record[row]);
	return 0;
}

/**
 * db_field_read()
 * @param obj : Database Object
 * @param fieldpos : Field to look up
 * @param row : Row to look up
 * @param retstr : Where to store the record after reading
 *
 * Will read current recordset based on your currently selected row in the 
 * object to get the value asked for in the @fieldpos.
 * 
 * @return  non-zero on failure.
 */
gint
bonddb_record_read(BonddbClass *obj, gint fieldpos, 
		   gint row, gchar **retstr)
{
	gint retval;
	*retstr = NULL;
	if (obj->ignoreassert == FALSE)
	{
		if ((retval = bonddb_record_assert(obj,row,fieldpos)))
			return retval;
	}
	else
		if (obj->record[row] == NULL)
			return -2;
  
	if (obj->record[row]->cache[fieldpos])
		*retstr = obj->record[row]->cache[fieldpos];

	return 0;
}

/**
 * db_field_read()
 * @param obj : Database Object
 * @param fieldpos : Field to look up
 * @param row : Row to look up
 * @param value : The value to write
 *
 * Will write value to the row and fielpos in the database object.  This value is 
 * cached, so that future reads do not need to go to the database.
 * 
 * @return  non-zero on failure.
 */
gint
bonddb_record_write(BonddbClass *obj, gint fieldpos, gint row, gchar *value, gchar **retvalue)
{
	gint retval;
	if (obj->ignoreassert == FALSE)
	{
		if ((retval = bonddb_record_assert(obj,row,fieldpos)))
			return retval;
		if (obj->record[row]->cache[fieldpos])
			mem_free(obj->record[row]->cache[fieldpos]);
	}

	if (value)
		obj->record[row]->cache[fieldpos] = mem_strdup(value);
	else
		obj->record[row]->cache[fieldpos] = NULL;
	*retvalue = obj->record[row]->cache[fieldpos];
	return 0;
}

/**
 * bonddb_record_delete()
 * @param obj :
 * @param row : Row to delete
 *
 * Removes the record cache from the object. 
 */
gint
bonddb_record_delete(BonddbClass *obj, gint row, gchar *tablename)
{
	gint i, newrow;
	g_assert(obj);

	if (row >= obj->numrecord || row < 0)
	{
		warningmsg("Invalid row %d 0...%d",row,obj->numrecord);
		return -1;
	}
	newrow = row;
	if (newrow >= obj->numrecord-1)
		newrow = row - 1;
	
	bonddb_filter_delete(obj, row);
	if (newrow < 0 && obj->numrecord > 0)
		newrow = 0;
	
	bonddb_record_free(obj->record[row]);
	obj->record[row] = NULL;
	for (i=row;i<obj->numrecord-1;i++)
		obj->record[i] = obj->record[i+1];
	obj->record[obj->numrecord-1] = NULL;
	obj->numrecord = obj->numrecord - 1;

	/* If currently selected row blank stuff */
	if (obj->currentrow == row && newrow >= 0 && obj->numrecord > 0)
	{
		obj->currentid = NULL;
		obj->currentrecord = NULL;
		/*obj->currentid = bonddb_id_get(obj, tablename, newrow); */
		/* Moveto is done after this function. Doing it here
		 * will cause problems because the obj->res is stale and
		 * does not match what the actual data is. */
		/*bonddb_moveto(obj, newrow); */
		obj->currentrow = newrow;
	}
	else if (obj->currentrow == row && obj->numrecord <= 0)
	{
		/* this is to handle deletion of last record. */
		obj->currentrecord = NULL;
		obj->currentid = NULL;
		if (obj->tmpid)
			bonddb_id_free(obj->tmpid);
		obj->tmpid = NULL;
		obj->currentrow = newrow;
	}
	return 0;
}

gint 
bonddb_record_undo(BonddbClass *res, gint row)
{
	gint i;
	BonddbRecord *r;
	if (res->numrecord <= row)
		return -1;
	if (row < 0)
		return -2;
	if (res->record[row] == NULL)
		return -3;
	r = res->record[row];
	for (i=0;i<r->numcache;i++)
		if (r->cache[i])
		{
			mem_free(r->cache[i]);
			r->cache[i] = NULL;
		}
	
	return 0;
}

static void
bonddb_record_debug_item(BonddbRecord *r)
{
	gint i;
	if (r == NULL)
	{
		debug_output("Record NULL\n");
		return ;
	}
	debug_output("Row: %d Cached: %d Visible: %d Mode %d\n",
		r->row,r->cached,r->visible,r->mode);
	for (i=0;i<r->numcache;i++)
	{
		debug_output("\t%s\n",(gchar*)r->cache[i]);
	}
}

void
bonddb_record_debug(BonddbClass *res)
{
	gint i;
	for (i=0;i<res->numrecord;i++)
		bonddb_record_debug_item(res->record[i]);
}


