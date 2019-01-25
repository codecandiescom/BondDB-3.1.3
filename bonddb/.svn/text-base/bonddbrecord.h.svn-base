#ifndef __bonddbrecord_h
#define __bonddbrecord_h

typedef struct _BonddbRecord BonddbRecord;

#include "bonddbobj.h"

struct _BonddbRecord
{
	BonddbClass *parent;
	/* flags */
	gboolean visible;
	gboolean cached;

	/* orginal row it refers */
	gint row;
	/* If its read or new record to be inserted */
	BonddbClassMode mode;
	/* gchar* list of fields, NULL's will be present for those not 
	 * retrieved or set etc */
	void **cache;
	gint numcache;

	BonddbId *id;
};

void bonddb_record_freeall(BonddbClass *obj);
BonddbRecord* bonddb_record_populate(BonddbClass *obj,gint row);	
gint bonddb_record_read(BonddbClass *obj, gint fieldpos, gint row, 
		gchar **retstr);
gint bonddb_record_write(BonddbClass *obj, gint fieldpos, gint row, 
		gchar *value, gchar **retval);
gint bonddb_record_delete(BonddbClass *obj, gint row, gchar *tablename);
gint bonddb_record_undo(BonddbClass *res, gint row);
void bonddb_record_debug(BonddbClass *res);

#endif

