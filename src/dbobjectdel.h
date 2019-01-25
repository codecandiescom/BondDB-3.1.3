
/* dbobjectdel.h
 * Andrew Hill - treshna Enterprises Ltd
 * This is designed to allow you to delete objects, and specifically the objects inside objects. 
 */
#ifndef dbobjectdel_h
#define dbobjectdel_h

#include "dbheader.h"
#include "dbgather.h"

/* A list of objects that are to be deleted. Aswell as how it is obtained */
typedef struct
	{
	GList *creflist;
	GList *objlist;
	}
DbDeleteList;

gint db_del_objectrec(Object * referent, DbUniqueId * objid);
void db_del_cleanup(DbDeleteList * dellist);
void db_del_objectrecinit(void);
void db_del_objectreccleanup(void);

#endif
