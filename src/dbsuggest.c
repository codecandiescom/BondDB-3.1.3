#include <string.h>
#include <glib.h>

#include "dbsuggest.h"
#include "dbobject.h"
#include "dbsqlparse.h"
#include "dbmapme.h"
#include "bc.h"

static DbSuggest *db_suggest_create();

/**
 * db_suggest_all:
 * @obj: database object
 * @fieldname: suspect field name
 * @tablename: suspect table name or %NULL
 *
 * From limited information find out where to get the information you want
 * and what the real field and table name is.  This will also generate an object
 * which correctly points to your real source of data.  suggest->err will be set 
 * to a non-zero if it failed to make any headway on its mission of finding your true
 * origins.
 * 
 * Returns: DbSuggest, which you should free up after you use it.
 */
DbSuggest *
db_suggest_all(Object * obj, gchar * _fieldname, gchar * _tablename)
   {
   gchar *tablename, *fieldname;
   DbSuggest *suggest = db_suggest_create();

   /* suggest a field and table this could point to */
   tablename = db_sqlparse_suggesttable(_fieldname);
   fieldname = db_sqlparse_suggestfield(_fieldname);

   if (tablename == NULL)
      tablename = _tablename;
   if (fieldname == NULL)
      fieldname = _fieldname;
   /* if tablename is null then its the current table */
   if (tablename == NULL)
      tablename = obj->basetable;
   /* put those values in there */
   suggest->tablename = tablename;
   suggest->fieldname = fieldname;
   /* find the field defination, though this may have a few problems */
   suggest->field = db_field_dbfind(obj, fieldname, tablename);
   /* if current table then you can get out */
   g_assert(obj->basetable);
   if (suggest->field != NULL)
      {
      suggest->obj = obj;
      return suggest;
      }
   /* load from somewhere else */
   suggest->obj = db_mapme_findobjectbytable(obj, tablename);
   if (suggest->obj == NULL)
      {
      errormsg("Unable to map to %s from %s", tablename, obj->basetable);
      suggest->err = -1;
      }

   return suggest;
   }

/**
 * db_suggest_wildcard:
 * @inputfield: input string in the sql statement
 * @fieldname: returned fieldname
 * @tablename: returned tablename
 *
 * Will return 0 on a straght table.name with @fieldname and @tablename been
 * correctly set.  If its table.* then it returns 1 to indicate that it has a wild
 * card and then sets fieldname to NULL, else if its just * then return 1 but have
 * fieldname and talblename as %NULL.  If it just contains a fieldname, ie name with no
 * . in it, it will set @fieldname to @inputfield.
 *
 * Returns: non-zero on error
 */
gint
db_suggest_wildcard(gchar * inputfield, gchar ** fieldname, gchar ** tablename)
   {
   gint i, j, containdot = 0;

   *tablename = NULL;
   *fieldname = NULL;

   if (strcmp("*", inputfield) == 0)
      return 1;
   /* check if it contains a dot in the string */
   j = strlen(inputfield);
   if (j > 1)
      for (i = 1; i < j; i++)
         if (inputfield[i] == '.')
            containdot = 1;
   i = 0;
   /* go and make a tablename hmm */
   if (containdot == 1)
      {
      (*tablename) = (gchar *) mem_alloc(strlen(inputfield) + 1);

      for (i = 0; i < strlen(inputfield); i++)
         {
         if (inputfield[i] == '.')
            {
            (*tablename)[i] = 0;
            i++;
            break;
            }
         (*tablename)[i] = inputfield[i];
         }
      (*tablename)[i] = 0;
      }
   /* check if tablename.* wildcard thing is like happening */
   if (inputfield[i] == '*')
      return 1;

   (*fieldname) = (gchar *) mem_alloc(strlen(inputfield) + 1);

   for (j = 0; i < strlen(inputfield); i++, j++)
      (*fieldname)[j] = inputfield[i];
   (*fieldname)[j] = 0;
   /* debugmsg("table %s, field %s",*tablename, *fieldname); */
   return 0;
   }

static DbSuggest *
db_suggest_create()
   {
   DbSuggest *suggest;

   suggest = (DbSuggest *) mem_alloc(sizeof(DbSuggest));
   suggest->err = 0;
   suggest->tablename = NULL;
   suggest->fieldname = NULL;
   suggest->obj = NULL;
   return suggest;
   }

/**
 * db_suggest_free:
 *
 * Free a suggestion.
 *
 */
void
db_suggest_free(DbSuggest *suggest)
	{
	mem_free(suggest);
	}
