#include <string.h>
#include <stdlib.h>
#include <glib.h>

#include "bc.h"
#include "dboo.h"
#include "db.h"
#include "dbconstraint.h"
#include "dbsqlparse.h"
#include "dbgather.h"
#include "advstring.h"

#ifdef __PORTED

/* Generate a path from a string, this will do some serous recusivness to create a OOPath */
OOPath *
db_oo_generatepath(gchar * request, gchar * base)
   {
   OOPath *path;
   GList *constraints;

   g_assert(request);

   path = (OOPath *) mem_alloc(sizeof(OOPath));
   path->tablepath = NULL;
   path->field = NULL;

   constraints = globaldb->constraints;
   if (constraints == NULL)
      {
      mem_free(path);
      return NULL;
      }
   /* go recusivily and building cunning path */
   db_oo_findpaths(path, constraints, request, base, 0);

   return path;
   }

/* Primary Function for generating paths */
gint
db_oo_findpaths(OOPath * path, GList * constraints, gchar * request, gchar * base, gint pos)
   {
   gint i, slen;
   gint spos;
   gchar *partstr, *prevstr, *fieldstr;
   GList *tblist = NULL;
   OOPathElement *elem;

   /* partstr is work in progress */
   /* confirmstr is the actual one in the database */
   /* prevstr is the previsou confirmed str */

   g_assert(path);
   g_assert(constraints);

   slen = strlen(request);
   spos = pos;
   prevstr = base;

   for (i = pos; i < slen; i++)
      {
      if (request[i] == '.')
         {
         partstr = db_oo_getpathstrseg(request, spos, i);
         /* add the base table, where it all comes from, if needed and hasn't been mentoned */
         if (partstr == NULL)
            break;
         /* get the elements of the table, the more complex bit here */
         elem = db_oo_verifytable(globaldb, constraints, partstr, prevstr);
         mem_free(partstr);

         /* check if last field in service, this is kindaof skipping ahead */
         if (db_oo_lastfield(request, i + 1) == 1)
            {
            fieldstr = db_oo_getpathstrseg(request, i + 1, slen);
            /* check for wildcard of all fields */
            if (fieldstr[0]!='*')
	         	path->field = mem_strdup(fieldstr);
	         else  
		        	path->field = db_oo_verifyfield(globaldb, fieldstr, db_oo_getpathtable(elem));
            mem_free(fieldstr);
            }

         /* say the first one has been added to the list and add the next bit */
         tblist = g_list_append(tblist, elem);
         prevstr = elem->c->table[elem->direction];
         /* reindex the starting position */
         i++;
         spos = i;
         }
      }
   path->tablepath = tblist;
   return 0;
   }

/* cut a bit out of a string */
gchar *
db_oo_getpathstrseg(gchar * request, gint startpos, gint endpos)
   {
   gint i;
   gchar *retstr = (gchar *) mem_alloc(sizeof(gchar) * ((endpos - startpos) + 1));

   for (i = startpos; i < endpos; i++)
      retstr[i - startpos] = request[i];
   retstr[i - startpos] = 0;
   return retstr;
   }

/* standard cleanup function, take note that the ath->tablepath->data stuff is allocated */
void
db_oo_freepath(OOPath * path)
   {
   GList *walk;

   g_assert(path);
   walk = g_list_first(path->tablepath);
   while (walk != NULL)
      {
      mem_free(walk->data);
      walk = walk->next;
      }
   g_list_free(path->tablepath);
   mem_free(path);
   }

/* is this the last element in a blah.blah.blah string? */
gint
db_oo_lastfield(gchar * request, gint pos)
   {
   gint i, slen;

   g_assert(request);
   slen = strlen(request);
   for (i = pos; i < slen; i++)
      if (request[i] == '.')
         return 0;
   return 1;
   }

/* Given a table of what you think, and a table which you are refering it from work out
	what the table name actually is. Also work out the direction of the relationship.
	-1 is no relationship, 0 is tableto is a subclass of tablefrom, 1 is tableto is the
	parent of tablefrom */
OOPathElement *
db_oo_verifytable(DbDatabaseDef * db, GList * constraints, gchar * tableto, gchar * tablefrom)
   {
   gint j, i, cmp;
   OOPathElement *elem;

   g_assert(db);
   g_assert(constraints);
   g_assert(tableto);
   g_assert(tablefrom);

   for (j = 0; j < 5; j++)
      {
      for (i = 0; i < db->numtable; i++)
         {
         cmp = adv_strcmp(db->table[i]->name, tableto);
         /* we have a potential match, check it to be certian */
         if (cmp == j && cmp >= 0)
            {
            /* woohoo relationship exists and we have a match */
            if ((elem = db_oo_hasrelationship(constraints, db->table[i]->name, tablefrom)) != NULL)
               {
               return elem;
               }
            }
         }
      }

   return NULL;
   }

/* deduce if there is a relationship that exists between the tableto and tablefrom,
	1 is returned if there is a relationship, 0 if there isn't. 2 if its a reverse relationship */
OOPathElement *
db_oo_hasrelationship(GList * constraints, gchar * tableto, gchar * tablefrom)
   {
   GList *walk;
   DbConstraintDef *c;
   OOPathElement *elem;

   walk = g_list_first(constraints);
   while (walk != NULL)
      {
      c = walk->data;
      g_assert(c);
      if (c->casual == 0)
         {
         /* Child relationship */
         if (strcmp(c->table[0], tablefrom) == 0)
            if (strcmp(c->table[1], tableto) == 0)
               {
               elem = (OOPathElement *) mem_alloc(sizeof(OOPathElement));
               elem->c = c;
               elem->direction = 1;
               elem->tabledef = NULL;
               return elem;
               }
         /* Parent relationship */
         if (strcmp(c->table[1], tablefrom) == 0)
            if (strcmp(c->table[0], tableto) == 0)
               {
               elem = (OOPathElement *) mem_alloc(sizeof(OOPathElement));
               elem->c = c;
               elem->direction = 0;
               elem->tabledef = NULL;
               return elem;
               }
         }
      walk = walk->next;
      }
   return NULL;
   }

gchar *
db_oo_verifyfield(DbDatabaseDef * db, gchar * field, gchar * table)
   {
   gint i, j, cmp;
   DbTableDef *tb;

   g_assert(db);

   tb = db_findtable(table);
   if (tb == NULL)
      {
      errormsg("Unable to find table %s", table);
      return NULL;
      }

   /* now find the best match */
   for (j = 0; j < 5; j++)
      {
      for (i = 0; i < tb->num; i++)
         {
         cmp = adv_strcmp(tb->field[i]->name, field);
         /* we have a potential match, check it to be certian */
         if (cmp == j && cmp >= 0)
            {
            /* Match found. */
            return tb->field[i]->name;
            }
         }
      }
   return NULL;
   }

/* Just show the debugging info */
void
db_oo_pathdebug(OOPath * path)
   {
   GList *walk;
   OOPathElement *elem;

   g_assert(path);
   walk = g_list_first(path->tablepath);
   while (walk != NULL)
      {
      elem = walk->data;
      debugmsg("Table:%s.%s->%s.%s", (gchar *) elem->c->table[1 - elem->direction],
               (gchar *) elem->c->column[1 - elem->direction],
               (gchar *) elem->c->table[elem->direction], (gchar *) elem->c->column[elem->direction]);
      walk = walk->next;
      }
   debugmsg("Field:%s", (gchar *) path->field);
   }

gchar *
db_oo_getpathtable(OOPathElement * elem)
   {
   g_assert(elem);
   return elem->c->table[elem->direction];
   }

/* Ok like the element paths, but do it a bit more jumper. */
gint
db_oo_seekpaths(OOPath * path, GList * constraints, gchar * request, gchar * base, gint pos)
{
   gint i, slen;
   gint spos;
   gchar *partstr, *prevstr, *fieldstr;
   GList *tblist = NULL;
   OOPathElement *elem;

   /* partstr is work in progress */
   /* confirmstr is the actual one in the database */
   /* prevstr is the previsou confirmed str */

   g_assert(path);
   g_assert(constraints);

   slen = strlen(request);
   spos = pos;
   prevstr = base;

   for (i = pos; i < slen; i++)
      {
      if (request[i] == '.')
         {
         partstr = db_oo_getpathstrseg(request, spos, i);
         /* add the base table, where it all comes from, if needed and hasn't been mentoned */
         if (partstr == NULL)
            break;
         /* get the elements of the table, the more complex bit here */
         elem = db_oo_tabledig(globaldb, constraints, partstr, prevstr);
         mem_free(partstr);

         /* check if last field in service, this is kindaof skipping ahead */
         if (db_oo_lastfield(request, i + 1) == 1)
            {
            fieldstr = db_oo_getpathstrseg(request, i + 1, slen);
            path->finaltable = db_oo_getpathtable(elem);
            path->field = db_oo_verifyfield(globaldb, fieldstr,path->finaltable);
            mem_free(fieldstr);
            }

         /* say the first one has been added to the list and add the next bit */
         tblist = g_list_append(tblist, elem);
         prevstr = elem->c->table[elem->direction];
         /* reindex the starting position */
         i++;
         spos = i;
         }
      }
   path->tablepath = tblist;
   return 0;
}


OOPathElement *
db_oo_tabledig(DbDatabaseDef * db, GList * constraints, gchar * tableto, gchar * tablefrom)
{
	
	return NULL;
}
#endif
