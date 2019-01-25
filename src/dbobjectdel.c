/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 

/**
 * Francis: Started writing since Oct 9, 2002. The main goal is to
 * handle recursive deletion completely.
 *
 * Things to be considered:
 *
 * (1) Since deleting process may require multiple delete step, at
 * later step, if error occurs, this could destroy to protect data
 * integrity
 
 * (2) Following referent can cause cyclic situation and this will
 * cause a memory run out.
 *
 * (3) Although it is not too much concern it * may take long time to
 * execute this function.
 **/
#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "dbobjectdel.h"
#include "db.h"
#include "dbobject.h"
#include "dbconstraint.h"
#include "dbgather.h"
#include "bc.h"

struct UniqRecordDef
	{
	Object *obj;
	DbUniqueId *objid;
	};

static GHashTable *delobjhashtab = NULL;
#if 0
static void
db_del_debug(Object * obj)
	{
	int i;

	message("#################################");
	printf("Name: %s, Num: %d, Row: %d, Current cache: %p\n", 
			obj->basetable, obj->num, obj->row, obj->currentcache);
	printf("Numcache: %d, Cache: %p, FilterApplied: %p\n", obj->numcache, 
			obj->cache, obj->filterapplied);

	for (i = 0; i < obj->numfield; i++)
		{
		if (obj->currentcache)
			printf("%s\t%s", obj->field[i]->field, 
					(gchar*)obj->currentcache->value[i]);
		else
			{
			printf("%s\t CURRENT CACHE NULL", obj->field[i]->field);
			}
		}
	puts("");
	message("#################################");
	}
#endif
/**
 * db_del_getcreflist:
 * @baseclass:
 * 
 * Return a list of crefs holding the table names that are to be deleted.
 */

static GList *
db_del_getcreflist(DbTableDef * baseclass)
	{
	GList *retlist = NULL;
	GList *walk;
	DbConstraintDef *cons;

	/* only pick tables that references from here to someone else, like only one to many relationships */

	/* get first item in list of tables this relates to */
	walk = g_list_first(baseclass->cref);
	for (; walk; walk = walk->next)
		{
		cons = walk->data;
		/* delete only hard links */
		if (strcmp(cons->table[1], baseclass->name) == 0 && 
				cons->casual == 0)
			{
			retlist = g_list_append(retlist, cons);
			}
		}

	return g_list_first(retlist);
	}

/**
 * db_del_getrowlist:
 * 
 * Get a GList of child objects.
 */

static GList *
db_del_getrowlist(Object * obj, DbUniqueId * objid)
	{
	gchar *value;
	Object *childobj;
	GList *walk;
	GList *children;
	GList *retlist = NULL;
	int i, numrows;

	DbConstraintDef *cons;
	DbTableDef *referenttable;
	struct UniqRecordDef *row;

	referenttable = db_findtable(obj->basetable);
	children = db_del_getcreflist(referenttable);
	/* begin */
	walk = g_list_first(children);
	for (; walk; walk = walk->next)
		{
		cons = walk->data;
		g_assert(cons);

		/* get information from parent object */
		db_getvalue(obj, cons->column[1], NULL, &value);
		debugmsg("Loading children from %s.%s where value is %s", 
				cons->table[0], cons->column[0], value);

		/* check if value is valid, if not ignore and continue */
		if (value == NULL || strlen(value) == 0)
			{
			continue;
			}

		/* load other table up */
		childobj = db_createobject(cons->table[0]);
		db_getobjectbyfield(childobj, cons->column[0], value);

		/* insert into return list, dont add if object is emtpy */
		if (db_isnewrecord(childobj) == FALSE && 
				db_isrecordset(childobj) == TRUE)
			{

			numrows = db_numrows(childobj);
			printf("numrows :%d\n", numrows);
			for (i = 0; i < numrows; i++)
				{
				db_moveto(childobj, i);
				row = mem_alloc(sizeof(struct UniqRecordDef));
				row->obj = childobj;
				row->objid = db_id_getbyrow(childobj, i);
				/* db_del_debug(childobj); */
				/* prepending for speed */
				retlist = g_list_prepend(retlist, row);
				}

			/* Memorise which tables I've created. I should free them later */
			if (!g_hash_table_lookup(delobjhashtab, childobj))
				{
				g_hash_table_insert(delobjhashtab, childobj, 
						childobj);
				}
			}
		else
			{
			db_freeobject(childobj);
			}

		}

	return g_list_first(retlist);
	}

/**
 * db_del_objectrec:
 * @parentobj:
 * @objidlist: list of row to be deleted
 *
 * Francis: This is a recursive function basically working as post visiting way.
 * WARNING: USE TRANSACTION WHEN CALLING THIS FUNCTION FOR THE FIRST TIME AND
 *          AFTER THIS.
 */
gint
db_del_objectrec(Object * referent, DbUniqueId * objid)
	{
	GList *walk;
	GList *predeleterowlist = NULL;
	struct UniqRecordDef *row;

	/* Rows which should be deleted first before the deletion of */
	predeleterowlist = db_del_getrowlist(referent, objid);

	walk = g_list_first(predeleterowlist);
	for (; walk; walk = walk->next)
		{
		row = walk->data;
		g_assert(row);
		db_del_objectrec(row->obj, row->objid);
		}

	/* At this point everything should have been taken care of, so that 
	 * delete with out any problem. */
	db_moverecordbyoid(referent, objid);
	db_obj_dodelete(referent);

	/* FIXME: db_bureaucrat_checkmisticconnections(referent, DbCache *cache); */

	/* Cleanup */
	walk = g_list_first(predeleterowlist);
	for (; walk; walk = walk->next)
		{
		mem_free(walk->data);
		}
	g_list_free(predeleterowlist);

	return 0;
	}

void
db_del_objectrecinit(void)
	{
	delobjhashtab = g_hash_table_new(NULL, NULL);
	/* FIXME: Begin transaction */
	}

static void
db_del_freeobject(gpointer key, gpointer value, gpointer user_data)
	{
	db_freeobject(g_hash_table_lookup(delobjhashtab, key));
	}

void
db_del_objectreccleanup(void)
	{
	g_hash_table_foreach(delobjhashtab, db_del_freeobject, NULL);
	/* FIXME: End transaction */
	}
