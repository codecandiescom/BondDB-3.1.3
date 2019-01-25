#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "db.h"
#include "dblooksee.h"
#include "dbtoliet.h"

/* NOW OBSOLETE, PLEASE DO NOT USE */

/* PLEASE NOTE:
 * This function needs to be ported over to the new sql parser at some point. just thought
 * i'd pre-warn you about it, thats all.
 * At the moment its manaully parsing, well you know. lots of for and whiles and cats and
 * all that nasty business. but it works. promise.
 */

gchar *
db_looksee_gettype(gint type)
	{
	gchar *str = NULL;

	switch (type)
		{
	case DBSEARCH_EQUAL:
		str = mem_strdup_printf("=");
		break;
	case DBSEARCH_CONTAIN:
		str = mem_strdup_printf("LIKE");
		break;
	case DBSEARCH_NOTEQUAL:
		str = mem_strdup_printf("!=");
		break;
	case DBSEARCH_GREATER:
		str = mem_strdup_printf(">");
		break;
	case DBSEARCH_LESS:
		str = mem_strdup_printf("<");
		break;
	case DBSEARCH_STARTWITH:
		str = mem_strdup_printf("LIKE");
		break;
	case DBSEARCH_GREATEREQUAL:
		str = mem_strdup_printf(">=");
		break;
	case DBSEARCH_LESSEQUAL:
		str = mem_strdup_printf("<=");
		break;
		}
	return str;
	}

void
db_looksee_addand(DbLookSee * looksee, gchar * field, gchar * value, gint type, gint datatype)
	{
	gchar *str, *opr, *tmp, *tmp2;

	g_assert(looksee);
	g_assert(field);
	g_assert(value);
	opr = db_looksee_gettype(type);
	if (datatype != DBBOOL)
		{
		if (looksee->where == NULL)
			{
			if (type == DBSEARCH_CONTAIN)
				looksee->where = mem_strdup_printf(" WHERE (lower(%s) %s lower('%c%s%c')",
				                                   field, opr, '%', value, '%');
			else if (type == DBSEARCH_STARTWITH)
				looksee->where = mem_strdup_printf(" WHERE (lower(%s) %s lower('%s%c')", field, opr, value, '%');
			else
				looksee->where = mem_strdup_printf(" WHERE (%s)%s '%s'", field, opr, value);
			looksee->sort = mem_strdup(field);
			}
		else
			{
			if (type == DBSEARCH_CONTAIN)
				str = mem_strdup_printf(" AND lower(%s) %s lower('%c%s%c')", field, opr, '%', value, '%');
			else if (type == DBSEARCH_STARTWITH)
				str = mem_strdup_printf(" AND lower(%s) %s lower('%s%c')", field, opr, value, '%');
			else
				str = mem_strdup_printf(" AND %s %s '%s'", field, opr, value);

			tmp = mem_strdup_printf("%s%s", looksee->where, str);
			mem_free(looksee->where);
			looksee->where = tmp;
			}
		}
	else
		{
		if ((g_strcasecmp("true", value) == 0) || (g_strcasecmp("false", value) == 0))
			{

			if (looksee->where == NULL)
				{
				looksee->where = mem_strdup_printf(" WHERE %s = '%s'", field, value);
				looksee->sort = mem_strdup(field);
				}
			else
				{
				str = mem_strdup_printf(" AND %s = '%s'", field, value);

				tmp2 = g_strconcat(looksee->where, str, NULL);
				tmp = mem_strdup(tmp2);
				g_free(tmp2);
				mem_free(looksee->where);
				looksee->where = tmp;
				}
			}
		}
	mem_free(opr);
	}

void
db_looksee_addor(DbLookSee * looksee, gchar * field, gchar * value, gint type, gint datatype)
	{
	gchar *str, *opr, *tmp, *tmp2;

	g_assert(looksee);
	g_assert(field);
	g_assert(value);
	opr = db_looksee_gettype(type);
	if (looksee->sort == NULL)
		looksee->sort = mem_strdup(field);
	debugmsg("datatype is %d", datatype);
	if (datatype != DBBOOL)
		{
		if (looksee->where == NULL)
			{
			if (type == DBSEARCH_CONTAIN)
				looksee->where =
				    mem_strdup_printf(" WHERE (lower(%s) %s lower('%c%s%c')", field, opr, '%', value, '%');
			else if (type == DBSEARCH_STARTWITH)
				looksee->where = mem_strdup_printf(" WHERE (lower(%s) %s lower('%s%c')", field, opr, value, '%');
			else
				looksee->where = mem_strdup_printf(" WHERE (%s)%s '%s'", field, opr, value);
			}
		else
			{
			if (type == DBSEARCH_CONTAIN)
				str = mem_strdup_printf(" OR lower(%s) %s lower('%c%s%c')", field, opr, '%', value, '%');
			else if (type == DBSEARCH_STARTWITH)
				str = mem_strdup_printf(" OR lower(%s) %s lower('%s%c')", field, opr, value, '%');
			else
				str = mem_strdup_printf(" OR %s %s '%s'", field, opr, value);
			g_assert(looksee->where);
			g_assert(str);
			tmp2 = g_strconcat(looksee->where, str, '\0');
			tmp = mem_strdup(tmp2);
			g_free(tmp2);
			mem_free(looksee->where);
			looksee->where = tmp;
			}
		}
	else
		{
		if ((g_strcasecmp("true", value) == 0) || (g_strcasecmp("false", value) == 0))
			{

			if (looksee->where == NULL)
				{
				looksee->where = mem_strdup_printf(" WHERE %s = '%s'", field, value);
				}
			else
				{
				str = mem_strdup_printf(" OR %s = '%s'", field, value);
				g_assert(looksee->where);
				g_assert(str);
				tmp2 = g_strconcat(looksee->where, str, '\0');
				tmp = mem_strdup(tmp2);
				g_free(tmp2);
				mem_free(looksee->where);
				looksee->where = tmp;
				}
			}

		}
	mem_free(opr);
	}

void
db_looksee_addsort(DbLookSee * looksee, gchar * field)
	{
	g_assert(looksee);
	g_assert(field);

	if (looksee->sort != NULL)
		mem_free(looksee->sort);
	looksee->sort = mem_strdup(field);
	}

void
db_looksee_addbracket(DbLookSee * looksee, gint type)
	{
	gchar *str, *tmp, *tmp2;
	gchar c;

	g_assert(looksee);
	if (type == 0)
		c = '(';
	else
		c = ')';
	if (looksee->where == NULL)
		{
		if (type == DBSEARCH_CONTAIN)
			looksee->where = mem_strdup_printf(" WHERE ( %c", c);
		else if (type == DBSEARCH_STARTWITH)
			looksee->where = mem_strdup_printf(" WHERE ( %c", c);
		else
			looksee->where = mem_strdup_printf(" WHERE ( %c", c);
		}
	else
		{
		if (type == DBSEARCH_CONTAIN)
			str = mem_strdup_printf(" %c ", c);
		else if (type == DBSEARCH_STARTWITH)
			str = mem_strdup_printf(" %c ", c);
		else
			str = mem_strdup_printf(" %c", c);
		tmp2 = g_strconcat(looksee->where, str, NULL);
		tmp = mem_strdup(tmp2);
		g_free(tmp2);
		mem_free(looksee->where);
		looksee->where = tmp;
		}
	}

DbLookSee *
db_looksee_create(gchar * table)
	{
	DbLookSee *looksee;

	looksee = (DbLookSee *) mem_alloc(sizeof(DbLookSee));
	looksee->where = NULL;
	looksee->sort = NULL;
	looksee->table = mem_strdup(table);
	return looksee;
	}

void
db_looksee_abort(DbLookSee * looksee)
	{
	g_assert(looksee);
	if (looksee->where != NULL)
		mem_free(looksee->table);
	if (looksee->where != NULL)
		mem_free(looksee->table);
	if (looksee->sort != NULL)
		mem_free(looksee->sort);
	mem_free(looksee);
	looksee = NULL;
	}

void
db_looksee_addquery(DbLookSee * looksee, gchar * query)
	{
	gchar *tmpstr, *tmp2;

	g_assert(looksee);
	if (looksee->where == NULL)
		looksee->where = mem_strdup_printf(" WHERE (%s)", query);
	else
		{
		tmp2 = g_strconcat(looksee->where, query, NULL);
		tmpstr = mem_strdup(tmp2);
		g_free(tmp2);
		mem_free(looksee->where);
		looksee->where = tmpstr;
		}
	}

/* Do your stuff!.. */

/**
 * db_looksee_exec
 * @looksee: Looksee arguments for searching, this is used to create an SQL statement
 * 
 * 
 * Returns: Database object, else %NULL on error.
 */
Object *
db_looksee_exec(DbLookSee * looksee)
	{
	gchar *tmpstr;
	gchar *query;
	Object *obj;

	g_assert(looksee);
	g_assert(looksee->table);

	db_toliet_flushall();

	if (looksee->sort != NULL)
		{
		tmpstr = mem_strdup_printf("%s ) ORDER BY %s", looksee->where, looksee->sort);
		mem_free(looksee->where);
		looksee->where = tmpstr;
		}

	/* clear the contents of the object before creating a new isntance of it */
	query = mem_strdup_printf("SELECT *, oid FROM %s %s", looksee->table, looksee->where);
	debugmsg("Query is %s", query);
	obj = db_sql(NULL, query);
	mem_free(query);

	if (looksee->where != NULL)
		mem_free(looksee->where);
	if (looksee->table != NULL)
		mem_free(looksee->table);
	if (looksee->sort != NULL)
		mem_free(looksee->sort);
	mem_free(looksee);
	looksee = NULL;

	return obj;
	}
