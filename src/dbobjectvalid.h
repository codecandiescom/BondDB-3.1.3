#ifndef dbobjectvalid_h
#define dbobjectvalid_h

#include "dbobject.h"

/* Checking functions. */
gboolean db_isnewrecord(Object * obj);
gboolean db_isappend(Object * obj);
gboolean db_isabletogetid(Object *obj);
gboolean db_isrecordchanged(Object * obj);
gboolean db_isobjectchanged(Object * obj);
gboolean db_isrecordset(Object * obj);
gboolean db_isfieldposok(Object * obj, gint pos);
gboolean db_isrecordposok(Object * obj, gint pos);
gboolean db_isvalidtable(Object *obj);
gboolean db_iscachenewrecord(Object * obj, DbCache *cache);
gboolean db_isabletoadd(Object *obj);
gboolean db_isfiltered(Object *obj);
gboolean db_iscachechanged(DbCache*);
gboolean db_isindatabase(Object * obj);
gboolean db_isfulldbsupport (Object *obj);
gboolean db_issqlreadable(Object *obj);
gboolean db_iscacheloaded(Object *obj);
gboolean db_isasodeadobject(Object *obj);
gboolean db_isdeadobject(Object * obj);

/* Setting attributes functions */
void db_setforwrite(Object * obj);
void db_setforinsert(Object * obj);
void db_setforread(Object * obj);
void db_setnotchanged(Object * obj);

/* getting variable returns */
gint db_numfields(Object * obj);

#endif
