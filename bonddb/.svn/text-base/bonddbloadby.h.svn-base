#ifndef __bonddbloadby_h
#define __bonddbloadby_h

#include "bonddbobj.h"

typedef struct _BonddbReference BonddbReference;

struct _BonddbReference
{
	gchar *fromtable;
	gchar *fromfield;
	gchar *totable;
	gchar *tofield;
	gboolean onetomany;
	gint fromcount;
	gint tocount;
};


gint bonddb_loadobjectbyobjects(BonddbClass * obj, GList * srcobjs, gchar **errmsg);

gint bonddb_ref_addbirth(BonddbClass *destobj, gchar *leftfield, gchar *rightvalue);
gint bonddb_ref_add(BonddbClass *obj, gchar *fromtable, gchar *fromfield,
		gchar *totable, gchar *tofield);
gint bonddb_ref_clear(BonddbClass *obj);
gint bonddb_ref_autoadd(BonddbClass *obj);
gint bonddb_ref_test(BonddbGlobal *bonddb, gint testnum);
	

#endif
