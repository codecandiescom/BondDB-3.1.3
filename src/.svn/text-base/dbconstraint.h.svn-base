#ifndef dbconstraint_h
#define dbconstraint_h

#include "dbgather.h"
#include "dbheader.h"

#define DB_NORMAL 0
#define DB_DEFAULTCASUAL 1
#define DB_DEFAULTSLOPPY 2

struct DbConstraintDef
	{
	gchar *table[2];
	gchar *column[2];
	gint casual;
	};

/* Loads all constraints from the database. In postgres, it uses the
   pg_trigger table to get this information */
gboolean db_constraints_init(DbDatabaseDef * db);
gboolean db_constraints_uninit(DbDatabaseDef * db);
gboolean db_constraints_free(DbConstraintDef * c);

/* Returns true if there are any constraints on a given table */
gboolean db_table_has_constraints(DbDatabaseDef * db, DbTableDef * table);

/* Given a table and column, returns a list of all the DbConstraintDefs that
   all of the relevant constraints */
GList *db_get_constraints(DbDatabaseDef * db, DbTableDef * table);
GList *db_get_constraints_column(DbDatabaseDef * db, DbTableDef * table, gchar * column);
GList *db_getcasualconstraints(GList * constraints, DbDatabaseDef * db);

/* stuff for use with casual relationships - extracts info from a field comment */
gint db_casual_exist(GList * constraints, GList * item);
gint db_casual_type(gchar * comment);
gchar *db_casual_field(gchar * comment);
gchar *db_casual_table(gchar * comment);

/* just for debugging */
void db_printconstraint(DbConstraintDef * c);
void db_printallconstraint(DbDatabaseDef * db);

DbConstraintDef *db_constraint_getonmatch(GList * creflist, gchar * sourcetable, gchar * desttable, gint casual);

GList *db_constraint_getontable(GList * creflist, gchar * tablename);
GList *db_constraint_getall();

#endif /* db_constraints_h */
