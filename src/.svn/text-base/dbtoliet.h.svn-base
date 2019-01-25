/* Developed by treshna Enterprises Ltd
 *
 * Unlike what the name describes, this is the write ahead caching system for objects
 *	It is closely related to the Objects.  The reason why it is
 * called dbtoliet is the inherltly large amount of flushing required.
 *
 * This file was started due to the inherent problems that arise from
 * write ahead cache due to stale objects, and wanting to read from freshly
 * written objects. This can not happen, as once an object has been written 
 * to the database you can not read straight back from it.  
 *
 * PLEASE REFER TO dbcache.h FOR MORE DETAILS AS A LOT OF CODE WAS MOVED THERE
 */

#ifndef dbtoilet_h
#define dbtoilet_h

#include "dbcache.h"

/*==============================================================================================
* Cache Management
*==============================================================================================
*/
/* These functions are been moved to dbcache */

/*==============================================================================================
* Cache Flushing
*==============================================================================================
*/

gint db_toliet_flushall(void);
gint db_toliet_flush(Object * obj, DbCache * cache);
gint db_toliet_flushobject(Object * obj);
gint db_toliet_sqlinsert(Object * obj, DbCache * c, gchar ** query);
gint db_toliet_sqlupdate(Object * obj, DbCache * c, gchar *tableopr, GList *tablespassed);
gint db_toliet_setwrote(DbCache * cache);
gint db_toliet_checkfordefaults(Object *obj, DbCache *cache);

#endif
