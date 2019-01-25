#ifndef bonddbarray_h
#define bonddbarray_h

gchar ** bonddb_array_extract(BonddbClass * obj, GList * fields, 
		gint * retnumrow, gint * retnumcol, gboolean getid);

#endif
