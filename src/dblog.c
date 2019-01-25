#include <glib.h>

#include "dblog.h"
#include "bc.h"

GList *globaldblog;

/**
 * db_log_add:
 * @instance: instance number of an object
 * 
 * Adds a log entry with that instance you specify.
 *
 * Returns: non-zero on failure
 */
gint
db_log_add(gint instance)
   {
   DbLog *log;
   GList *walk = g_list_first(globaldblog);

   /* see if a unique occurance has already occured */
   while (walk != NULL)
      {
      log = walk->data;
      g_assert(log);
      if (log->instance == instance)
         return -1;
      walk = walk->next;
      }

   log = (DbLog*) mem_alloc(sizeof(DbLog));
   log->instance = instance;
   globaldblog = g_list_append(g_list_first(globaldblog), log);
   return 0;
   }

/**
 * db_log_get:
 * 
 * Returns a list of all the log entries, do not free this up and return to the starting
 * position when your done.
 *
 * Returns: GList of all log entries, %NULL on an empty log.
 */
GList *
db_log_get(void)
   {
   return globaldblog;
   }

/**
 * db_lomem_free:
 * 
 * Empties the log of all entries and frees any assocated memory.
 *
 * Returns: GList of all log entries.
 */
void
db_lomem_free(void)
   {
   DbLog *log;
   GList *walk = globaldblog;

   while (walk != NULL)
      {
      log = walk->data;
      g_assert(log);
      mem_free(log);
      walk = walk->next;
      }
   g_list_free(globaldblog);
   globaldblog = NULL;
   }
