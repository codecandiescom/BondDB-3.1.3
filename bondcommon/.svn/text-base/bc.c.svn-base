#include <stdlib.h>
#include <string.h>

#include "bc.h"

gint
bc_init(gint argc, gchar ** argv)
{
	debug_init();
	return bcconfig_init(argc, argv);
}

gint
bc_cleanup(void)
{
	char *debugmemory;
	int dodebug=0;
		
	/* check debug setting in config file. */
	debugmemory = bcconfig_getvalue("debug_memdump");
	if (debugmemory)
		dodebug=!strcmp(debugmemory,"true") || atoi(debugmemory);

	/* clean up the config files. */
	bcconfig_cleanup();
	/* clean up the debuging output. */
	debug_cleanup();
	
    /* If config file asked for a memory dump do one */
	if (dodebug)
	     mem_display();
	   
	return 0;
}
