#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dbwrapper.h"

#ifdef _GDA

#include "bc.h"
#include "dbgda.h"
#include "dbgather.h"
#include "dbpgsqlgather.h"
#include "dbsqlparse.h"
#include "dbdefault.h"

gint
db_buildtabledef(DbDatabaseDef * db)
	{
	gint num, i;
	gchar *str, *oid;
	DbRecordSet *result;
	GdaDataModel *res;
	

	res = gda_connection_get_schema(globaldbconn->conn,
		GDA_CONNECTION_SCHEMA_TABLES, NULL);
	

	g_assert(db);
	
	

	/* select * from pg_class; */
	result = db_dbexec(globaldbconn, "SELECT oid, * FROM pg_class where relname not like 'pg_%'");
	if (db_dbcheckresult(result) != 0)
		{
		debugmsg("failed query in genreating tables");
		db_dbclear(result);
		return -1;
		}
	/* dump all tables beginning with pg_, for these are not mine to start with */
	num = db_dbnumrows(result);
	db->table = mem_alloc(sizeof(DbTableDef *) * num);
	db->numtable = num;
	/* printf("Number of tables/views/sequences is %d\n\n", db->numtable); */
	for (i = 0; i < num; i++)
		{
		str = db_dbgetvalue(result, i, 1);
		oid = db_dbgetvalue(result, i, 0);
		g_assert(oid != NULL);
		db->table[i] = mem_alloc(sizeof(DbTableDef));
		db->table[i]->name = mem_strdup_printf("%s", str);
		db->table[i]->query = NULL;
		db->table[i]->type = 0;
		db->table[i]->oid = atoi(oid);

		db->table[i]->uidfield = NULL;
		db->table[i]->subtable = NULL;
		db->table[i]->fromtable = NULL;
		db->table[i]->cref = NULL;

		/* printf("%d: name: %s\noid: %d\n\n",i, db->table[i]->name, db->table[i]->oid); */
		db_buildfielddef(db->table[i]);
		}

	db_dbclear(result);
	return 0;
	}

/* build a list of fields in a table, and add to the DbTableDef */
gint
db_buildfielddef(DbTableDef * db)
	{
	gboolean found=FALSE;
	gint num, i;
	gchar *query;
	DbRecordSet *result;

	g_assert(db);

	/* select * from pg_attributes; */
	query = mem_strdup_printf("select attnum,attname,typname,attlen,attnotnull,atttypmod,"
	                          "usename,usesysid,pg_class.oid,attrelid,relpages,reltuples,relhaspkey,relhasrules,relacl "
	                          "from pg_class,pg_user,pg_attribute,pg_type where (pg_class.relname='%s') "
	                          "and (pg_class.oid=pg_attribute.attrelid) and (pg_class.relowner=pg_user.usesysid) "
	                          "and (pg_attribute.atttypid=pg_type.oid) and (attnum > 0) order by attnum", db->name);
	result = db_dbexec(globaldbconn, query);
	if (db_dbcheckresult(result) != 0)
		{
		debugmsg("failed query at this point");
		db_dbclear(result);
		mem_free(query);
		return -1;
		}
	num = db_dbnumrows(result);
	db->field = (DbFieldDef **) mem_alloc(sizeof(DbFieldDef *) * (num+1));
	db->num = num;
	for (i = 0; i < num; i++)
		{
		db->field[i] = (DbFieldDef *) mem_calloc(sizeof(DbFieldDef));
		db->field[i]->name = mem_strdup(db_dbgetvalue(result, i, 1));
		db->field[i]->typename = mem_strdup(db_dbgetvalue(result, i, 2));
		db->field[i]->fieldtype = 0;
		db->field[i]->tableoid = atoi(db_dbgetvalue(result, i, 9));	/* get table source */
		db->field[i]->comment = NULL;		 /* comment assoicated with the widget, used to generate other information */
		db->field[i]->defaultvalue = NULL;	/* default value put into database */
		db->field[i]->defaultfunc = 0;	 /* call a function for getting default value */
		db->field[i]->method = NULL;		 /* Francis: added */
		db->field[i]->tabledef = db;
		if (strcmp(db_dbgetvalue(result, i, 4), "t") == 0)
			db->field[i]->notnull = TRUE;
		else
			db->field[i]->notnull = FALSE;
		/* debugmsg("%s: \t %s: \t %s",db->name,db_dbgetvalue(result,i,1),db_dbgetvalue(result,i,9)); */
		/* check if it is unique id field and if so record it so */
		/* TODO, make it actually work out if this is the current oid */
		/* printf("field %s is %d. %s\n",db->field[i]->name,db->field[i]->notnull,db_dbgetvalue(result, i, 4)); */
		if (strcasecmp(db->field[i]->name,"oid") == 0)
			{
			db->uidfield = mem_strdup(db_dbgetvalue(result, i, 1));
			mem_free(db->field[i]->typename);
			db->field[i]->typename = mem_strdup("pgoid");
			found = TRUE;
			}
		}
	if (found == FALSE)
		{
		db->field[num] = (DbFieldDef *) mem_calloc(sizeof(DbFieldDef));
		db->field[num]->name = mem_strdup("oid");
		db->field[num]->tabledef = db;		
		db->field[num]->fake = TRUE;
		db->field[num]->typename = mem_strdup("pgoid");
		db->num++;
		db->uidfield = mem_strdup("oid");
		}
	
	mem_free(query);
	db_dbclear(result);
	db_buildfielddefcomment(db);
	db_buildfielddefdefaultvalue(db);
	return 0;
	}

/* Get comments relating to field definiations */
gint
db_buildfielddefcomment(DbTableDef * db)
	{
	gint num, i, j;
	gchar *query, *tmpstr;
	DbRecordSet *result;

	g_assert(db);

	/* select * from pg_attributes; */
	if (db_pg_getversion() == 0)
		{
		query = mem_strdup_printf("select pg_attribute.attrelid, attname, pg_attribute.oid,"
		                          "pg_description.*, pg_class.relname from pg_attribute, pg_description, pg_class "
		                          "where pg_attribute.oid = pg_description.objoid and pg_class.oid=pg_attribute.attrelid "
		                          "and pg_class.relname='%s'", db->name);

		result = db_dbexec(globaldbconn, query);
		if (db_dbcheckresult(result) != 0)
			{
			debugmsg("failed query at this point");
			db_dbclear(result);
			mem_free(query);
			return -1;
			}
		num = db_dbnumrows(result);
		for (j = 0; j < num; j++)
			{
			for (i = 0; i < db->num; i++)
				{
				if (strcmp(db->field[i]->name, db_dbgetvalue(result, j, 1)) == 0)
					{
					tmpstr = mem_strdup(db_dbgetvalue(result, j, 4));
					db->field[i]->comment = g_list_append(db->field[i]->comment, tmpstr);
					}
				}
			}
		}
	else
		{
		query = mem_strdup_printf("select pg_description.*, pg_attribute.attname from "
		                          "pg_description, pg_class, pg_attribute where pg_attribute.attrelid=objoid "
		                          "and pg_attribute.attnum=objsubid and pg_description.objoid=pg_class.relfilenode "
		                          "and pg_class.relname='%s'", db->name);
		result = db_dbexec(globaldbconn, query);
		if (db_dbcheckresult(result) != 0)
			{
			debugmsg("failed query at this point");
			db_dbclear(result);
			mem_free(query);
			return -1;
			}
		num = db_dbnumrows(result);
		for (j = 0; j < num; j++)
			{
			for (i = 0; i < db->num; i++)
				{
				if (strcmp(db->field[i]->name, db_dbgetvalue(result, j, 4)) == 0)
					{
					tmpstr = mem_strdup(db_dbgetvalue(result, j, 3));
					db->field[i]->comment = g_list_append(db->field[i]->comment, tmpstr);
					}
				}
			}
		}
	db_dbclear(result);

	result = db_dbexec(globaldbconn, query);
	if (db_dbcheckresult(result) != 0)
		{
		debugmsg("failed query at this point");
		mem_free(query);
		db_dbclear(result);
		return -1;
		}
	num = db_dbnumrows(result);
	for (j = 0; j < num; j++)
		{
		for (i = 0; i < db->num; i++)
			{
			if (strcmp(db->field[i]->name, db_dbgetvalue(result, j, 1)) == 0)
				{
				tmpstr = mem_strdup(db_dbgetvalue(result, j, 4));
				db->field[i]->comment = g_list_append(db->field[i]->comment, tmpstr);
				}
			}
		}
	mem_free(query);
	db_dbclear(result);
	return 0;
	}

gint
db_buildviewdef(DbDatabaseDef * db)
	{
	gint num, i;
	gchar *query, *str;
	DbRecordSet *result;
	DbRecordSet *subq;

	g_assert(db);
	if (db_pg_getversion()==2)
	     {
	     /* Views not supported in postgresql 7.4 */
	     db->view = mem_alloc(sizeof(DbTableDef *));
	     db->numview = 0;
	     db_tablesourceforallfields(db);
	     return 0;
	     }
	   
	result = db_dbexec(globaldbconn, "select C.relname from pg_class C, pg_rewrite R where "
	                   "(relname !~ '^pg_') and (r.ev_class = C.oid) and (r.ev_type = '1') group by relname");
	if (db_dbcheckresult(result) != 0)
		{
		db_dbclear(result);
		return -1;
		}
	num = db_dbnumrows(result);
	db->view = mem_alloc(sizeof(DbTableDef *) * num);
	db->numview = num;
	for (i = 0; i < num; i++)
		{
		str = db_dbgetvalue(result, i, 0);
		query = mem_strdup_printf("SELECT pg_get_viewdef('%s')", str);
		subq = db_dbexec(globaldbconn, query);
		db->view[i] = mem_alloc(sizeof(DbTableDef));
		db->view[i]->name = mem_strdup(str);
		db->view[i]->type = 1;
		db->view[i]->oid = 0;
		db->view[i]->uidfield = NULL;
		db->view[i]->subtable = NULL;
		db->view[i]->fromtable = NULL;
		db->view[i]->cref = NULL;

		if (db_dbcheckresult(subq) == 0 && db_dbnumrows(subq) > 0)
			db->view[i]->query = mem_strdup(db_dbgetvalue(subq, 0, 0));
		else
			db->view[i]->query = NULL;
		/* get field information */
		db_buildfielddef(db->view[i]);
		db_dbclear(subq);
		mem_free(query);
		}

	db_dbclear(result);
	db_tablesourceforallfields(db);
	return 0;
	}

/* find the postgresql oid for a table given  */
gint
db_tablesourceforfield(DbDatabaseDef * db, gchar * fieldname, gint parenttableoid, gchar * parentquery)
	{
	gint num, i, oid;
	gchar *query, *str;

	/* PGresult *result; */
	DbRecordSet *result;

	query = mem_strdup_printf("SELECT attrelid, attname FROM pg_attribute where attname = '%s'", fieldname);
	result = db_dbexec(globaldbconn, query);

	if (db_dbcheckresult(result) != 0)
		{
		db_dbclear(result);
		debugmsg("failed query in generating tables");
		return -1;
		}
	mem_free(query);
	num = db_dbnumrows(result);
	for (i = 0; i < num; i++)
		{
		oid = atoi(db_dbgetvalue(result, i, 0));
		/* dump if it is yourself */
		if (oid == parenttableoid)
			continue;
		/* dump if isn't a raw table */
		/* not sure what wrong, but ignore this */
		if (db_isview(db, oid, &str) == 0)
			;
		else
			continue;
		/* see if this oid happens in the orginal sql query */
		if (db_sql_reftotable(parentquery, str) != -1)
			{
			db_dbclear(result);
			return oid;
			}
		}
	db_dbclear(result);
	return 0;
	}

/**
 * db_buildfielddefdefaultvalue:
 * 
 * Get what default values be associated with this table.
 * 
 */
gint
db_buildfielddefdefaultvalue(DbTableDef * db)
	{
	gint num, i, j;
	gchar *query, *tmpstr, *othertmpstr;
	DbRecordSet *result;

	g_assert(db);
	/* note. this function was not written under the influence of drugs */
	/* select * from pg_attributes; */
	query = mem_strdup_printf("select pg_attribute.attname, pg_attrdef.* "
	                          "from pg_attrdef, pg_attribute, pg_class "
	                          "where pg_attrdef.adnum = pg_attribute.attnum and pg_attrdef.adrelid = pg_attribute.attrelid "
	                          "and pg_class.oid=pg_attribute.attrelid and pg_class.relname='%s'", db->name);

	result = db_dbexec(globaldbconn, query);
	if (db_checkpgresult(result) != 0)
		{
		db_dbclear(result);
		debugmsg("failed to run query %s", query);
		mem_free(query);
		return -1;
		}
	num = db_dbnumrows(result);
	for (j = 0; j < num; j++)
		{
		for (i = 0; i < db->num; i++)
			{
			if (strcmp(db->field[i]->name, db_dbgetvalue(result, j, 0)) == 0)
				{
				tmpstr = db_dbgetvalue(result, j, 4);
				if (tmpstr != NULL && strlen(tmpstr) > 0)
					{
					/* assign default value */
					db->field[i]->defaultvalue = mem_strdup(tmpstr);
					/* get more detail on the default value */
					othertmpstr = db_dbgetvalue(result, j, 3);
					/* debugmsg("default value is %s in %s",db->field[i]->defaultvalue,db->name); */
					/* see if it got value ok, if not continue with your job */
					if (othertmpstr == NULL && strlen(othertmpstr) <= 0)
						continue;
					/* work out if this is a function or not */
					db->field[i]->defaultfunc = db_default_isitafunction(tmpstr, othertmpstr);
					}
				}
			}
		}
	/* I love you guys */
	mem_free(query);
	db_dbclear(result);
	return 0;
	}

/**
 * db_pg_getversion:
 *
 * Work out what version of postgresql your using.
 * if your using 7.1 or lower it will return 0, else it will return 1 on
 * 7.2 or higher.
 */
gint
db_pg_getversion()
	{
	gint i;
	gchar *value;
	DbRecordSet *result;

	result = db_dbexec(globaldbconn, "SELECT version()");
	if (db_dbcheckresult(result) != 0)
		{
		debugmsg("failed query in getting postgresql version");
		db_dbclear(result);
		return -1;
		}
	if (db_dbnumrows(result) != 1)
		{
		errormsg("Unexpect result");
		db_dbclear(result);
		return -1;
		}
	/* this code isn't perfect, it may fail on weird version numbers */
	value = db_dbgetvalue(result, 0, 0);
	for (i = 0; i < strlen(value) - 3; i++)
		{
		if (value[i] == '7' && value[i + 1] == '.' && value[i + 2] == '1')
			{
			db_dbclear(result);
			return 0;
			}
		if (value[i] == '7' && value[i + 1] == '.' && value[i + 2] >= '2' && value[i + 2] <= '3')
			{
			db_dbclear(result);
			return 1;
			}
		if (value[i] == '7' && value[i + 1] == '.' && value[i + 2] >= '4')
			{
			db_dbclear(result);
			return 2;
			}
		}
	db_dbclear(result);
	return 0;
	}

#endif
