#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <glib.h>

#include "dbmapme.h"
#include "dbobject.h"
#include "db.h"
#include "dbconstraint.h"
#include "bc.h"
#include "dbsqlparse.h"
#include "dbobjectnav.h"

/* is there a source to this ... look for field name in other tables
   Returns the classname */
gchar *
db_mapfieldsource(DbTableDef * def, gchar * fieldname)
   {
   GList *walk;
   gchar *suggesttable;
   DbConstraintDef *c;
   DbTableDef *dt;
   DbFieldDef *df;

   suggesttable = db_sqlparse_suggesttable(fieldname);
   if (suggesttable != NULL)
      {
      dt = db_findtable(suggesttable);
      if (dt != NULL)
         {
         /* see if the field occurs in this table */
         df = db_findfield(dt, fieldname);
         if (df != NULL)
            {
            mem_free(suggesttable);
            return dt->name;
            }
         }
      mem_free(suggesttable);
      }
   /* ok so isnt, just look for field */
   g_assert(def);
   walk = g_list_first(def->cref);
   while (walk != NULL)
      {
      c = walk->data;
      g_assert(c);
      /* get the other table name */
      if (strcasecmp(c->table[0], def->name) == 0)
         suggesttable = c->table[1];
      else
         suggesttable = c->table[0];

      dt = db_findtable(suggesttable);
      if (dt != NULL)
         {
         /* see if the field occurs in this table */
         df = db_findfield(dt, fieldname);
         if (df != NULL)
            return suggesttable;
         }
      walk = walk->next;
      }
   return NULL;
   }

Object *
db_getmapbyclass(Object * obj, gchar * classname)
   {
   Object *c;
   GList *walk;

   g_assert(obj);
   g_assert(classname);

   db_mapme_fixsync(obj);
   walk = g_list_first(obj->mapobject);
   while (walk != NULL)
      {
      c = walk->data;
      if (strcasecmp(c->basetable, classname) == 0)
         return c;
      walk = walk->next;
      }
   return NULL;
   }

/* Warning, do not free object after is has been returned */
Object *
db_mapacrossobjects(Object * obj, gchar * classname)
   {
   GList *walk;
   Object *c, *retobj = NULL;

   db_mapme_fixsync(obj);
   c = obj;
   if (strcasecmp(c->basetable, classname) == 0)
      retobj = c;
   else
      {
      walk = g_list_first(c->mapobject);
      while (walk != NULL)
         {
         c = walk->data;
         if (strcasecmp(c->basetable, classname) == 0)
            {
            retobj = c;
            break;
            }
         walk = walk->next;
         }
      }
   return retobj;
   }

/* creates a map to another object in an object */
gint
db_createmapobject(Object * obj, gchar * classname)
   {
   gint retval = 0;
   Object *newobj, *c;
   GList *walk;

   db_mapme_fixsync(obj);
   g_assert(obj);
   g_assert(classname);

   walk = obj->mapobject;
   while (walk != NULL)
      {
      c = walk->data;
      if (strcasecmp(c->basetable, classname) == 0)
         {
         warningmsg("trying to create a map that is already mapped. will abort map creation");
         return -1;
         }
      walk = walk->next;
      }

   newobj = db_createobject(classname);
   g_assert(newobj);
   retval = db_loadobjectbyobject(obj, newobj, classname);
   if (retval < 0)
      warningmsg("error occurred during a map to %s from %s", classname, obj->basetable);
	else
	if (retval == 0)
		obj->mapobject = g_list_append(obj->mapobject, newobj);
   return retval;
   }

gint
db_freemapobject(Object * obj)
   {
   GList *walk;
   Object *_obj;

   g_assert(obj);

   walk = g_list_first(obj->mapobject);

   while (walk != NULL)
      {
      _obj = walk->data;
      db_freeobject(_obj);
      walk = walk->next;
      }
   g_list_free(obj->mapobject);
   obj->mapobject = NULL;
   return 0;
   }

gint
db_mapme_debug(Object * _obj)
   {
   GList *walk;
   Object *obj;

   walk = g_list_first(_obj->mapobject);
   debugmsg("Map me print out of debugging info");
   while (walk != NULL)
      {
      obj = walk->data;
      g_assert(obj);
      if (obj != NULL)
         {
         debugmsg("MAPME: %s, items:%d\n", obj->basetable, obj->num);
         }
      walk = walk->next;
      }
   return 0;
   }

/**
 * db_mapme_findobjectbytable:
 * 
 * Given an object and a table go though the _obj and see if there is a
 * object attached by the given name. If not try and attach one. 
 *
 * Returns: an object
 */
Object *
db_mapme_findobjectbytable(Object * _obj, gchar * table)
   {
   Object *obj = NULL;			  /* returning object */
	/* Check if its not a dead record your mapping across. */
	if (_obj->num == 0)
		return NULL;

   db_mapme_fixsync(_obj);
   obj = db_mapacrossobjects(_obj, table);
   if (obj == NULL)
      {
      if (db_createmapobject(_obj, table) != 0)
         return NULL;
      obj = db_mapacrossobjects(_obj, table);
      }
   return obj;
   }

/* given an object it goes though and recreates all umm.... objects to point to right places
   TODO. this whole section, handling thing needs to be written sometime to be less intensive
   to the database. The dboo module will ultimately replace this once it is complete. */
gint
db_mapme_realignobjects(Object * obj)
   {
   gint retval = 0;
   Object *c;
   GList *walk;

   g_assert(obj);
   obj->mapoutofsync = FALSE;

   if (db_isnewrecord(obj) == TRUE)
      {
      /* debugmsg("New object, i dont have to realign here"); */
      walk = g_list_first(obj->mapobject);
      while (walk != NULL)
         {
         c = walk->data;
         db_obj_clear(c);
         walk = walk->next;
         }
      return retval;
      }

   
	/* debugmsg("%d items in the mapobject",g_list_length(obj->mapobject)); */
   for (walk = g_list_first(obj->mapobject); walk != NULL; walk=walk->next)
		{
		c = walk->data;
		/* debugmsg("table %s",c->basetable); */
		}
   for (walk = g_list_first(obj->mapobject); walk != NULL; walk=walk->next)
      {
      c = walk->data;
		retval = db_loadobjectbyobject(obj, c, c->basetable);
      }
   return retval;
   }

/* Copy one map object to anothers.
	The other object needs the mapobjects to be set to null.
 */
gint
db_copymapobject(Object * retobj, Object * obj)
   {
   Object *c;
   GList *walk, *retlist = NULL;

   g_assert(!retobj->mapobject);
   db_mapme_fixsync(obj);
   walk = g_list_first(obj->mapobject);
   while (walk != NULL)
      {
      c = walk->data;
      g_assert(c);
		warningmsg("THIS ISN'T PORTED");
#ifdef __PORTED

      retlist = g_list_append(retlist, db_copyobject(c, -1));
#endif

      walk = walk->next;
      }
   retobj->mapobject = retlist;
   return 0;
   }

/**
 * db_mapme_fixsync:
 *
 * If an object is out of sync (happens sometimes to keep speed good) bring it back inalign
 */
void
db_mapme_fixsync(Object * obj)
   {
   g_assert(obj);
   if (obj->mapoutofsync == TRUE)
      db_mapme_realignobjects(obj);
   }

