#ifndef __bonddbkey_h
#define __bonddbkey_h

#include "bonddbobj.h"

typedef struct _BonddbPrimaryKey BonddbPrimaryKey;

struct _BonddbPrimaryKey 
{
	gchar *tablename;
	GList *fieldlist;
};

gint bonddb_primarykey_add(BonddbClass *obj, gchar *tablename, GList *fields);
gint bonddb_primarykey_guess(BonddbClass *obj, gchar *tablename);
void bonddb_primarykey_free(BonddbClass *obj);

#endif
