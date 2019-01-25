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
#include <glib.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "dbwrapper.h"
#include "dbsqlparse.h"
#include "dbgather.h"
#include "dbfield.h"
#include "sql_parser.h"
#include "bc.h"

/* note, not case senstive */
/**
 * db_sql_findstrkeyword:
 * @offset: Where to start searching in the string.
 * @_fullstr: Full string to search for @_str in.
 * @_str: Search string to look for.
 *
 * Look for a string in @_fullstr and return the position which it occurs at. This works
 * on words, spaces and non-alpha numerics can not occur in _str.
 * 
 * Returns: Position of string where @_str occurs.
 */
gint
db_sql_findstrkeyword(gint offset, gchar * _fullstr, gchar * _str)
	{
	gint i, num, j = 0, retval;
	gchar *fullstr, *str;
	gchar c1, c2;

	g_assert(_fullstr);
	g_assert(_str);
	/* convert strings to upper case */
	fullstr = mem_strdup(_fullstr);
	str = mem_strdup(_str);
	g_strup(str);
	g_strup(fullstr);
	/* do loop thing to find string */
	num = strlen(fullstr);
	for (i = offset; i < num; i++)
		{
		/* case senstivity test */
		c1 = fullstr[i];
		c2 = str[j];
		if (c2 >= 65 && c2 <= 90)
			c2 += ((c2-65)+97);
		if (c1 >= 65 && c1 <= 90)
			c1 += ((c1-65)+97);
		if (c1 == c2)
			{
			j++;
			if (j == strlen(str))
				{
				if (fullstr[i + 1] == 0 || fullstr[i + 1] == ' ' || fullstr[i + 1] == 13 ||
				        fullstr[i + 1] == ',' || fullstr[i + 1] == '*')
					{
					retval = i - (strlen(str) - 1);
					mem_free(str);
					mem_free(fullstr);
					return retval;
					}
				else
					j = 0;
				}
			continue;
			}
		j = 0;
		}

	mem_free(str);
	mem_free(fullstr);
	return -1;
	}

/* find the first occurance of a word in the string */
static gint
db_sql_findnonblank(gint startpos, gchar * str)
	{
	gint i, slen;

	slen = strlen(str);

	for (i = startpos; i < slen; i++)
		{
		if (str[i] == 0)
			return -1;
		if (str[i] >= 48 && str[i] < 123 && str[i] != '=' && str[i] != '>' && str[i] != '<')
			return i;
		}
	return -1;
	}

/* find the end of a word */
static gint
db_sql_findblank(gint startpos, gchar * str)
	{
	gint i, slen;

	slen = strlen(str);

	for (i = startpos; i < slen; i++)
		{
		if (str[i] == 0)
			return -1;
		if (str[i] < 48 || str[i] >= 123 || str[i] == '=' || str[i] == '>' || str[i] == '<')
			return i;
		}
	return -1;
	}
/* if parsing sql statement fails use this as an alternative method */
static gchar*
db_sql_alternativefirsttable(gchar *sql)
	{
	gint i, j=0;
	gint frompos, fromtablestart, fromtableend;
	gchar *retstr = NULL;

	if ((db_sql_findstrkeyword(0, sql, "select"))<0)
		{
		warningmsg("Not select statement %s",sql);
		return NULL;
		}
	
	frompos = db_sql_findstrkeyword(5, sql, "from");
	fromtablestart = db_sql_findnonblank(frompos+4, sql);
	fromtableend = db_sql_findblank(fromtablestart, sql);
	if (fromtableend <= 0 || fromtablestart <= 0)
		{
		warningmsg("this is weird");
		return NULL;
		}
	/* Build string now */
	retstr = mem_alloc(sizeof(gchar)*(fromtableend-fromtablestart)+2);
	for (i=fromtablestart;i<fromtableend;i++)
		retstr[j++] = sql[i];
	retstr[j] = 0;

	/* m.essage("returning %s",retstr); */

	return retstr;
	}
	
gchar *
db_sql_findword(gint startpos, gint endpos, gchar * fullstr)
	{
	gchar *retstr;
	gint i, slen = strlen(fullstr);

	if (startpos >= slen || endpos >= slen || endpos < 0 || startpos < 0)
		return NULL;
	retstr = (gchar *) mem_alloc((endpos - startpos) + 1);
	for (i = startpos; i < endpos; i++)
		retstr[i - startpos] = fullstr[i];
	retstr[i - startpos] = 0;
	return retstr;
	}

/* return a GLIST of words which are in the fullstr from the startpos.
	NOTE: Free up each item when done.
*/
GList *
db_sql_extractwords(gint startpos, gchar * fullstr, gint limit)
	{
	gint i, slen, wordpos;
	GList *retlist = NULL;
	gchar *str;

	slen = strlen(fullstr);
	if (limit == -1)
		limit = 2000;
	for (i = 0; i < limit; i++)
		{
		wordpos = db_sql_findnonblank(startpos, fullstr);
		if (wordpos == -1)
			break;
		startpos = db_sql_findblank(startpos, fullstr);
		if (startpos == -1)
			break;
		str = db_sql_findword(wordpos, startpos, fullstr);
		if (str == NULL)
			continue;
		retlist = g_list_append(retlist, str);
		}
	return retlist;
	}

/* look for reference in the query to the table name */
gint
db_sql_reftotable(gchar * query, gchar * table)
	{
	gint frompos, tablepos, closepos;

	frompos = db_sql_findstrkeyword(0, query, "FROM");
	if (frompos == -1)
		return -1;

	closepos = db_sql_findstrkeyword(frompos, query, "WHERE");
	if (closepos == -1)
		closepos = db_sql_findstrkeyword(frompos, query, "HAVING");
	if (closepos == -1)
		closepos = strlen(query);

	tablepos = db_sql_findstrkeyword(frompos, query, table);
	if (tablepos == -1 || tablepos > closepos)
		return -1;
	return tablepos;
	}

/**
 * db_sqlparse_suggesttable:
 * @fieldname: A string to extract the fieldname from.
 *
 * From a given @fieldname in the format of tablename.fieldname this function
 * returns the tablename component. This is the more important used functions. 
 * Suggest a table this could point to.
 *
 * Returns: Newly allocated string of the tablename or %NULL on error.
 */
gchar *
db_sqlparse_suggesttable(gchar * fieldname)
	{
	void *retptr;
	gchar *tmpstr;
	gint i, slen = strlen(fieldname);

	for (i = 0; i < slen; i++)
		if (fieldname[i] == '.' || (i < slen - 1 && fieldname[i + 1] == '>' && fieldname[i] == '-'))
			{
			tmpstr = g_strndup(fieldname, i);
			retptr = mem_strdup(tmpstr);
			g_free(tmpstr);
			return retptr;
			}
	return NULL;
	}

/* suggest a field this could point to */
gchar *
db_sqlparse_suggestfield(gchar * fieldname)
	{
	gchar *retstr;
	gint i, j, slen = strlen(fieldname);

	for (i = slen - 1; i >= 0; i--)

		if (fieldname[i] == '.' || (i > 0 && fieldname[i] == '>' && fieldname[i - 1] == '-'))

			{
			i++;
			retstr = (gchar *) mem_alloc((slen - i) + 1);
			for (j = 0; j < (slen - i); j++)
				retstr[j] = fieldname[i + j];
			retstr[j] = 0;
			return retstr;
			}

	return NULL;
	}

/**
 * db_sqlparse_lookelsewhere:
 * @str:
 * @retfield:
 * @rettable:
 *
 * Remeber to free the variables @retfield and @rettable
 * 
 * Returns: 0 of there is no suggestion, 1 if there is a suggestion, -1 if there was
 * a problem.   
 */
gint
db_sqlparse_lookelsewhere(gchar * str, gchar ** retfield, gchar ** rettable)
	{
	gchar *tmpstr;
	gint i, j, slen = strlen(str);

	*retfield = NULL;
	*rettable = NULL;
	g_assert(str);
	for (i = slen - 1; i >= 0; i--)
		if (str[i] == '.')
			{
			i++;
			*retfield = (gchar *) mem_alloc((slen - i) + 1);
			for (j = 0; j < (slen - i); j++)
				(*retfield)[j] = str[i + j];
			(*retfield)[j] = 0;

			tmpstr = g_strndup(str, i - 1);
			*rettable = mem_strdup(tmpstr);
			g_free(tmpstr);
			return 1;
			}
	return 0;
	}

/* Add the pg_oid field into a sql query if it is missing -> db_id_verifysql?. */
gchar *
db_sqlparse_addmissing_pg_oid(gchar * query)
	{
	return NULL;
	}

/* drop the ' marks from a string */
gchar *
db_sqlparse_dropquation(gchar * query)
	{
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
	}

/**
 * db_sqlparse_getfirsttable:
 * @sql: SQL Statement  
 *
 * gets the first table in a select statement
 *
 * Returns: the name of the table,  NULL on error
 */
gchar *
db_sqlparse_getfirsttable(gchar * sql)
	{
	gchar *retstr = NULL;
	gchar *temp;
	sql_statement *s;

	s = sql_parse(sql);
	if (s == NULL)
		{
		warningmsg("Unable to parse SQL statement, trying fallback method");
		return db_sql_alternativefirsttable(sql);
		}

	temp = sql_statement_get_first_table(s);
	retstr = mem_strdup(temp);
	g_free(temp);
	sql_destroy(s);
	return retstr;
	}

/**
 * db_sqlparse_getselectfields:
 * @sql: SQL Statement  
 *
 * Gets the fields that a select statement is over
 *
 * Returns: a glist of all fields that the select statement is over
 */
GList *
db_sqlparse_getselectfields(gchar * sql)
	{
	sql_statement *s;
	GList *retlist = NULL;

	s = sql_parse(sql);
	if (s == NULL)
		{
		debugmsg("sql_build returned NULL");
		return NULL;
		}

	retlist = sql_statement_get_fields(s);
	sql_destroy(s);
	return retlist;
	}
/**
 * db_sqlparse_freeselectfields
 * @fieldlist: List returned by db_sqlparse_getselectfields();
 * 
 * Free up the variables generated by db_sqlparse_getselectfields();
 */
void
db_sqlparse_freeselectfields(GList*fieldlist)
	{
	GList *walk;
	/* This memory is allocated by parser and is in the strdup format */
	for(walk=g_list_first(fieldlist);walk!=NULL;walk=walk->next)
		g_free(walk->data);
	g_list_free(fieldlist);
	}

static GList *
db_sqlparse_wheregetvalues(sql_field_item * item)
	{
	gchar *str, *buf = NULL;
	GList *retlist = NULL, *cur;

	if (!item)
		return NULL;
	if (item->type != SQL_name &&
		item->type != SQL_function &&
		item->type != SQL_equation)
		return NULL;

	/* This has NOT been implemented yet. */
	if (item->type == SQL_equation)
		{
		warningmsg ("SQL equation has NOT been implemented yet.");
		return retlist;
		}

	/* Francis: Added this pathetic code. This can only handle a constant function */
	if (item->type == SQL_function)
		{
		extern DbConnection *globaldbconn;
		DbRecordSet *res;
		char *sql;
		GString *func_expr;
		GList *args;
		char *arg_value;

		func_expr = g_string_new(item->d.function.funcname);

		func_expr = g_string_append(func_expr, "(");
		for (args = g_list_first(item->d.function.funcarglist); args; args = g_list_next(args))
			{
			arg_value = ((sql_field *) args->data)->item->d.name->data;
			/* fr:spy view warningmsg ("arg value: %s", arg_value); */

			func_expr = g_string_append(func_expr, arg_value);
			}
		func_expr = g_string_append(func_expr, ")");

		sql = mem_strdup_printf("select %s", func_expr->str);
		g_string_free(func_expr, FALSE);
		/* fr:spy view warningmsg ("SQL: %s", sql); */
		res = db_dbexec(globaldbconn, sql);

		str = mem_strdup_printf("%s", db_dbgetvalue(res, 0, 0));
		if (str != NULL)
			retlist = g_list_append(retlist, str);

		return retlist;
		}

	for (cur = item->d.name; cur != NULL; cur = cur->next)
		{
		if (buf != NULL)
			{
			str = mem_strdup_printf("%s%s%s", buf, (char *)cur->data, cur->next ? "." : "");
			/* fr:spy view warningmsg ("buf not null: %s",  str); */
			mem_free(buf);
			}
		else
			{
			str = mem_strdup_printf("%s%s", (char *)cur->data, cur->next ? "." : "");
			/* spy view warningmsg ("%s|%s\n",  (char *)cur->data,  cur->next ?  "." :  ""); */
			/* spy view warningmsg ("buf null: %s",  str); */
			}
		buf = str;
		}
	if (buf != NULL)
		retlist = g_list_append(retlist, buf);
	return retlist;
	}

static GList *
db_sqlparse_whererecusive(sql_where * where)
	{
	sql_condition *cond;
	GList *retlist = NULL;

	if (!where)
		return NULL;

	switch (where->type)
		{
	case SQL_single:
		warningmsg("single case");
		cond = where->d.single;
		switch (cond->op)
			{
		case SQL_eq:
		case SQL_is:
		case SQL_gt:
		case SQL_lt:
		case SQL_geq:
		case SQL_leq:
			retlist = db_sqlparse_wheregetvalues(cond->d.pair.left->item);
			retlist = g_list_concat(retlist, db_sqlparse_wheregetvalues(cond->d.pair.right->item));
		default:
			break;
			}
		break;
	case SQL_negated:
		retlist = db_sqlparse_whererecusive(where->d.negated);
		break;

	case SQL_pair:
		warningmsg("pair case");
		retlist = db_sqlparse_whererecusive(where->d.pair.left);
		retlist = g_list_concat(retlist, db_sqlparse_whererecusive(where->d.pair.right));
		break;
	default:
		break;
		}
	/* m.essage("%d items",g_list_length(retlist)); */
	return retlist;
	}

/**
 * db_sqlparse_getwherefieldsandvalues:
 * @sql: SQL Statement  
 * 
 * gets the field and value combos, like select * from table where FIELD1=VALUE1 and F2=V2
 * creates two arrays DbField **fields and gchar **values
 * 
 * Returns: a negative int on fail otherwise returns the number of results
 */
gint
db_sqlparse_getwherefieldsandvalues(gchar * query, DbField *** fields, gchar *** values)
	{
	gchar *first, *second;
	gchar *tablename;
	gint num = 0;
	GList *retlist, *walk;
	sql_statement *sql;
	sql_select_statement *select;
#if 0
#warning "Code still needs work"
#endif
	*fields = NULL;
	*values = NULL;
	warningmsg("This is still a bit buggy");
	tablename = db_sqlparse_getfirsttable(query);
	sql = sql_parse(query);
	if (sql->type != SQL_select && sql->statement != NULL)
	{
		errormsg ("SQL query does not seem to be a SELECT statment.");
		return -1;
	}
	select = sql->statement;
	retlist = db_sqlparse_whererecusive(select->where);
	walk = g_list_first(retlist);
	while (walk != NULL)
		{
		first = (gchar *) walk->data;
		walk = walk->next;
		second = (gchar *) walk->data;
		num++;
		debugmsg("%d: Got %s = %s:", num, first, second);
		*fields = (DbField **) mem_realloc(*fields, sizeof(DbField *) * num);
		*values = (gchar **) mem_realloc(*values, sizeof(gchar *) * num);
		(*fields)[num - 1] = NULL;
		(*values)[num - 1] = NULL;
		if (first != NULL && second != NULL)
			{
			(*fields)[num - 1] = db_field_getbyfield(first, tablename);
			if ((*fields)[num - 1] == NULL)
				{
				debugmsg("%s isn't a field", first);
				(*fields)[num - 1] = db_field_getbyfield(second, tablename);
				if ((*fields)[num - 1] != NULL)
					(*values)[num - 1] = mem_strdup(first);
				}
			else
				(*values)[num - 1] = mem_strdup(second);
			mem_free(first);
			mem_free(second);
			}
		walk = walk->next;
		}
	g_list_free(retlist);
	sql_destroy(sql);
	mem_free(tablename);
	return num;
	}

/**
 * db_sqlparse_getcommonsql:
 * @sql
 * 
 * Given an sql statement ie. SELECT *,oid FROM tablename WHERE id=1234
 * return just the SELECT *,oid FROM tablename.  This is used for hash tables
 * to generate the common DbField and name components.  You need to free the
 * allocated string
 * 
 */

gchar *
db_sqlparse_getcommonsql(gchar * sql)
	{
	errormsg("NOT WRITTEN!");
	/*
	   gchar seps[] = " ,\t\n"; gchar *token; gchar result[100];

	   // Establish string and get the first token: token = strtok( sql, seps ); while( token != "WHERE" ) { // While
	   there are tokens in "sql" //printf( " %s\n", token ); strcat(result, token); strcat(result, " "); // Get next
	   token: token = strtok( NULL, seps ); }

	 */

	return sql;									     
	}

/**
 * db_sqlparse_typeofsql:
 *
 * Works out if its a SELECT or an INSERT or an UPDATE . This still needs writing
 */
gint
db_sqlparse_typeofsql(gchar * sql)
	{
	g_assert(sql);
	if (g_strncasecmp(sql,"select",6)==0)
		return 0;
	if (g_strncasecmp(sql,"insert",6)==0)
		return 1;
	if (g_strncasecmp(sql,"update",6)==0)
		return 3;
	return 2;
	}
