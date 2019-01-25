#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "bonddbobj.h"
#include "bonddbrecord.h"
#include "bonddbarray.h"

#include "bc.h"

/**
 * bonddb_array_extract()
 * @param obj : database object
 * @param fields : A glist array of all the field names you want, teh next
 * @param rows : returned number of rows in the list.
 * @param cols : returned number of columns, should be
 * @param getid : If id's are needed to be extraced also.
 *
 * Creates an array of dimisons @rows x @cols containing the values in cache
 * of each field and table requested in @fields. @fields must be a glist
 * of strings containting both the field and corresponding table names. Create
 * it with every first entry being a tablename, second entry being fieldname. 
 *
 * @return  Freshly created array which you must free using mem_free, do not
 * free the individual elements in this array.
 */
gchar **
bonddb_array_extract(BonddbClass *obj, GList *fieldlist, gint *retnumrow, 
		gint *retnumcol, gboolean getid)
	{
	gchar *value;
	GList *walk;
	gchar **retarr = NULL;
	gint i, oldpos, numrow, numcol, colcount;

	g_assert(obj);
	*retnumrow = 0;
	*retnumcol = 0;
	
	if (fieldlist == NULL || g_list_length(fieldlist) % 2 != 0)
	{
		errormsg("Invalid fields list passed in. Should be in "
			"table then fieldname format. %d items.",
			g_list_length(fieldlist));
		return NULL;
	}
	if (bonddb_isrecordset(obj) == FALSE)
	{
		warningmsg("No record set exist so can not extract values");
		return NULL;
	}
	oldpos = obj->currentrow;
	fieldlist = g_list_first(fieldlist);
	
	if (obj->firsttable == NULL && obj->query)
		bonddb_parsesql(obj);
	/* g_assert(obj->firsttable); */
	if (obj->firsttable == NULL)
	{
		warningmsg("obj->firsttable is NULL. This is readonly now");
		getid = FALSE;
	}
	numrow = bonddb_numrecord(obj);
	numcol = g_list_length(fieldlist) / 2;

	retarr = mem_calloc(sizeof(void *) * (numrow * numcol));
	for (i = 0; i < numrow; i++)
		{
		bonddb_moveto(obj, i);
		colcount = 0;
		for (walk=fieldlist;walk!=NULL;walk=walk->next)
		{
			if (bonddb_getvalue(obj,walk->data,walk->next->data,
					&value) != 0)
			{
				errormsg("Failed to extract data from "
					"%s.%s. Aborting read.",
					(char*)walk->data,(char*)walk->next->data);
				mem_free(retarr);
				return NULL;
			}
			retarr[(i * numcol) + colcount] = value;
			/* debugmsg("Returning %d %d.%d %s", (i * numcol) + colcount, i,colcount, value); 
			fflush(NULL); */
			colcount ++;
			walk=walk->next;
		}
		/* Only try and get the id once. else ignore lack of id. */
		if (getid)
			if (!bonddb_id_get_currentrow(obj, obj->firsttable))
				getid = FALSE;
		
		/* After the first attempts at reading information assume that
		   everything is how it should be. */
		/* if (i == 0)
			obj->ignoreassert = TRUE; */
		}

	obj->ignoreassert = FALSE;
	if (numrow > 0)
		bonddb_moveto(obj,oldpos);
	*retnumrow = numrow;
	*retnumcol = numcol;
	return retarr;
	}

