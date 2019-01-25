#ifndef __dbclient_h
#define __dbclient_h

/* dbclient, by andru hill
 *
 * Esblish connections to a postgresql database, get configuration information.
 */

gint db_init(gchar * conninfo);
gint db_cleanup(void);
gint db_restart(void);

gint db_loadbindings(gchar *driver);

#endif
