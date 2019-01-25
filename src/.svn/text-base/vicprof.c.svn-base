#include <stdio.h>
#include <glib.h>

#include "bc.h"
#include "db.h"

void
vic_testspeed(void)
	{
	Object *obj;
	GList *requestlist;

	message("STARTING TESTING");
	obj = db_createobject("membership");

	db_getobjectbyfield(obj, "programmegroupid", "2");
	requestlist = db_filter_addrequestlist(NULL, "id", NULL);
	requestlist = db_filter_addrequestlist(requestlist, "memberid", NULL);
	requestlist = db_filter_addrequestlist(requestlist, "programmeid", NULL);
	requestlist = db_filter_addrequestlist(requestlist, "name", "programme");
	db_filter_load(obj, requestlist);
	debugmsg("%d of %d records showing", db_numrecord(obj), obj->num);
	db_freeobject(obj);
	debugmsg("ENDING TESTING");
	db_cleanup();
	}

int
main(int argc, char *argv[])
	{
	bc_init(argc,argv);
        db_init(bcconfig_getvalue("db_connection_string"));
	
	vic_testspeed();
	bc_cleanup();
	return 0;
	}

