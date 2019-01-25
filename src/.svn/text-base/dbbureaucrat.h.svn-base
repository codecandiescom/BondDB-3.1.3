#ifndef dbbureaucrat_h
#define dbbureaucrat_h

/* I think the name says it all */

#include "dbobject.h"
#include "dbtoliet.h"

typedef struct
   {
   gint obj_instances;

   Object **obj;
   gint num;

   gint lastobjchanged;
   }
DbBureaucrat;

/* For the flushing bit */
typedef struct
   {
   GList *cachelist;
   Object *obj;
   }
DbFlush;

/* generate a new instance */
gint db_bureaucrat_generateinstance(void);

/* delete an object instance */
gint db_bureaucrat_deleteinstance(gint instance);
gint db_bureaucrat_deleteinstance_byobject(Object *obj);

/* free to be called on db_cleanup */
void db_bureaucrat_cleanup();

/* adds an object into the bureaucrat */
gint db_bureaucrat_add(Object * obj);

/* search for an object based on instance */
Object *db_bureaucrat_find(gint instance);

/* Get a list of all objects that have changed. */
GList *db_bureaucrat_changedobjects();
DbFlush *db_bureaucrat_createflush(Object * obj);
void db_bureaucrat_freeflush(GList * flushlist);

gint db_bureaucrat_recordwrite(gint instance);

/* the freeing of memory */
void db_bureaucrat_cleanup();

/* Yeah.. not sure. i wrote this function last week */
GList *db_bureaucrat_changedobjects();

gint db_bureaucrat_checkpendingwrites(Object *obj);
gint db_bureaucrat_checkmisticconnections(Object *objchanged, DbCache *cache);

gint db_bureaucrat_debug();

#endif
