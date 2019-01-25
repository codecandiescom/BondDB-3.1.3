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

#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "bc.h"

#include "db.h"
#include "dbclient.h"
#include "dbgather.h"
#include "dbsqlparse.h"
#include "dblooksee.h"
#include "dbconstraint.h"
#include "dbobject.h"
#include "dbmapme.h"
#include "dboo.h"
#include "dbuniqueid.h"
#include "dbbirth.h"
#include "dbfield.h"
#include "dbsuggest.h"
#include "dbfilter.h"
#include "sql_parser.h"
#include "dbmethod.h"
#include "dbsync.h"
#include "dbmasteroftable.h"

/*gint test_result(gchar * retstr, gchar * shouldbe, gint * pass);*/
gint test_total(gint pass, gint total);

extern int globalwatchmemoryid;

int
main(int argc, char *argv[])
	{
	DbDatabaseDef *db;
	gchar *retstr;
	gint test = 0;
	gint i, retval, connect;
	gint pass = 0;
	GList *walk;
	DbTableDef *tb;
	Object *obj;

	if (argc <= 1)
		{
		/* run your a specific test here on default */
		test = 0;
		}
	else
		test = atoi(argv[1]);
	if (argc >= 3)
		setdebuglinenum(1);
	bc_init(argc,argv);
	connect = db_init(NULL);
	if (connect != 0)
		{
		errormsg("No database found");
		exit(0);
		}
	db = globaldb;

	switch (test)
		{
		/* table info retriveal */
	case 0:
			{
			if (db->numtable <= 3)
			     {				
			     errormsg("Database appears to be missing tables or figuring out what tables there are is not working.");
			     break;
			     }
			debugmsg("Number of tables %d\n"
			         "Number of fields in %s = %d\n", db->numtable, db->table[db->numtable - 3]->name,
			         db->table[db->numtable - 3]->num);
			if (db->numview > 0)
				debugmsg("Number of views %d\n"
				         "Number of fields in %s = %d\n", db->numview, db->view[0]->name, db->view[0]->num);
			else
				debugmsg("No views found\n");

			break;
			}
	case 1:
			{
			/* open and close an object */
			obj = db_createobject("person");
			debugmsg("object has been created, now i shall free");
			db_freeobject(obj);
			break;
			}
	case 2:
			{
			/* run a query */
			obj = db_createobject("person");
			db_getobjectbyfield(obj, "id", "2");
			if (obj->num != 1 && db_id_isnewrecord(obj->id) == FALSE)
				errormsg("TEST FAILED");
			else
				debugmsg("TEST PASSED");
			db_freeobject(obj);
			break;
			}
	case 3:
			{
			/* run a query */
			obj = db_createobject("person");
			g_assert(obj->cache);
			db_getobjectbyfield(obj, "id", "2");
			db_getvalue(obj, "email", NULL, &retstr);
			debugmsg("returned email address: %s", retstr);
			if (retstr == NULL || strcmp(retstr, "andrew@treshna.com") != 0)
				errormsg("TEST FAILED");
			else
				debugmsg("TEST PASSED");
			db_freeobject(obj);
			break;
			}
	case 4:
			{
			/* Load and modify record */
			obj = db_createobject("person");
			db_getobjectbyfield(obj, "id", "2");
			db_getvalue(obj, "lastname", NULL, &retstr);
			if (retstr == NULL)
				errormsg("invalid retval");
			else
				{
				if (strcmp(retstr, "hill") == 0)
					pass++;
				else
					errormsg("%s didn't match thatcher", retstr);
				debugmsg("lastname is %s", retstr);
				}
			db_setvalue(obj, "lastname", NULL, "thatcher");
			db_setvalue(obj, "firstname", NULL, "andru");
			db_setvalue(obj, "email", NULL, "andrew@treshna.com");
			db_getvalue(obj, "lastname", NULL, &retstr);
			if (retstr == NULL)
				errormsg("invalid retval");
			else
				{
				if (strcmp(retstr, "thatcher") == 0)
					pass++;
				else
					errormsg("%s didn't match hill", retstr);
				debugmsg("lastname is %s", retstr);
				}
			db_setvalue(obj, "lastname", NULL, "hill");
			db_freeobject(obj);
			test_total(pass, 2);
			break;
			}
	case 5:
			{
			/* create a new record */
			debugmsg("testing db write to for a new record");
			obj = db_createobject("person");
			db_setvalue(obj, "id", NULL, "24");
			db_setvalue(obj, "lastname", NULL, "hill");
			db_setvalue(obj, "firstname", NULL, "dru");
			db_setvalue(obj, "email", NULL, "dru@treshna.com");
			db_freeobject(obj);

			obj = db_createobject("person");
			db_getobjectbyfield(obj, "id", "24");
			db_getvalue(obj, "email", NULL, &retstr);
			if (retstr == NULL || strcmp(retstr, "dru@treshna.com") != 0)
				;
			else
				pass++;
			db_deleteobject(obj);
			db_freeobject(obj);
			if (pass == 0)
				errormsg("TEST FAILED");
			else
				debugmsg("TEST PASSED");
			break;
			}
	case 6:
			{
			obj = db_sql(NULL, "SELECT * FROM soldier");
			if (db_numrows(obj) == 4)
				debugmsg("TEST PASSED");
			else
				errormsg("TEST FAILED");
			   db_cache_debug(obj);
			db_freeobject(obj);

			obj = db_sql(NULL, "select * from soldier where id=3");
			if (db_numrows(obj) == 1)
				debugmsg("TEST PASSED");
			else
				errormsg("TEST FAILED");
			db_freeobject(obj);
			break;
			}
	case 7:
		     {
			
		/* basic string parsing */
		debugmsg("Testing select statement for sql parsing");
		debugmsg("%d",
		         db_sql_reftotable("SELECT * FROM person, persontype WHERE persontype.id = person.type", "person"));
		if (db_sql_reftotable("SELECT * FROM person, persontype WHERE persontype.id = person.type", "person") == 14)
			debugmsg("TEST PASSED");
		else
			errormsg("TEST FAILED");
		break;
		     }
		   
		/* display a list of views, and information about each view */
	case 8:
			{
			debugmsg("Printing list of views, %d num", db->numview);
			for (i = 0; i < db->numview; i++)
				{
				debugmsg("==== %s ====", db->view[i]->name);
				debugmsg("Query: %s", db->view[i]->query);
				walk = db->view[i]->fromtable;
				while (walk != NULL)
					{
					tb = walk->data;
					g_assert(tb);

					debugmsg("From Table: %s", tb->name);
					walk = walk->next;
					}

				walk = db->view[i]->subtable;
				while (walk != NULL)
					{
					tb = walk->data;
					g_assert(tb);

					debugmsg("Sub table view: %s", tb->name);
					walk = walk->next;
					}
				}
			break;
			}
		/* Test searching for data retrival of records */
	case 9:
			{
			debugmsg("testing db write to for a new record");
				
			obj = db_createobject("person");
			db_sql(obj, "select person.id, person.firstname, person.lastname from person where (id='2')");
			db_getvalue(obj,"email",NULL,&retstr);
			test_result(retstr, "andrew@treshna.com", &pass);
			db_freeobject(obj);

			obj = db_sql(NULL, "select soldier.id,soldier.name from soldier");
			db_getvalue(obj,"name","country",&retstr);
			test_result(retstr, "'New Zealand'", &pass);
			   debugmsg("id %d",mem_getid(obj->query));
			db_freeobject(obj);

			obj = db_sql(NULL, "select soldier.id,soldier.name from soldier where soldier.id='2'");
			db_getvalue(obj,"name","country",&retstr);
			test_result(retstr, "'New Zealand'", &pass);
			db_freeobject(obj); 
			
			test_total(pass, 3); 
			break;
			}
		/* test loading objects from within objects */
	case 10:
			{
			DbLookSee *looksee;

			debugmsg("Testing search capiablities");
			looksee = db_looksee_create("soldier");
			db_looksee_addand(looksee, "name", "Andru", 1, DBTEXT);
			obj = db_looksee_exec(looksee);
			g_assert(obj);
			debugmsg("%d items returned", obj->num);
			db_freeobject(obj);
			break;
			}
	case 11:
			{
			GList *l;
			DbConstraintDef *c;
			DbTableDef *tmptab;

			debugmsg("Testing constraints capabilities");

			for (l = db->constraints; l != NULL; l = l->next)
				{
				c = l->data;
				debugmsg("constraint: %s->%s = %s->%s", c->table[0], c->column[0], c->table[1], c->column[1]);
				}
			tmptab = db_findtable("soldier");
			g_assert(tmptab);
			g_assert(tmptab->cref);

			break;
			}

	case 12:
			{
			debugmsg("testing assigning variables");
			db_generalisetype(db);
			break;
			}
	case 13:
			{
			debugmsg("Testing deletation of records");
			obj = db_createobject("person");
			db_setvalue(obj, "id", NULL, "13");
			db_setvalue(obj, "lastname", NULL, "hillto be delete");
			db_setvalue(obj, "firstname", NULL, "dj dru");
			db_setvalue(obj, "email", NULL, "andru@treshna.com");
			db_freeobject(obj);
			debugmsg("Now going to load and then delete the record");
			obj = db_createobject("person");
			db_getobjectbyfield(obj, "id", "13");
			db_deleteobject(obj);
			db_clearobject(obj);
			debugmsg("Checking to see if its properly been deleted");
			db_getobjectbyfield(obj, "id", "13");
			if (obj->num == 0)
				debugmsg("TEST PASSED");
			else
				errormsg("TEST FAILED, record not deleted, %d items", obj->num); 
			db_freeobject(obj);

			break;
			}
	case 14:
			{
			gchar *tmpstr2;
			Object *objrank;

			debugmsg("Testing object by object loadings");
			obj = db_createobject("soldier");
			objrank = db_createobject("rank");
			db_getobjectbyfield(obj, "id", "3");
			retval = db_loadobjectbyobject(obj, objrank, "rank");
			debugmsg("Retval from load by object %d", retval);
			if (retval == 0)
				{
				db_getvalue(objrank, "description", NULL, &retstr);
				db_getvalue(obj, "name", NULL, &tmpstr2);
				}
			else
				errormsg("loadobjects failed");

			debugmsg("%s %s is in the house", retstr, tmpstr2);
			db_freeobject(obj);
			db_freeobject(objrank);

			if (retstr == NULL || tmpstr2 == NULL || strcmp(retstr, "General") != 0 || strcmp(tmpstr2, "Avis") != 0)
				errormsg("TEST FAILED");
			else
				debugmsg("TEST PASSED");
			break;
			}
	case 15:
			{
			Object *objrank;

			debugmsg("Testing add object by another object");
			objrank = db_createobject("rank");
			db_getobjectbyfield(objrank, "id", "3");
			obj = db_createobject("soldier");
			db_addobjecttoobject(obj, objrank);
			db_setvalue(obj, "id", NULL, "6");
			db_setvalue(obj, "name", NULL, "cannon fodder");
			db_setvalue(obj, "countryid", NULL, "1");
			db_setvalue(obj, "dob", NULL, "2000-12-21");
			db_setvalue(obj, "active", NULL, "t");
			db_setvalue(obj, "nihil", NULL, "f");
			db_freeobject(obj);
			obj = db_createobject("soldier");
			debugmsg("Retval from load by object %d", db_loadobjectbyobject(objrank, obj, "soldier"));
			debugmsg("Number of corporals: %d", obj->num);
			if (obj->num < 2)
				{
				errormsg("Too few corporals, I failed to add that new solider cannonfodder");
				break;
				}
			db_getvalue(obj, "name", NULL, &retstr);
			debugmsg("Private: %s", retstr);
			if (strcmp(retstr, "Liam") == 0)
				pass++;
			obj->row++;
			db_getvalue(obj, "name", NULL, &retstr);
			debugmsg("Private: %s", retstr);
			if (strcmp(retstr, "cannon fodder") == 0)
				pass++;

			db_getobjectbyfield(obj, "id", "6");
			db_deleteobject(obj);
			db_freeobject(obj);
			db_freeobject(objrank);
			test_total(pass, 2);
			break;
			}
		/* test dbmapme module */
	case 16:
			{
			debugmsg("Testing dbmapme module");
			tb = db_findtable("soldier");
			retstr = db_mapfieldsource(tb, "description");
			debugmsg("Searching for the reference 'description' returned %s", retstr);

			retstr = db_mapfieldsource(tb, "rank.description");
			debugmsg("Searching for the reference 'rank.description' returned %s", retstr);

			retstr = db_mapfieldsource(tb, "rank_id.description");
			debugmsg("Searching for the reference 'rank_id.description' returned %s", retstr);

			retstr = db_sqlparse_suggesttable("rank.description");
			debugmsg("Suggested table for 'rank.description' is %s", retstr);
			mem_free(retstr);

			retstr = db_sqlparse_suggestfield("rank.description");
			debugmsg("Suggested field for 'rank.description' is %s", retstr);
			mem_free(retstr);

			retstr = db_sqlparse_suggestfield("rankname");
			g_assert(!retstr);

			break;
			}
	case 17:
			{
			Object *objrank;

			debugmsg("Testing dbmapme module, phase 2");
			retstr = NULL;
			obj = db_createobject("soldier");
			db_getobjectbyfield(obj, "id", "3");
			retval = db_createmapobject(obj, "rank");
			debugmsg("Creating map, retval is %d", retval);
			db_getvalue(obj, "name", NULL, &retstr);
			debugmsg("Person loaded at moment is %s", retstr);
			test_result(retstr, "Avis", &pass);
			objrank = db_getmapbyclass(obj, "rank");
			db_getvalue(objrank, "description", NULL, &retstr);
			debugmsg("Rank of person is %s", retstr);
			test_result(retstr, "General", &pass);
			test_total(pass, 2);
			db_freeobject(obj);
			break;
			}
	case 18:
		debugmsg("I AM TIGER!");
			{
			gint vla;

			obj = db_createobject("persontype");
			db_getvalue(obj, "id", NULL, &retstr);
			vla = atoi(retstr);
			if (vla < 1)
				errormsg("TEST FAILED");
			else
				debugmsg("TEST PASSED");
			db_freeobject(obj);
			break;
			}
	case 19:
			{
			gchar *comment = mem_alloc(15);
			gint type;
			gchar *table;
			gchar *field;

			strcpy(comment, "1;liam.name");
			debugmsg("Using string %s\n", comment);
			type = db_casual_type(comment);
			field = db_casual_field(comment);
			table = db_casual_table(comment);
			debugmsg("Got type: %d\nGot field: %s\nGot table: %s\n", type, field, table);
			mem_free(comment);
			mem_free(field);
			mem_free(table);
			break;
			}
		/* Test if default values work */
	case 20:
			{
			obj = db_createobject("soldier");
			db_getobjectbyfield(obj, "id", "3");
			db_add(obj);
			db_setvalue(obj, "id", NULL, "20");
			db_setvalue(obj, "name", NULL, "test20");
			db_setvalue(obj, "rankid", NULL, "3");
			db_setvalue(obj, "countryid", NULL, "1");
			db_setvalue(obj, "active", NULL, "f");

			db_loadall(obj);
			db_movelast(obj);

			db_getvalue(obj, "id", NULL, &retstr);
			test_result(retstr, "20", &pass);
			db_getvalue(obj, "name", NULL, &retstr);
			test_result(retstr, "test20", &pass);

			db_getobjectbyfield(obj, "id", "20");
			db_deleteobject(obj);
			db_freeobject(obj);
			test_total(pass, 2);
			break;
			}
		/* variation of above test */
	case 21:
			{
			obj = db_createobject("soldier");
			db_add(obj);
			db_setvalue(obj, "id", NULL, "21");
			db_setvalue(obj, "name", NULL, "test21");
			db_setvalue(obj, "rankid", NULL, "3");
			db_setvalue(obj, "countryid", NULL, "1");
			db_setvalue(obj, "active", NULL, "f");
			if (obj->num == 1)
				pass++;

			db_loadall(obj);
			db_movelast(obj);

			db_getvalue(obj, "id", NULL, &retstr);
			test_result(retstr, "21", &pass);
			db_getvalue(obj, "name", NULL, &retstr);
			test_result(retstr, "test21", &pass);

			db_getobjectbyfield(obj, "id", "21");
			db_deleteobject(obj);
			if (obj->query)
			     pass++;
			else
			     warningmsg("Lost sql query");
			db_freeobject(obj);
			test_total(pass, 4);
			break;
			}
	case 22:
			{
			Object *obj2;

			obj = db_createobject("soldier");
			db_setvalue(obj, "id", NULL, "32");
			db_setvalue(obj, "name", NULL, "number32");
			db_setvalue(obj, "rankid", NULL, "3");
			db_setvalue(obj, "countryid", NULL, "1");
			db_setvalue(obj, "active", NULL, "f");

			obj2 = db_createobject("soldier");
			db_loadall(obj2);
			db_movelast(obj2);
			db_getvalue(obj2, "id", NULL, &retstr);
			test_result(retstr, "32", &pass);
			db_getvalue(obj2, "name", NULL, &retstr);
			test_result(retstr, "number32", &pass);

			db_getobjectbyfield(obj, "id", "32");
			db_deleteobject(obj);
			db_freeobject(obj);
			test_total(pass, 2);
			break;
			}
		/* Testing extract of cache */
	case 23:
			{
			void **retarr;
			gint col, row;
			GList *flist;
			flist = g_list_append(NULL,"name");
			flist = g_list_append(flist,"soldier");
			flist = g_list_append(flist,"kills");
			flist = g_list_append(flist,"soldier");
			flist = g_list_append(flist,"rankid");
			flist = g_list_append(flist,"soldier");
			obj = db_createobject("soldier");
			db_loadall(obj);
			retarr = db_cache_extract(obj, flist, &row, &col);
			if (row == 4)
				pass++;
			else
				errormsg("Row %d != 4",row);
			if (col == 3)
				pass++;
			else
				errormsg("Col %d != 3",col);
			mem_free(retarr);
			db_freeobject(obj);
			
			test_total(pass, 2);
			break;
			}
	case 24:
			{
			/* check that adding to an object works */
			debugmsg("Testing adding to objects");
			obj = db_createobject("person");
			db_getobjectbyfield(obj, "lastname", "hill");
			/* db_obj_debug(obj); */
			db_add(obj);
			db_setvalue(obj, "id", NULL, "24");
			db_setvalue(obj, "lastname", NULL, "hill");
			db_setvalue(obj, "firstname", NULL, "emperor");
			db_setvalue(obj, "email", NULL, "emperor@treshna.com");

			db_moveto(obj, 0);
			debugmsg("Checking the number of objects loaded. 1st time. %d", obj->num);
			if (db_numrecord(obj) == 5)
				pass++;
			else
				errormsg("Failed to add record, %d num", obj->num);
			db_getvalue(obj, "id", NULL, &retstr);
			debugmsg("Checking id is equal to 5, it is %s", retstr);
			if (strcmp(retstr, "2") == 0)
				pass++;
			else
				errormsg("Failed to query appended object, %s returned instead of 5", retstr);

			debugmsg("Checking the number of objects loaded. 2nd time. %d", obj->num);
			if (obj->num == 5)
				pass++;
			else
				errormsg("Failed to add record, %d num", obj->num);

			db_freeobject(obj);

			obj = db_createobject("person");
			db_getobjectbyfield(obj, "firstname", "emperor");
			db_getvalue(obj, "email", NULL, &retstr);
			debugmsg("Email address is is %s", retstr);
			if (retstr != NULL && strcmp(retstr, "emperor@treshna.com") == 0)
				pass++;
			else
				errormsg("Failed to load record");

			db_deleteobject(obj);
			db_freeobject(obj);
			if (pass == 4)
				debugmsg("TEST PASSED");
			else
				errormsg("TEST FAILED, passed %d out of 4", pass);

			break;
			}
	case 25:
			{
			debugmsg("Testing if default values shouldn't be written");
			obj = db_createobject("persontype");
			db_obj_addwrite(obj, "name", NULL, "bob", FALSE);
			db_freeobject(obj);

			obj = db_createobject("persontype");
			db_loadall(obj);
			if (obj->num != 4)
				errormsg("TEST FAILED");
			else
				debugmsg("TEST PASSED");
			db_freeobject(obj);
			break;
			}
	case 26:
			{
			obj = db_createobject("soldier");
			db_getobjectbyfield(obj, "id", "3");
			db_getvalue(obj, "name", "country", &retstr);
			test_result(retstr, "'New Zealand'", &pass);
			db_getvalue(obj, "name", "soldier", &retstr);
			test_result(retstr, "Avis", &pass);

			test_total(pass, 2);
			db_freeobject(obj);
			break;
			}
	case 27:
			{
			walk = NULL;
			obj = db_createobject("soldier");
			db_loadall(obj);
			/* db_moveto(obj,3); */
			walk = db_filter_addrequestlist(walk, "name", NULL);
			walk = db_filter_addrequestlist(walk, "countryid", NULL);
			db_filter_load(obj, walk);
			db_filter_filterbyvalue(obj, "countryid", "soldier", "1");

			db_getvalue(obj, "name", NULL, &retstr);
			test_result(retstr, "Liam", &pass);
			retstr = mem_strdup_printf("%d", db_numrecord(obj));
			test_result(retstr, "3", &pass);
			mem_free(retstr);

			test_total(pass, 2);
			db_freeobject(obj);
			break;
			}
	case 28:
			{
			walk = NULL;
			obj = db_createobject("soldier");
			db_loadall(obj);
			walk = db_filter_addrequestlist(walk, "name", NULL);
			walk = db_filter_addrequestlist(walk, "countryid", NULL);
			db_filter_load(obj, walk);
			db_filter_filterbyvalue(obj, "countryid", "soldier", "1");
			db_filter_clear(obj);

			db_getvalue(obj, "name", NULL, &retstr);
			test_result(retstr, "Liam", &pass);

			db_filter_filterbyvalue(obj, "countryid", "soldier", "3");
			db_moveto(obj, 0);
			db_getvalue(obj, "name", NULL, &retstr);
			test_result(retstr, "Baz", &pass);
			retstr = mem_strdup_printf("%d", db_numrecord(obj));
			test_result(retstr, "1", &pass);
			mem_free(retstr);
			db_freeobject(obj);

			test_total(pass, 3);
			break;
			}
	case 29:
			{
			GList *filterlist = NULL;
			Object *obj2;

			obj2 = db_createobject("country");
			obj = db_createobject("soldier");
			db_getobjectbyfield(obj2, "id", "3");
			db_loadobjectbyobject(obj2, obj, "soldier");
			db_getvalue(obj, "name", NULL, &retstr);
			test_result(retstr, "Baz", &pass);

			filterlist = db_filter_addrequestlist(filterlist, "name", "soldier");
			db_filter_load(obj, filterlist);

			db_getvalue(obj, "name", NULL, &retstr);
			test_result(retstr, "Baz", &pass);

			test_total(pass, 2);
			db_freeobject(obj);
			db_freeobject(obj2);
			break;
			}
		/* testing db_id_verifysql_other, also tests db_sqlparse_getselectfields kinda */
	case 30:
			{
			DbUniqueId *uid;
			gchar *sql, *retstr;

			uid = (DbUniqueId *) mem_alloc(sizeof(DbUniqueId));
			uid->num = 3;
			uid->pg_oid = -1;
			uid->field = (DbField **) mem_alloc(sizeof(DbField *) * 3);
			uid->field[0] = (DbField *) mem_alloc(sizeof(DbField *));
			uid->field[0]->field = mem_strdup("field1");
			uid->field[1] = (DbField *) mem_alloc(sizeof(DbField *));
			uid->field[1]->field = mem_strdup("field2");
			uid->field[2] = (DbField *) mem_alloc(sizeof(DbField *));
			uid->field[2]->field = mem_strdup("field3");

			sql = mem_strdup("select * from whatever");
			retstr = db_id_verifysql(sql);
			if (strcmp(retstr, "select *,oid from whatever") == 0)
				{
				debugmsg("TEST PASSED");
				}
			else
				{
				errormsg("FAILED");
				}
			debugmsg("For the record the sql string returned was ...");
			debugmsg("...%s", retstr);

			for (i = 0; i < 3; i++)
				{
				mem_free(uid->field[i]->field);
				mem_free(uid->field[i]);
				}

			mem_free(uid->field);
			mem_free(uid);
			mem_free(retstr);
			mem_free(sql);
			break;
			}
	case 31:
			{
			/* check that adding to an object works, an extension from test 24 */
			debugmsg("Testing adding to objects");
			obj = db_createobject("person");
			db_getobjectbyfield(obj, "lastname", "hill");
			db_add(obj);
			db_setvalue(obj, "id", NULL, "31");
			db_setvalue(obj, "lastname", NULL, "hill");
			db_setvalue(obj, "firstname", NULL, "emperor");
			db_setvalue(obj, "email", NULL, "dru@treshna.com");

			db_moveto(obj, 0);
			debugmsg("Checking the number of objects loaded. 1st"
				" time. %d", obj->num);
			if (obj->num == 5)
				pass++;
			else
				errormsg("Failed to add record, %d num", obj->num);
			/* db_debugobject(obj); */

			db_getvalue(obj, "id", NULL, &retstr);
			debugmsg("Checking id is equal to 2 it is %s", retstr);
			test_result(retstr, "2", &pass);

			debugmsg("Checking the number of objects loaded."
				"2nd time. %d", obj->num);
			if (obj->num == 5)
				pass++;
			else
				errormsg("Failed to add record, %d num", obj->num);
			db_moveto(obj, 4);
			db_getvalue(obj, "id", NULL, &retstr);
			test_result(retstr, "31", &pass);

			db_freeobject(obj);

			obj = db_createobject("person");
			db_getobjectbyfield(obj, "firstname", "emperor");
			db_getvalue(obj, "email", NULL, &retstr);
			debugmsg("Email address is is %s", retstr);
			test_result(retstr, "dru@treshna.com", &pass);

			db_deleteobject(obj);
			db_freeobject(obj);
			test_total(pass, 5);
			break;
			}

		/* Testing db_birth_createsql */
	case 32:
			{
			gchar *sql;
			gint i;
			DbBirth *testomatic;

			/* setting up a decent DbBirth to test */
			testomatic = (DbBirth *) mem_alloc(sizeof(DbBirth));
			testomatic->num = 3;

			testomatic->value = (gchar **) mem_alloc(sizeof(gchar *) * 3);
			testomatic->value[0] = mem_strdup("generic1");
			testomatic->value[1] = mem_strdup("generic2");
			testomatic->value[2] = mem_strdup("generic3");

			testomatic->field = (DbField **) mem_alloc(sizeof(DbField *) * 3);
			testomatic->field[0] = (DbField *) mem_alloc(sizeof(DbField));
			testomatic->field[0]->field = mem_strdup("field1");
			testomatic->field[0]->table = mem_strdup("table1");
			testomatic->field[1] = (DbField *) mem_alloc(sizeof(DbField));
			testomatic->field[1]->field = mem_strdup("field2");
			testomatic->field[1]->table = mem_strdup("table1");
			testomatic->field[2] = (DbField *) mem_alloc(sizeof(DbField));
			testomatic->field[2]->field = mem_strdup("field1");
			testomatic->field[2]->table = mem_strdup("table2");

			debugmsg("Testing db_birth_createsql ...now");
			/* now for the test */
			sql = db_birth_createsql(testomatic);
			if (strcmp(sql, "SELECT * FROM table1,table2 WHERE "
			           "table1.field1='generic1' AND table1.field2='generic2' " "AND table2.field1='generic3'") == 0)
				{
				debugmsg("TEST PASSED");
				}
			else
				{
				debugmsg("Failed with compare of \n%s\n%s", sql, "SELECT * FROM table1,table2 WHERE "
				         "table1.field1='generic1' AND table1.field2='generic2' " "AND table2.field1='generic3'");
				errormsg("FAILED");
				}

			/* good job memory you can go now */
			for (i = 0; i < 3; i++)
				{
				mem_free(testomatic->field[i]->field);
				mem_free(testomatic->field[i]->table);
				mem_free(testomatic->field[i]);
				mem_free(testomatic->value[i]);
				}
			mem_free(testomatic->field);
			mem_free(testomatic->value);
			mem_free(testomatic);
			mem_free(sql);

			break;
			}

	case 33:
			{
/*			sql_statement *s;
			char *rebuild;
			while(1)
			     {
				
			s = sql_parse
			    ("select *, wibble, pig.blah, spoon from foo, membership where blah=smu and (name like '%joe' or foo is waa) order by member.oid group by wibble");
			if (!s)
				break;

			sql_destroy(s);
			memsql_display();
			     }
			   */
			message("test disabled");
			break;
			}
	case 34:
			{
			gchar *inputfield, *fieldname, *tablename;
			gint retval;

			inputfield = mem_strdup("*");
			debugmsg("Running test");
			retval = db_suggest_wildcard(inputfield, &fieldname, &tablename);
			if (fieldname == NULL && tablename == NULL && retval == 1)
				debugmsg("TEST 1 PASSED");
			else
				errormsg("TEST 1 FAILED");

			mem_free(inputfield);
			mem_free(fieldname);
			mem_free(tablename);
			inputfield = mem_strdup("tablename.*");
			retval = db_suggest_wildcard(inputfield, &fieldname, &tablename);
			if (fieldname == NULL && (strcmp(tablename, "tablename") == 0) && retval == 1)
				debugmsg("TEST 2 PASSED");
			else
				{
				errormsg("TEST 2 FAILED");
				debugmsg("Tablename = %s, fieldname = %s, retval=%d", tablename, fieldname, retval);
				}

			mem_free(inputfield);
			mem_free(fieldname);
			mem_free(tablename);
			inputfield = mem_strdup("tablename.fieldname");
			retval = db_suggest_wildcard(inputfield, &fieldname, &tablename);
			if ((strcmp(fieldname, "fieldname") == 0) && (strcmp(tablename, "tablename") == 0) && retval == 0)
				debugmsg("TEST 3 PASSED");
			else
				{
				errormsg("TEST 3 FAILED");
				debugmsg("Tablename = %s, fieldname = %s, retval=%d", tablename, fieldname, retval);
				}
			mem_free(inputfield);
			mem_free(fieldname);
			mem_free(tablename);
			break;
			}
	case 35:
			{
			Object *destobj, *srcobj;
			GList *srcobjs;
			
			destobj = db_sql(NULL,"SELECT * FROM soldier, country where soldier.countryid=country.id");
			srcobj = db_sql(NULL,"SELECT * FROM rank where id='2'");
			srcobjs = g_list_prepend(NULL,srcobj);
			srcobj = db_sql(NULL,"SELECT * FROM country");
			srcobjs = g_list_prepend(srcobjs,srcobj);
			
			db_loadobjectbyobjects(destobj, srcobjs);
			if (db_numrecord(destobj) == 1)
				debugmsg("TEST PASSED");
			else
				errormsg("TEST FAILED");
			for (walk=g_list_first(srcobjs);walk!=NULL;walk=walk->next)
				db_freeobject(walk->data);
			g_list_free(srcobjs);
			db_freeobject(destobj);
			
			break;
			}
		/* testing db_loadall */
	case 36:
			{
			Object *test;
			int result;

			debugmsg("Testing loadall");
			test = db_createobject("rank");
			result = db_loadall(test);
			db_freeobject(test);
			break;
			}
	case 37:
			{
			Object *destobj;

			obj = db_createobject("soldier");
			destobj = db_createobject(NULL);
			db_getobjectbyfield(obj, "id", "3");
			db_loadobjectbyobject(obj, destobj, "weapon");
			if (destobj->numfield == 4)
				pass++;
			else
				errormsg("wrong number of fields, %d", destobj->numfield);
			if (destobj->num == 0)
				pass++;
			else
				errormsg("wrong number of rows, %d", destobj->num);
			db_add(destobj);
			if (destobj->num == 1)
				pass++;
			else
				errormsg("wrong number of rows, %d", destobj->num);
			db_getvalue(destobj, "soldierid", NULL, &retstr);

			test_result(retstr, "3", &pass);

			test_total(pass, 4);
			db_freeobject(destobj);
			db_freeobject(obj);
			break;
			}
	case 38:
			{
			/* record additions and saving data back */
			debugmsg("testing db write to for a new record");
			obj = db_createobject("person");
			db_setvalue(obj, "id", NULL, "24");
			db_setvalue(obj, "lastname", NULL, "hill");
			db_setvalue(obj, "firstname", NULL, "dru");
			db_setvalue(obj, "email", NULL, "dru@treshna.com");
			db_add(obj);
			db_setvalue(obj, "id", NULL, "25");
			db_setvalue(obj, "lastname", NULL, "hill2");
			db_setvalue(obj, "firstname", NULL, "dru2");
			db_setvalue(obj, "email", NULL, "dru@treshna.com2");
			db_add(obj);
			db_freeobject(obj);

			obj = db_createobject("person");
			db_getobjectbyfield(obj, "id", "24");
			db_getvalue(obj, "email", NULL, &retstr);
			test_result(retstr, "dru@treshna.com", &pass);
			db_deleteobject(obj);
			db_getobjectbyfield(obj, "id", "25");
			db_getvalue(obj, "email", NULL, &retstr);
			test_result(retstr, "dru@treshna.com2", &pass);
			db_deleteobject(obj);
			db_freeobject(obj);

			test_total(pass, 2);
			break;
			}
	case 39:
			{
			/* test loadobjectbyobject in a more complect way. */
			Object *obj2;

			debugmsg("Testing object by object loadings");
			obj = db_createobject("soldier");
			db_add(obj);
			debugmsg("Beginning ----- ");
			db_setvalue(obj, "id", NULL, "24");
			db_setvalue(obj, "rankid", NULL, "3");
			db_setvalue(obj, "countryid", NULL, "1");
			db_setvalue(obj, "name", NULL, "norty dru");
			db_setvalue(obj, "active", NULL, "t");
			db_setvalue(obj, "location", NULL, "Bathroom");
			debugmsg("Created soldier ----- ");

			obj2 = db_createobject("weapon");
			retval = db_loadobjectbyobject(obj, obj2, "weapon");
			debugmsg("Starting write weapon ----- ");
			db_add(obj2);
			db_setvalue(obj2, "id", NULL, "24");
			db_setvalue(obj2, "name", NULL, "AK48");
			db_setvalue(obj2, "damage", NULL, "102");
			debugmsg("Testing results ----- ");

			db_getvalue(obj2, "name", NULL, &retstr);
			test_result(retstr, "AK48", &pass);

			db_freeobject(obj2);
			db_freeobject(obj);

			obj = db_createobject("soldier");
			db_getobjectbyfield(obj, "id", "24");
			db_deleteobject(obj);
			db_freeobject(obj);
			test_total(pass, 1);
			break;
			}
	case 40:
			{
			Object *destobj, *srcobj;
			GList *srcobjs;
			
			destobj = db_sql(NULL,"SELECT * FROM rank");
			
			srcobj = db_sql(NULL,"SELECT * FROM soldier, country where soldier.countryid=country.id ORDER BY soldier.id");
			db_add(srcobj);
			db_setvalue(srcobj,"active","soldier","t");
			db_setvalue(srcobj,"name","soldier","testone");
			db_setvalue(srcobj,"id","soldier","33");
			db_setvalue(srcobj,"countryid","soldier","1");
			db_setvalue(srcobj,"rankid","soldier","2");
			db_save(srcobj);

			db_moveto(srcobj,0);
			db_getvalue(srcobj,"name","soldier",&retstr);
			test_result(retstr, "Andru", &pass);
			db_freeobject(srcobj);
			srcobj = db_sql(NULL,"SELECT * FROM soldier,country WHERE soldier.countryid=country.id ORDER BY soldier.id DESC");
			db_moveto(srcobj,0);
			db_getvalue(srcobj,"id","soldier",&retstr);
			test_result(retstr, "33", &pass);
			db_deleteobject(srcobj);
			db_obj_debug(srcobj);
			
			/* db_moveto(srcobj,2); */
			db_getvalue(srcobj,"id","soldier",&retstr);
			test_result(retstr, "4", &pass);
		db_getvalue(srcobj,"name","soldier",&retstr);
			test_result(retstr, "Baz", &pass);
			db_obj_debug(srcobj);
			srcobjs = g_list_prepend(NULL,srcobj);

			db_loadobjectbyobjects(destobj, srcobjs); 
			if (db_numrecord(destobj) == 1)
				debugmsg("TEST PASSED");
			else
				errormsg("TEST FAILED");
			for (walk=g_list_first(srcobjs);walk!=NULL;walk=walk->next)
				db_freeobject(walk->data);
			g_list_free(srcobjs); 
			db_freeobject(destobj);
			test_total(pass, 4);
			break;
			}
	case 41:
			{
			walk = NULL;
			obj = db_createobject("soldier");
			db_loadall(obj);
			walk = db_filter_addrequestlist(walk, "name", NULL);
			walk = db_filter_addrequestlist(walk, "countryid", NULL);
			db_filter_load(obj, walk);
			db_filter_filterbyvalue(obj, "countryid", "soldier", "1");
			db_add(obj);
			/* db_moveto(obj,0); */
			db_getvalue(obj, "description", "rank", &retstr);
			test_result(retstr, "Corporal", &pass);

			mem_free(retstr);
			db_freeobject(obj);

			test_total(pass, 1);
			break;
			}

	case 42:
			{

			DbMethodDef *md;
			DbParseResult res;

			md = NULL;
			res = db_method_parse(&md, "calcTotal(soldier.rankid, 'hello home', '12')");
			if (res.ok)
				{
				puts(db_method_to_string(md));
				db_method_cleanup(md);
				}
			else
				{
				printf("Error: %s\n", res.errormsg);
				}

			md = NULL;
			res = db_method_parse(&md, "any: calcTotal(soldier.rankid, 'hello\\'s home', '12' )");
			if (res.ok)
				{
				puts(db_method_to_string(md));
				db_method_cleanup(md);
				}
			else
				{
				printf("Error: %s\n", res.errormsg);
				}

			md = NULL;
			res = db_method_parse(&md, "calcTotal(hello.rank, 'hello\\'s \\\"1\\\" home', '12' )");
			if (res.ok)
				{
				puts(db_method_to_string(md));
				db_method_cleanup(md);
				}
			else
				{
				printf("Error: %s\n", res.errormsg);
				}

			md = NULL;
			res = db_method_parse(&md, "calcTotal(soldier.rank, 56, 'hat')");
			if (res.ok)
				{
				puts(db_method_to_string(md));
				db_method_cleanup(md);
				}
			else
				{
				printf("Error: %s\n", res.errormsg);
				}

			md = NULL;
			res = db_method_parse(&md, "calcTotal(soldier.rank, q'hello', '12')");
			if (res.ok)
				{
				puts(db_method_to_string(md));
				db_method_cleanup(md);
				}
			else
				{
				printf("Error: %s\n", res.errormsg);
				}

			md = NULL;
			res = db_method_parse(&md, "calcTotal soldier.rank, q'hello', '12'");
			if (res.ok)
				{
				puts(db_method_to_string(md));
				db_method_cleanup(md);
				}
			else
				{
				printf("Error: %s\n", res.errormsg);
				}

			}
		break;

	case 43:
			{
			int i, j;
			GList *wk;

			for (i = 0; i < db->numtable; i++)
				{
				for (j = 0; j < db->table[i]->num; j++)
					{
					for (wk = g_list_first(db->table[i]->field[j]->comment); wk; wk = g_list_next(wk))
						{
						printf("comments: %s\n", (gchar *) wk->data);
						}
					}
				}
			}
		break;
	       /* Test deletion */
	case 44:
		 obj = db_sql(NULL,"SELECT * FROM person");
		 if (db_numrows(obj) == 5)
		     pass++;
		 db_add(obj);
		 if (db_numrows(obj) == 6)
		     pass++;
		 db_deleteobject(obj);
		 if (db_numrows(obj) == 5)
		     pass++;
		 
		 db_add(obj);
		 db_setvalue(obj,"id","person","18");
		 db_setvalue(obj,"email","person","t@t.com");
		 db_getvalue(obj,"email","person",&retstr);
		 if (db_numrows(obj) == 6)
		     pass++;
		 db_deleteobject(obj);
		 
		 if (db_numrows(obj) == 5)
		     pass++;

		 test_total(pass, 5);
		   
		 db_freeobject(obj);  
		 break;
	case 45:
		 obj = db_sql(NULL,"SELECT * FROM person WHERE id=2");
		 db_moveto(obj,0);
		 message("%d results",db_numrecord(obj));
		 db_setvalue(obj,"firstname","person","");
		 db_freeobject(obj);

		 obj = db_sql(NULL,"SELECT * FROM person WHERE id=2");
		 db_getvalue(obj,"firstname","person",&retstr);
  		test_result(retstr, "", &pass);
		   
		 db_setvalue(obj,"firstname","person","andru");
		 db_freeobject(obj);
		 test_total(pass, 1);
		   break;
	case 46:
			{
			 /* Figuring out who the master of a table is */
			obj = db_sql(NULL,"SELECT * FROM soldier, country where "
				"soldier.countryid=country.id");
			db_masteroftable_test(obj);
			db_freeobject(obj);
			
			obj = db_sql(NULL,"SELECT * FROM soldier, weapon, rank, country where "
				"soldier.countryid=country.id AND "
				"soldier.id=weapon.soldierid AND "
				"rankid = rank.id");
			db_masteroftable_test(obj);
			db_freeobject(obj);
			/*obj = db_sql(NULL,"SELECT * FROM person WHERE id=2");
			debugmsg("testing db master field handling");
			obj = db_sql(NULL,"SELECT * FROM person WHERE id=2");
			db_setvalue(obj, "id", NULL, "24");
			db_setvalue(obj, "lastname", NULL, "hill");
			db_setvalue(obj, "firstname", NULL, "dru");
			db_setvalue(obj, "email", NULL, "dru@treshna.com");
			db_add(obj);
			db_setvalue(obj, "id", NULL, "25");
			db_setvalue(obj, "lastname", NULL, "hill2");
			db_setvalue(obj, "firstname", NULL, "dru2");
			db_setvalue(obj, "email", NULL, "dru@treshna.com2");
			db_add(obj);
			db_freeobject(obj);

			obj = db_createobject("person");
			db_getobjectbyfield(obj, "id", "24");
			db_getvalue(obj, "email", NULL, &retstr);
			test_result(retstr, "dru@treshna.com", &pass);
			db_deleteobject(obj);
			db_getobjectbyfield(obj, "id", "25");
			db_getvalue(obj, "email", NULL, &retstr);
			test_result(retstr, "dru@treshna.com2", &pass);
			db_deleteobject(obj);
			db_freeobject(obj);

			test_total(pass, 2); */
			break;
			}
		 
	default:
			{
			debugmsg("Test hasn't been written yet");
			}
		}
	
	db_cleanup();
	bc_cleanup();
	   
	/* g_mem_profile(); */
	return 0;
	}
