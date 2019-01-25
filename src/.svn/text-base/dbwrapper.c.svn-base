#include <stdio.h>
#include <glib.h>

#include "bc.h"
#include "dbwrapper.h"

/* Global pointer of current connection to the database */
DbConnection *globaldbconn;

/**
 * db_dbgetserver:
 *
 * Inform you of what type of database server your connected to.
 */
gint
db_dbgetserver()
   {
   if (globaldbconn == NULL)
      {
      errormsg("globaldbconn is null, initilise database first");
      return -1;
      }
   return globaldbconn->dbserver;
   }
