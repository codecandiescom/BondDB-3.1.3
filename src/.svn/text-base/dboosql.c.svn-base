#include <stdlib.h>
#include <glib.h>

#include "dboosql.h"
#include "bc.h"
#include "db.h"
#include "dbobject.h"
#include "dbsqlparse.h"

#ifdef __PORTED

gint
db_oo_generatesql(Object * obj, gchar * table)
	{
	OOPath *path;

	g_assert(obj);
	g_assert(table);
	g_assert(obj->table->name);
	/* todo, add code to support NULL query. may not be added here but somwhere else */

	path = db_oo_generatepath(table, obj->table->name);
	if (path == NULL)
		{
		errormsg("Path is null, so hmm. strange things");
		return -1;
		}
	return db_oo_generatesql_addpath(obj, path);
	}

/* This is one bad arse function. Hmmm */
gint
db_oo_generatesql_addpath(Object * obj, DbPath * path)
	{
	DbPathElement *elem;
	GList *retlist = NULL, *walk;
	gint pos;
	gchar *tmpstr, retquery;
	gchar *query = obj->query;

	g_assert(query);

	tmpstr = mem_strdup_printf("%s.%s", path->finaltable, path->field);
	retquery = db_sql_addfield(query, tmpstr, &pos);
	mem_free(tmpstr);

	/* Check to see if the table has a join already present in the system */
	walk = g_list_first(path->tablepath);
	while (walk != NULL)
		{
		elem = walk->data;
		/* See if table is already mapped into sql statement. */
		if (db_sql_checkforjoin(retquery, elem->c->table[1 - elem->direction], elem->c->table[elem->direction]) != 0)
			{
			/* Examples SQL statement select rank.*, country.name from rank left outer join country on
			   rank.countryid=country.id; c->table[0] left join c->table[1] on c->column[0]=c->column[1]; select
			   rank.*, country.name from rank left outer join insurance on rank.insuranceid = insurance.id left
			   outer join country on country.id=insurance.countryid; */
			if (elem->direction == 1)
				retquery = db_sql_addjoin(query, elem->c->table[0], elem->c->table[1],
				                          "LEFT OUTER JOIN", elem->c->column[0], elem->c->column[1]);
			/* Examples of SQL statement select country.*, rank.description from country left join rank on
			   rank.countryid=country.id; c->table[1] left join c->table[0] on c->column[1]=c->column[0]; */
			if (elem->direction == 0)
				retquery = db_sql_addjoin(query, elem->c->table[1], elem->c->table[0],
				                          "LEFT OUTER JOIN", elem->c->column[1], elem->c->column[0]);
			}
		walk = walk->next;
		}

	return 0;
	}

/* Go though query and look for a join on tablefrom to tableto.
 *	ie c->table[0] left join c->table[1] on c->column[0]=c->column[1]; 
 */
gint
db_sql_checkforjoin(gchar * query, gchar * tablefrom, gchar * tableto)
	{

	return 0;
	}

/* Add the join into the sql statement.
 *
 */
gchar *
db_sql_addjoin(gchar * query, gchar * tablefrom, gchar * tableto, gchar * jointype, gchar * fieldfrom, gchar * fieldto)
	{

	return NULL;
	}

gchar *
db_sql_addfield(gchar * query, gchar * tmpstr, int *pos)
	{
	return NULL;
	}

#endif
