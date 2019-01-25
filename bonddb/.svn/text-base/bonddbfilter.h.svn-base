#ifndef __bonddbfilter_h
#define __bonddbfilter_h

#include "sql_parser.h" 

typedef struct _BonddbFilterRequest BonddbFilterRequest;

#include "bonddbobj.h"

struct _BonddbFilterRequest 
{
	gchar *tablename;
	gchar *fieldname;
	gchar *value;					 
	sql_condition_operator operation;
};

gint bonddb_filter_clear(BonddbClass *obj);
gint bonddb_filter_moveto(BonddbClass *obj, gint row);
gint bonddb_filter_numrecord(BonddbClass *obj);
gint bonddb_filter_delete(BonddbClass *obj, gint currentrow);
gint bonddb_filter_byvalue(BonddbClass *obj, gchar *tablename, 
		gchar *fieldname, gchar *fieldvalue, 
		sql_condition_operator opr);
gint bonddb_filter_remove(BonddbClass *obj, gchar *tablename, 
		gchar *fieldname);
gint bonddb_filter_currentrow(BonddbClass *obj);
gint bonddb_filter_test(BonddbGlobal *bonddb, gint testnum);
		
#endif
