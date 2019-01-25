#ifndef __dblog_h
#define __dbloh_h

#include <glib.h>

typedef struct
   {
   gint instance;
   }
DbLog;

/* adds an item to the log */
gint db_log_add(gint instance);

/* get all log entries */
GList *db_log_get(void);

/* empties log */
void db_log_free(void);
void db_lomem_free(void);

#endif
