#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>

#include "dbwrapper.h"
#include "dbgather.h"
#include "bc.h"
#include "dbconstraint.h"

#define PG_SEPERATOR "\\000"

/* Loads all constraints from the database. In postgres, it uses the
   pg_trigger table to get this information */

static gboolean
constraint_exists(GList * list, DbConstraintDef * c)
	{
	GList *l;
	DbConstraintDef *tmp;

	for (l = g_list_first(list); l != NULL; l = l->next)
		{
		tmp = l->data;

#define MSTRCMP(a) (strcmp(tmp->a, c->a) == 0)

		if (MSTRCMP(table[0]) && MSTRCMP(table[1]) && MSTRCMP(column[0]) && MSTRCMP(column[1]))
			return TRUE;
		}

	return FALSE;
	}

gboolean
db_constraints_init(DbDatabaseDef * db)
	{
	gchar *select_statement = "select tgargs from pg_trigger";
	DbRecordSet *res;
	DbConstraintDef *c;
	gchar *null_string;
	gint i;

	g_assert(db);
	g_assert(globaldbconn != NULL);
	g_assert(!db->constraints);

	db->constraints = NULL;

	/* for (i = 0; i < db->numtable; i++) { printf("Getting constraints (at start) for %d: ", i); printf("%s\n",
	   db->table[i]->name); } */

	res = db_dbexec(globaldbconn, select_statement);

	if (db_checkpgresult(res) != 0)
		{
		errormsg("Error loading constraints information from Postgresql");
		return FALSE;
		}

	for (i = 0; i < db_dbnumrows(res); i++)
		{
		/* printf("In loop %d of %d.\n", i, db_dbnumrows(res)); */
		null_string = db_dbgetvalue(res, i, 0);
		null_string = strtok(null_string, PG_SEPERATOR);

		c = (DbConstraintDef *) mem_alloc(sizeof(DbConstraintDef));
		if (!c)
			return FALSE;

		c->table[0] = mem_strdup(null_string = strtok(NULL, PG_SEPERATOR));
		c->table[1] = mem_strdup(null_string = strtok(NULL, PG_SEPERATOR));
		null_string = strtok(NULL, PG_SEPERATOR);
		c->column[0] = mem_strdup(null_string = strtok(NULL, PG_SEPERATOR));
		c->column[1] = mem_strdup(null_string = strtok(NULL, PG_SEPERATOR));
		c->casual = 0;

		if (!c->table[0] || !c->table[1] || !c->column[0] || !c->column[1] || constraint_exists(db->constraints, c))
			{
			/* printf("Constraint doesn't exist.\n"); */
			if (c->table[0] != NULL)
				mem_free(c->table[0]);
			if (c->table[1] != NULL)
				mem_free(c->table[1]);
			if (c->column[0] != NULL)
				mem_free(c->column[0]);
			if (c->column[1] != NULL)
				mem_free(c->column[1]);
			mem_free(c);
			}
		else
			{
			/* printf("Constraint being appended.\n"); */
			db->constraints = g_list_append(db->constraints, c);
			}

		/* for (x = 0; x < db->numtable; x++) { printf("Getting constraints (in loop) for %d: ", x); printf("%s\n",
		   db->table[i]->name); } */
		}

	db_dbclear(res);

	db->constraints = db_getcasualconstraints(db->constraints, db);

	for (i = 0; i < db->numtable; i++)
		{
		/*   	printf("Getting constraints for %d: ", i);
		   	printf("%s\n", db->table[i]->name);
		  */ db->table[i]->cref =  db_get_constraints(db,  db->table[i]);
		}

	return TRUE;
	}

gboolean
db_constraints_free(DbConstraintDef * c)
	{
	if (c == NULL)
		return FALSE;
	if (c->table[0] != NULL)
		mem_free(c->table[0]);
	if (c->table[1] != NULL)
		mem_free(c->table[1]);
	if (c->column[0] != NULL)
		mem_free(c->column[0]);
	if (c->column[1] != NULL)
		mem_free(c->column[1]);
	return TRUE;
	}

gboolean
db_constraints_uninit(DbDatabaseDef * db)
	{
	GList *l;
	DbConstraintDef *c;

	g_assert(db);
	for (l = g_list_first(db->constraints); l != NULL; l = l->next)
		{
		c = l->data;
		db_constraints_free(c);
		mem_free(c);
		l->data = NULL;
		}
	g_list_free(db->constraints);

	db->constraints = NULL;

	return TRUE;
	}

/* Returns true if there are any constraints on a given table */
gboolean
db_table_has_constraints(DbDatabaseDef * db, DbTableDef * table)
	{
	GList *l;
	DbConstraintDef *c;

	g_assert(db);
	g_assert(table);
	g_assert(table->name);

	for (l = g_list_first(db->constraints); l != NULL; l = l->next)
		{
		c = l->data;
		if ((strcmp(table->name, c->table[0]) == 0) || (strcmp(table->name, c->table[1]) == 0))
			return TRUE;
		}

	return FALSE;
	}

/* Given a table and column, returns a list of all the DbConstraintDefs that
   all of the relevant constraints */
GList *
db_get_constraints(DbDatabaseDef * db, DbTableDef * table)
	{
	GList *retval, *l;
	DbConstraintDef *c;

	g_assert(db);
	g_assert(table);
	g_assert(table->name != NULL);

	retval = NULL;

	for (l = g_list_first(db->constraints); l != NULL; l = l->next)
		{
		c = l->data;

		if ((strcmp(table->name, c->table[0]) == 0) || (strcmp(table->name, c->table[1]) == 0))
			retval = g_list_append(retval, c);
		}

	return retval;
	}

GList *
db_get_constraints_column(DbDatabaseDef * db, DbTableDef * table, gchar * column)
	{
	GList *retval, *l;
	DbConstraintDef *c;

	g_assert(db);
	g_assert(table);
	g_assert(table->name);

	retval = NULL;

	for (l = g_list_first(db->constraints); l != NULL; l = l->next)
		{
		c = l->data;

		if (((strcmp(table->name, c->table[0]) == 0) &&
		        (strcmp(column, c->column[0]) == 0)) ||
		        ((strcmp(table->name, c->table[1]) == 0) && (strcmp(column, c->column[1]) == 0)))
			retval = g_list_append(retval, c);
		}

	return retval;
	}

GList *
db_getcasualconstraints(GList * constraints, DbDatabaseDef * db)
	{
	GList *commentwalk;
	gint i, j;
	gboolean update;
	DbConstraintDef *c;

	g_assert(db);

	for (i = 0; i < db->numtable; i++)
		{
		for (j = 0; j < db->table[i]->num; j++)
			{
			if (db->table[i]->field[j]->comment == NULL)
				continue;
			commentwalk = g_list_first(db->table[i]->field[j]->comment);
			while (commentwalk != NULL)
				{
				update = FALSE;
				c = mem_alloc(sizeof(DbConstraintDef));
				c->table[0] = mem_strdup(db->table[i]->name);
				c->column[0] = mem_strdup(db->table[i]->field[j]->name);

				c->casual = db_casual_type(commentwalk->data);
				c->table[1] = db_casual_table(commentwalk->data);
				c->column[1] = db_casual_field(commentwalk->data);

				if (!
				        (c->casual == -1 || c->table[0] == NULL || c->column[0] == NULL || c->table[1] == NULL
				         || c->column[1] == NULL))
					if (db_casual_exist(constraints, commentwalk) == 0)
						{
						constraints = g_list_append(constraints, c);
						update = TRUE;
						}
				if (update == FALSE)
					{
					db_constraints_free(c);
					mem_free(c);
					}

				commentwalk = commentwalk->next;
				}
			}
		}
	return constraints;
	}

gint
db_casual_exist(GList * constraints, GList * item)
	{
	GList *locator = g_list_find(constraints, item->data);

	if (g_list_length(locator) == 0)
		return 0;
	return 1;
	}

/* fields for comments of csaual relationships is 1;table.field */

/* note should expand to be more forgiving in future.
   ie casual:table.field or casual:field or even just casual
   to really confuse things... hehe */
gint
db_casual_type(gchar * comment)
	{
	gchar **comar1;
	gint type;

	if (comment == NULL)
		return -1;
	comar1 = g_strsplit(comment, ";", 2);
	type = atoi(comar1[0]);
	g_strfreev(comar1);
	return type;
	}

gchar *
db_casual_table(gchar * comment)
	{
	char **comar1;
	gchar **comar2;
	gchar *table;

	if (comment == NULL)
		return NULL;
	comar1 = g_strsplit(comment, ";", 2);
	if (comar1[0] == NULL || comar1[1] == NULL)
		return NULL;
	comar2 = g_strsplit(comar1[1], ".", 2);
	if (comar2[0] == NULL || comar2[1] == NULL)
		return NULL;
	table = mem_alloc(sizeof(comar2[0]) + 1);
	strcpy(table, comar2[0]);
	g_strfreev(comar1);
	g_strfreev(comar2);
	return table;
	}

gchar *
db_casual_field(gchar * comment)
	{
	gchar **comar1;
	gchar **comar2;
	gchar *field;

	if (comment == NULL)
		return NULL;
	comar1 = g_strsplit(comment, ";", 2);
	if (comar1[0] == NULL || comar1[1] == NULL)
		return NULL;
	comar2 = g_strsplit(comar1[1], ".", 2);
	if (comar2 == NULL || comar2[1] == NULL)
		return NULL;
	field = mem_alloc(sizeof(comar2[1]) + 1);
	strcpy(field, comar2[1]);
	g_strfreev(comar1);
	g_strfreev(comar2);
	return field;
	}

/**
 * db_printconstraint:
 * @c: Database constraint to print debugging information about
 * 
 * Displays useful information to stdout about what the relationship is.
 */

void
db_printconstraint(DbConstraintDef * c)
	{
	g_assert(c);
	printf("%s.%s -> %s.%s: %d\n", c->table[0], c->column[0], c->table[1], c->column[1], c->casual);
	}

/**
 * db_printallconstraint:
 * @db: database structure, (use extern DbDatabaseDef *globaldb; )
 * 
 * Displays a list of all database constraints that are in the database.
 */
void
db_printallconstraint(DbDatabaseDef * db)
	{
	GList *l;

	g_assert(db);
	for (l = g_list_first(db->constraints); l != NULL; l = l->next)
		db_printconstraint(l->data);
	}

/**
 * db_constraint_getonmatch:
 * @creflist: GList of relationships
 * @sourcetable: source table for the relationship
 * @desttable: destiation table for relationship
 * @casual: level of searching, 0 = only hard references, 1 = only soft, 2 = any link 
 *
 * Look for a relationship that links @sourcetable and @desttable and then return that relationship
 *
 * Returns: %NULL on failure else a cref for the relationship.
 */
DbConstraintDef *
db_constraint_getonmatch(GList * creflist, gchar * sourcetable, gchar * desttable, gint casual)
	{
	GList *walk;
	DbConstraintDef *cref;

	walk = g_list_first(creflist);
	while (walk != NULL)
		{
		cref = walk->data;
		if (strcasecmp(cref->table[0], sourcetable) == 0 && (cref->casual == casual || casual == 2))
			if (strcasecmp(cref->table[1], desttable) == 0 && (cref->casual == casual || casual == 2))
				return cref;
		if (strcasecmp(cref->table[1], sourcetable) == 0 && (cref->casual == casual || casual == 2))
			if (strcasecmp(cref->table[0], desttable) == 0 && (cref->casual == casual || casual == 2))
				return cref;
		walk = walk->next;
		}
	return NULL;
	}

/**
 * db_constraint_getontable:
 * @creflist: A master GList of all constraints/relationships defined.
 * @tablename: tablename to search on
 * 
 * Create a GList of all the references that are in @tablename.
 *
 * Returns: GList* which u have to free with g_list_free();
 */
GList *
db_constraint_getontable(GList * creflist, gchar * tablename)
	{
	DbConstraintDef *cref;
	GList *walk, *retlist = NULL;

	for (walk = g_list_first(creflist); walk != NULL; walk = walk->next)
		{
		cref = walk->data;
		if (strcmp(cref->table[0], tablename) == 0)
			retlist = g_list_append(retlist, cref);
		}
	return retlist;
	}

GList *
db_constraint_getall()
	{
	return globaldb->constraints;
	}
