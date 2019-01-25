#ifndef __dbpath_h
#define __dbpath_h

#include "dbgather.h"
#include "dbheader.h"

/* this is a list that is contained in the OOPath, defining each step in the process.
 	Note, tabledef and c shoulnd't be freed as they are just pointers to the global stuff */
struct DbPathElement
   {
   DbTableDef *tabledef;		  /* This table defintiation. hmmm */
   DbConstraintDef *c;			  /* Constraint information describing the relationship */
   gint direction;				  /* Which what c is defined, ie if direction==0 then c->table[0] is this table, else if
   										     direction==1 then c->table[1] is this table */
   };

/* Part for things like table.table.table.field */
struct DbPath
   {
   GList *tablepath;				  /* OOPathElement list of tables that the structure parses though to find object */
   gchar *field;					  /* last field in chain */
   gchar *finaltable;			  /* finial table */
   };

/*=====================================================================================*/

/* Generate a path from a string, this will do some serous recusivness to create a OOPath */
DbPath *db_path_generatepath(gchar * request, gchar * base);

/* Sub and misc functions */ /* Joy to the world */
gint db_path_findpaths(DbPath * path, GList * constraints, gchar * request, gchar * base, gint pos);
gchar *db_path_getpathstrseg(gchar * request, gint startpos, gint endpos);
gint db_path_lastfield(gchar * request, gint pos);

/* Path Support */
void db_path_pathdebug(DbPath * path);
void db_path_freepath(DbPath * path);

/* Given a table of what you think, and a table which you are refering it from work out
	what the table name actually is. Also work out the direction of the relationship.
	-1 is no relationship, 0 is tableto is a subclass of tablefrom, 1 is tableto is the
	parent of tablefrom */
DbPathElement *db_path_verifytable(DbDatabaseDef * db, GList * constraints, gchar * tableto, gchar * tablefrom);

/* Sub functions */
DbPathElement *db_path_hasrelationship(GList * constraints, gchar * tableto, gchar * tablefrom);
gchar *db_path_verifyfield(DbDatabaseDef * db, gchar * field, gchar * table);
gchar *db_path_getpathtable(DbPathElement * elem);

/*=====================================================================================*/

/* Generate a path from a more complex path which requires significant digging */
gint db_path_seekpaths(DbPath * path, GList * constraints, gchar * request, gchar * base, gint pos);
DbPathElement * db_path_tabledig(DbDatabaseDef * db, GList * constraints, gchar * tableto, gchar * tablefrom);

#endif
