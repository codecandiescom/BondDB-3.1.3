#ifndef bc_h
#define bc_h

/* Warning, you normally have to include glib.h in your code also. */
#include <glib.h>

#include "bcmem.h"
#include "bcdebug.h"
#include "bcconfig.h"
#include "bcgarbage.h"

gint bc_init(gint argc, gchar ** argv);
gint bc_cleanup(void);

#endif
