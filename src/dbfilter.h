#ifndef dbfilter_h
#define dbfilter_h

#include "dbheader.h"

#define FILTER_EMPTY 0
#define FILTER_SHOW 1
#define FILTER_HIDDEN 2

#include "sql_parser.h"

typedef struct
	{
	gchar *fieldname;
	gchar *tablename;
	gchar *value;					 
	sql_condition_operator operation;
	gint flags;
	gint fieldpos;
	}
DbFilterAppend;

struct DbFilter
	{
	gboolean active;
	gint sortedpos;
	};

struct DbFilterApplied
	{
	GList *requestlist;
	GList *filters;
	gint internalaction;
	};

#include "dbuniqueid.h"

gint db_filter_free(DbFilter * filter);
DbFilter *db_filter_create(gint active, gint sortedpos);

/* high level */
gint db_filter_sortbypos (Object *obj, gchar * fieldname, gchar * tablename, gboolean order_ascending);
gint db_filter_filterbyvalue(Object * obj, gchar * fieldname, gchar * tablename, gchar * fieldvalue);
gint db_filter_load(Object * obj, GList * requestlist);

GList *db_filter_addrequestlist(GList * requestlist, gchar * fieldname, gchar * tablename);
GList *db_filter_addrequestlist_hidden(GList * requestlist, gchar * fieldname, gchar * tablename);

/* base functions */
gint db_filter_num(Object * obj);
gint db_filter_moveto(Object * obj, gint row);
gint db_filter_clear(Object * obj);
void db_filter_removefilter(Object * obj);
gchar *db_filter_getatpos(Object * obj, gint pos);
gint db_filter_checkvisible(Object * obj);
void db_filter_setfilterstate(Object * obj, gboolean state);
gint db_filter_apply(Object *obj);
gint db_filter_createpos(Object * obj);

/* information about how the filter came to exist */
void db_filter_freeapplied(Object * obj);
void db_filter_applied_addrequest(Object * obj, GList * requestlist);
void db_filter_applied_addfilter(Object * obj, gchar * field, gchar * table, gchar * value);

/* navigation */
gint db_filter_getrowposinfilter(Object * obj, gint * row);
gint db_filter_deletedrow(Object * obj, gint row);
gint db_filter_resort(Object * obj, gint pos);

#endif
