#ifndef __dbobjectnav_h
#define __dbobjectnav_h

#include "dbheader.h"
#include "dbobject.h"

/* record navigation */
gint db_movenext(Object * obj);
gint db_moveto(Object * obj, gint row);
gint db_movelast(Object * obj);

gint db_numrecord(Object * obj);
gint db_currentrow(Object * obj);
gint db_moveforce(Object *obj, gint row);

/* other stuff... */
Object *db_copyobject(Object * obj, gint row);

gint db_moverecordbyoid(Object * obj, DbUniqueId *id);

#endif
