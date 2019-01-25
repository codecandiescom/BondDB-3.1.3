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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <glib.h>

#include "bonddbobj.h"

#include "bonddbparse.h"
/*#include "dbsqlparse.h"
#include "dbgather.h"
#include "dbcache.h"
#include "dbobject.h"
#include "dbsuggest.h"
#include "dbconstraint.h"*/
#include "sql_parser.h"
#include "bc.h"





/*** FIX****
static GList * 
bonddbparse_verifysql_addfield(GList *retlist, DbFieldDef *fielddef, DbTableDef *tabledef)
	{
	retlist = g_list_prepend(retlist, tabledef);
	retlist = g_list_prepend(retlist, fielddef);
	return retlist;
	}
*/

/**
 * db_verifysql_parse()
 * @param obj : Database object
 * @param orginalsql : The orginal sql statement
 * 
 * Check the sql statement, and if it can be passed successfully.  Add any
 * unique id loading that needs to be done.
 * 
 * @return  non-zero on error.
 */
gint bonddbparse_verifysql(BonddbClass *obj, gchar *orginalsql)
	{
	gchar *tmpstr, *basetable;
//	GList *fieldtablelist; DON'T FORGET TO FREE!
	GList *fieldlist;
	GList *tablelist;

	g_assert(obj);
	if (obj->statement)
		{
		errormsg("Sql statement already exists.");
		warningmsg("%s",sql_stringify(obj->statement));
		}
	g_assert(!obj->statement);

	obj->statement = sql_parse(orginalsql);
	if (obj->statement == NULL)
		{
		/*obj->sqlgood = FALSE;*/
		debuglog(50, "Failed to parse sql statement %s", orginalsql);
  /***FIX*** bonddb_verifysql_handlefailure(obj, orginalsql, &fieldlist, &tablelist);
		sql_statement_free_fields(fieldlist);
		sql_statement_free_tables(tablelist); */
		return -1;
		}
	/* get some basic information */
	if (obj->statement->type == SQL_select)
		{
		fieldlist = sql_statement_get_fields(obj->statement); 
		tablelist = sql_statement_get_tables(obj->statement);
		if (tablelist == NULL || fieldlist == NULL)
			{
			errormsg("field or table passed in %s is NULL", orginalsql);
			return -2;
			}
		else
			basetable = tablelist->data;
	/***FIX*** fieldtablelist = bonddb_verifysql_gettablefieldlist(fieldlist, tablelist); */
		/* add pog_oid for every table in the select statement. Do a as to uniqueid */
	/**FIX** fieldtablelist = bonddb_verifysql_adduniqueids(obj->statement, fieldtablelist, fieldlist, tablelist); */
		/* create the sql statement. */

		tmpstr = sql_stringify(obj->statement);
		if (obj->query)
			mem_free(obj->query);
		obj->query = mem_strdup_printf("%s", tmpstr);
		mem_free(tmpstr);
		if (obj->firsttable == NULL && basetable)
			bonddb_setfirsttable(obj, basetable);
		/* populate that field goodly. */
		/*if (obj->sqlready == TRUE)
			{
			db_field_populate(obj, fieldtablelist, basetable);
			obj->sqlgood = TRUE;
			}
		else
			{*/
			sql_destroy(obj->statement);
			obj->statement = NULL;
			obj->sqlgood = FALSE;
			/*}*/

		/* cleanup, make sure i free everything */
		sql_statement_free_fields(fieldlist);
		sql_statement_free_tables(tablelist);
//		g_list_free(fieldtablelist);
		}

	return 0;
	}

static gboolean 
bonddbparse_verifysql_tableinlist(GList *tablelist, gchar *tablename)
	{
	GList *walk;

	for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
		if (strcasecmp(tablename, (gchar *) walk->data) == 0)
			return TRUE;
	return FALSE;
	}

static gint 
bonddbparse_verifysql_addfieldtable_appendfield(BonddbClass *obj, gchar *fieldname, gchar *tablename)
	{
	/** needs fixing **/
/*	DbTableDef *td;
	DbFieldDef *fd = NULL;
	DbField *field;

	td = db_findtable(tablename);
	if (td != NULL)
		fd = db_findfield(td, fieldname);
	if (fd == NULL)
		{
		errormsg("Unable to find %s.%s", tablename, fieldname);
		return -1;
		}

	obj->field = (DbField **) mem_realloc(obj->field, sizeof(DbField *) * (obj->numfield + 1));
	field = db_field_create(fd, td);
	field->fieldposinsql = obj->numfield;
	obj->field[obj->numfield++] = field;
 */
	return 0;
	}
	
/**** FIX **
static gint 
bonddb_verifysql_addjoin(BonddbClass *obj, GList *tablelist, gchar *tablename)
	{
	DbConstraintDef *c;
	gchar *check;
	gchar *left = NULL;
	gchar *right = NULL;
	gchar *leftfield = NULL;
	gchar *rightfield = NULL;
	GList *walk, *innerwalk;

	g_assert(obj);
	g_assert(tablelist);
	g_assert(tablename);

	right = tablename;

	for (walk = g_list_first(tablelist); walk != NULL; walk = walk->next)
		{
		 ---break out condiition. found the correct answer ---
		if (rightfield)
			break;
		check = walk->data;
		for (innerwalk = g_list_first(globaldb->constraints); innerwalk != NULL; innerwalk = innerwalk->next)
			{
			c = innerwalk->data; 
			if (strcasecmp(c->table[0], check) == 0)
				if (strcasecmp(c->table[1], right) == 0)
					{
					left = check;
					leftfield = c->column[0];
					rightfield = c->column[1];
					break;
					}
			}
		}
	if (leftfield != NULL && rightfield != NULL)
		return sql_statement_append_tablejoin(obj->statement, left, right, leftfield, rightfield);
	return -1;
	}
*/
/**
 * bonddb_verifysql_addfieldtable()
 * @param obj :
 *
 * Append a @tablename.@fieldname onto a exisiting database object. An SQL
 * statement must already be associated with the object before running this.
 */
gint 
bonddbparse_verifysql_addfieldtable(BonddbClass *obj, gchar *fieldname, gchar *tablename)
	{
//	gint toadd; value never used!
	gchar *tmpstr;
	GList *tablelist, *fieldlist;

	g_assert(obj);
	if (!obj->statement)
		{
		errormsg("Can not add %s.%s to object %s because no sql statement exists. Query:%s", tablename, fieldname,
		         obj->firsttable, obj->query);
		return -1;
		}
	/* need function to check whether the field in the table exists  */
	/*if (db_checkfieldtableexist(tablename, fieldname) == FALSE)
		{
		errormsg("%s.%s doesn't exist in the database. "
			 "I can not extract them.\n%s", 
			 tablename, fieldname, obj->query);
		return -2;
		}*/
	fieldlist = sql_statement_get_fields(obj->statement);
	tablelist = sql_statement_get_tables(obj->statement);

	if (tablename == NULL)
		tablename = obj->firsttable;
	mem_free(obj->query);
	obj->query = NULL;
	
	/*toadd = obj->numfield;*/
	
	/* Check if we already know about this table. If not we need to 
	 * handle this */
	if (bonddbparse_verifysql_tableinlist(tablelist, tablename) == FALSE)
		{
		/* add table and where statement */

	        /** FIX ** db_verifysql_addjoin(obj, tablelist, tablename);*/
		/* append oid */
		sql_statement_append_field(obj->statement, tablename, "oid", NULL);
		bonddbparse_verifysql_addfieldtable_appendfield(obj, "oid", tablename);
//		toadd++;
		}
//	toadd++;
	/* Add the field to the sql statement */
	sql_statement_append_field(obj->statement, tablename, fieldname, NULL);
	/* create a bit more in the field statement */
	bonddbparse_verifysql_addfieldtable_appendfield(obj, fieldname, tablename);
	/* expand the cache to accomidate new values */
	
		
	/*db_cache_expandfield(obj, toadd);*/
	
		/* stringify the whole thing */
	tmpstr = sql_stringify(obj->statement);
	obj->query = mem_strdup(tmpstr);
	mem_free(tmpstr);
	debugmsg("Sql statement is now %s", obj->query);
	sql_statement_free_fields(fieldlist);
	sql_statement_free_tables(tablelist);

	return 0;
	}


/**
 * db_sql_findstrkeyword()
 * @param offset : Where to start searching in the string.
 * @param _fullstr : Full string to search for @_str in.
 * @param _str : Search string to look for.
 *
 * Look for a string in @_fullstr and return the position which it occurs at. 
 * This works on words, spaces and non-alpha numerics can not occur in _str.
 *
 * @return  Position of string where @_str occurs.
 */
gint
bonddb_parse_findstrkeyword(gint offset, gchar * _fullstr, gchar * _str)
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
                                if (fullstr[i + 1] == 0 || 
					fullstr[i + 1] == ' ' || 
					fullstr[i + 1] == 13 ||
                                        fullstr[i + 1] == ',' || 
					fullstr[i + 1] == '*' ||
					fullstr[i + 1] == '\t')
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

			
/* find the end of a word */
static gint
bonddb_parse_findblank(gint startpos, gchar * str)
        {
        gint i, slen;

        slen = strlen(str);

        for (i = startpos; i < slen; i++)
                {
                if (str[i] == 0)
                        return -1;
                if (str[i] < 48 || str[i] >= 123 || str[i] == '=' || 
			str[i] == '>' || str[i] == '<')
                        return i;
                }
        return -1;
        }

/* find the first occurance of a word in the string */
static gint
bonddb_parse_findnonblank(gint startpos, gchar * str)
        {
        gint i, slen;

        slen = strlen(str);

        for (i = startpos; i < slen; i++)
                {
                if (str[i] == 0)
                        return -1;
                if (str[i] >= 48 && str[i] < 123 && str[i] != '=' && 
			str[i] != '>' && str[i] != '<')
                        return i;
                }
        return -1;
        }


/* if parsing sql statement fails use this as an alternative method */
static gchar*
bonddb_parse_alternativefirsttable(gchar *sql)
	{
	gint i, j=0;
	gint frompos, fromtablestart, fromtableend;
	gchar *retstr = NULL, *tmpstr;
	g_assert(sql);
	tmpstr = g_ascii_strup(sql, -1);
	if (!g_strrstr(tmpstr, "SELECT")) 
		{
		warningmsg("Not select statement %s",sql);
		g_free(tmpstr); /* CHECKED: this should be g _ f r e e */
		return NULL;
		}
	g_free(tmpstr); /* CHECKED: this should be g _ f r e e */

	frompos = bonddb_parse_findstrkeyword(5, sql, "from");
	fromtablestart = bonddb_parse_findnonblank(frompos+4, sql);
	fromtableend = bonddb_parse_findblank(fromtablestart, sql);
	if (fromtableend == -1)
		fromtableend = strlen(sql);
	if (fromtableend <= 0 || fromtablestart <= 0)
		{
		warningmsg("Parse failed. %d, %d, %d. Can't find from "
		"in %s",frompos, fromtablestart, fromtableend, sql);
		return NULL;
		}
	/* Skip any ending semicolons */
	if(sql[fromtableend-1] == ';')
		fromtableend--;

	/* Build string now */
	retstr = mem_alloc(sizeof(gchar)*(fromtableend-fromtablestart)+2);
	for (i=fromtablestart;i<fromtableend;i++)
		retstr[j++] = sql[i];
	retstr[j] = 0;

	/* m.essage("returning %s",retstr); */

	return retstr;
	}

/**
 * bonddb_sqlparse_getfirsttable()
 * @param sql : SQL Statement
 *
 * gets the first table in a select statement
 *
 * @return  the name of the table,  NULL on error
 */
gchar *bonddb_parse_getfirsttable(gchar * sql)
	{
	return bonddb_parse_alternativefirsttable(sql);
	}


gint
bonddb_parse_tablelist(BonddbClass *obj)
{
	gint i;
	gchar *value;
	BonddbGlobal *bonddb;
	g_assert(obj);
	if (obj->res == NULL)
		return -1;
	bonddb = obj->bonddb;
	
	for (i=0;i<obj->numcolumn;i++)
	{
		bonddb->l->_db_tablename(bonddb->l, obj->conn, 
			obj->res, i, &value);
		if (value == NULL)
			continue;
		if (obj->tablelist && g_list_find(obj->tablelist, value))
		{
			mem_free(value);
			continue;
		}
		obj->tablelist = g_list_append(obj->tablelist,value);
	}
	if (!obj->tablelist)
		return -1;
	return 0;
}
