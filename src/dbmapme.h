#ifndef __dbmapme_h
#define __dbmapme_h

/* will allow objects to map to other objects */

#include "dbobject.h"
#include "dbgather.h"

/* see if reference to this field lies in any constraint references, returns tablename */
gchar *db_mapfieldsource(DbTableDef * def, gchar * fieldname);

/* make a map for an object */
gint db_createmapobject(Object * obj, gchar * classname);

/* free maps */
gint db_freemapobject(Object * obj);

/* get a object from the map */
Object *db_getmapbyclass(Object * obj, gchar * classname);
gint db_mapme_debug(Object * _obj);

Object *db_mapacrossobjects(Object * obj, gchar * classname);

/* MAIN FUNCTION */
Object * db_mapme_findobjectbytable(Object *_obj, gchar *table);

/* for refreshing stuff, this needs to be rewritten at somepoint, well all of mapme does */
gint db_mapme_realignobjects(Object *obj);
gint db_copymapobject(Object *retobj, Object *obj);
void db_mapme_fixsync(Object *obj);

#endif
