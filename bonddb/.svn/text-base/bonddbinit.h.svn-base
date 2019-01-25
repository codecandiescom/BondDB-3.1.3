#ifndef __bonddbinit_h
#define __bonddbinit_h

/* dbclient, by andru hill
 *
 * Esblish connections to a postgresql database, get configuration information.
 */

#include "bonddbglobal.h"

#define GETCONNSTR_ALL 0
#define GETCONNSTR_REMOTE_WRITE 1

BonddbGlobal * bonddb_init(gchar * conninfo, gchar *driver, GList **list);
gint bonddb_cleanup(BonddbGlobal *bonddb);
BonddbPlugin* bonddb_get_dbplugin(BonddbGlobal *bonddb);
gchar * bonddb_connect_errormsg(void);
GList * bonddb_get_grouplist(BonddbGlobal *bonddb, gchar *username);

#endif
