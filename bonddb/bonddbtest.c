
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "bonddbobj.h"
#include "bonddbinit.h"
#include "bonddbglobal.h"
#include "bonddbwrite.h"
#include "bonddbloadby.h"
#include "bonddbarray.h"
#include "bonddbpurify.h"
#include "bonddbfilter.h"
#include "sql_parser.h"
#include "bc.h"

#define NUMTEST 70

static gint
bonddb_runtest(BonddbGlobal * bonddb, gint testnum)
	{
	gchar *value, *errormsg, *sql, *tstr;
	gint pass = 0, i;
	gint retval = 0;
	GList *list = NULL;
	gchar *errmsg;
	BonddbClass *obj, *obj2;

	switch (testnum)
		{
	case 0:
		debug_output("Dummy test.\n");
		break;
	case 1:
		test_announce(testnum, "BonddbClass new and free");
		obj = bonddb_new(bonddb);
		bonddb_free(obj);
		obj = bonddb_new_basetable(bonddb, "person", &errmsg);
		bonddb_free(obj);
		retval = test_total(0, 0);
		break;
	case 2:
		test_announce(testnum, "BonddbClass sql run now");
		obj = bonddb_new_basetable(bonddb, "person", &errmsg);
		bonddb_sql_runnow(obj, &errmsg);
		bonddb_free(obj);
		retval = test_total(0, 0);
		break;
	case 3:
		test_announce(testnum, "BonddbClass sql run now raw");
		obj =
		    bonddb_new_sql_withadmin(bonddb, "SELECT count(id) FROM person",
		                             FALSE, FALSE, &errmsg);
		bonddb_sql_runnow(obj, &errmsg);
		if (obj->res != NULL)
			pass++;
		bonddb_free(obj);
		obj =
		    bonddb_new_sql_withadmin(bonddb, "SELECT date(now())", TRUE,
		                             FALSE, &errmsg);
		bonddb_free(obj);
		retval = test_total(pass, 1);
		break;
	case 4:
		test_announce(testnum, "BonddbClass sql numrecords()");
		obj =
		    bonddb_new_sql_withadmin(bonddb, "SELECT date(now())", TRUE,
		                             FALSE, &errmsg);
		test_result_int(bonddb_numrecord(obj), 1, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 1);
		break;
	case 5:
		test_announce(testnum, "BonddbClass state logic");

		obj = bonddb_new(bonddb);
		if (test_result_int(obj->state, BONDDB_INVALID, &pass))
			debug_output("Failed on BONDDB_INVALID state\n");
		bonddb_free(obj);

		debugmsg("step 1\n");
		obj = bonddb_new_basetable(bonddb, "person", &errmsg);
		if (obj->res == NULL)
			pass++;
		if (test_result_int(obj->state, BONDDB_NEW, &pass))
			debug_output("Failed on BONDDB_NEW state\n");
		bonddb_free(obj);

		debugmsg("step 2\n");
		obj =
		    bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person", TRUE,
		                             FALSE, &errmsg);
		if (test_result_int(obj->state, BONDDB_READ_RAW, &pass))
			debug_output("Failed on BONDDB_READ_RAW state\n");
		bonddb_free(obj);

		debugmsg("step 3\n");
		obj = bonddb_new_sql_withadmin(bonddb, "UPDATE person SET "
		                               "firstname='bobsmity' WHERE id=23",
		                               TRUE, FALSE, &errmsg);
		if (test_result_int(obj->state, BONDDB_RAW, &pass))
			debug_output("Failed on BONDDB_RAW state\n");
		bonddb_free(obj);

		retval = test_total(pass, 5);
		break;
	case 10:
		test_announce(testnum, "Select then set primary key");
		obj =
		    bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person", TRUE,
		                             FALSE, &errmsg);
		list = g_list_append(list, "id");
		test_result_int(bonddb_setprimarykey(obj, "person", list), 0,
		                &pass);
		bonddb_free(obj);
		retval = test_total(pass, 1);
		g_list_free(list);
		break;
	case 11:
		test_announce(testnum, "Select then id_get()");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		list = g_list_append(list, "id");
		test_result_int(bonddb_setprimarykey(obj, "person", list), 0,
		                &pass);
		obj->currentid = bonddb_id_get(obj, "person", 0);
		g_assert(obj->currentid);
		test_result_int(obj->currentid->num, 1, &pass);
		test_result(obj->currentid->primarykeys[0], "2", &pass);
		bonddb_id_free(obj->currentid);
		bonddb_free(obj);
		retval = test_total(pass, 3);
		g_list_free(list);
		break;
	case 12:
		test_announce(testnum, "Select then id_get() then id_find");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		list = g_list_append(list, "id");
		test_result_int(bonddb_setprimarykey(obj, "person", list), 0,
		                &pass);
		obj->currentid = bonddb_id_get(obj, "person", 0);
		g_assert(obj->currentid);
		test_result_int(obj->currentid->num, 1, &pass);
		test_result(obj->currentid->primarykeys[0], "2", &pass);
		test_result_int(bonddb_id_get_row_from_id(obj, obj->currentid,
		                obj->currentid->
		                tablename), 0, &pass);
		bonddb_id_free(obj->currentid);
		bonddb_free(obj);
		retval = test_total(pass, 4);
		g_list_free(list);
		break;
	case 13:
		test_announce(testnum, "jump to");
		obj = bonddb_new_sql_withadmin(bonddb,
		                               "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		obj2 = bonddb_new_sql_withadmin(bonddb,
		                                "SELECT * FROM person "
		                                "WHERE id > 2 "
		                                "ORDER BY id", TRUE, FALSE,
		                                &errmsg);
		bonddb_parsesql(obj);
		bonddb_parsesql(obj2);
		bonddb_moveto(obj, 0);
		bonddb_moveto(obj2, 1);
		test_result_int(bonddb_jumpto(obj2, obj), 0, &pass);
		test_result_int(obj->currentrow, 2, &pass);
		retval = test_total(pass, 2);
		bonddb_free(obj);
		bonddb_free(obj2);
		break;
	case 15:
		test_announce(testnum, "new_basetable then add()");
		obj = bonddb_new_basetable(bonddb, "person", &errmsg);
		debugmsg("step 1\n");
		if (bonddb_add(obj, "person") != 0)
			errormsg("bonddb_add() failed.");
		debugmsg("step 2\n");
		test_result_int(bonddb_numrecord(obj), 1, &pass);
		debugmsg("step 3\n");
		bonddb_free(obj);
		retval = test_total(pass, 1);
		break;
	case 16:
		test_announce(testnum, "Select then add()");
		obj =
		    bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person", TRUE,
		                             FALSE, &errmsg);
		if (bonddb_add(obj, "person") != 0)
			errormsg("bonddb_add() failed.");
		test_result_int(bonddb_numrecord(obj), 6, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 1);
		break;
	case 17:
		test_announce(testnum, "Select then call moveto()");
		obj =
		    bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person", TRUE,
		                             FALSE, &errmsg);
		if (bonddb_moveto(obj, 1) != 0)
			errormsg("bonddb_moveto() failed.");
		test_result_int(obj->currentrow, 1, &pass);
		test_result_int(bonddb_numrecord(obj), 5, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 2);
		break;
	case 18:
		test_announce(testnum, "Select then add() check mode");
		obj =
		    bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person", TRUE,
		                             FALSE, &errmsg);
		if (bonddb_add(obj, "person") != 0)
			errormsg("bonddb_add() failed.");
		test_result_int(obj->currentrecord->mode, BONDDB_INSERT, &pass);
		bonddb_moveto(obj, 1);
		test_result_int(obj->currentrecord->mode, BONDDB_READ, &pass);
		bonddb_moveto(obj, bonddb_numrecord(obj) - 1);
		test_result_int(obj->currentrecord->mode, BONDDB_INSERT, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 3);
		break;
	case 19:
		/* Temporary test. change as you will */
		test_announce(testnum, "new_basetable() then loadall()");
		obj = bonddb_new_basetable(bonddb, "person", &errmsg);
		bonddb_loadall(obj, &errmsg);
		test_result_int(bonddb_numrecord(obj), 5, &pass);
		bonddb_loadall(obj, &errmsg);
		test_result_int(bonddb_numrecord(obj), 5, &pass);
		bonddb_free(obj);
		test_total(pass, 2);
		break;
	case 20:
		test_announce(testnum, "Select then getvalue()");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		g_assert(obj->res);
		test_result_int(bonddb_getvalue(obj, "person", "firstname",
		                                &value), 0, &pass);
		test_result(value, "andru", &pass);
		bonddb_moveto(obj, 1);
		test_result_int(bonddb_getvalue(obj, "person", "firstname",
		                                &value), 0, &pass);
		test_result(value, "hill", &pass);

		i = bonddb_field_get_name_by_index(obj, 3, &value, &sql);
		test_result_int(0, i, &pass);
		test_result(value, "businesstitle", &pass);
		test_result(sql, "person", &pass);

		bonddb_free(obj);
		retval = test_total(pass, 7);
		break;
	case 21:
		test_announce(testnum, "Select then setvalue()");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		test_result_int(bonddb_setvalue(obj, "person", "firstname",
		                                "andru2", FALSE), 0, &pass);
		test_result_int(bonddb_getvalue(obj, "person", "firstname",
		                                &value), 0, &pass);
		if (obj->currentid)
			pass++;
		else
			errormsg("obj->currentid is null. This should be set");
		test_result(value, "andru2", &pass);
		bonddb_moveto(obj, 1);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result(value, "hill", &pass);
		bonddb_moveto(obj, 0);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result(value, "andru2", &pass);
		test_result_int(g_list_length(obj->writequeue), 1, &pass);

		bonddb_free(obj);
		retval = test_total(pass, 7);
		break;
	case 22:
		test_announce(testnum, "Select then setvalue() with sanity "
		              "checks");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		test_result_int(bonddb_setvalue(obj, "person", "firstname",
		                                "andru2", FALSE), 0, &pass);
		test_result_int(obj->mode, BONDDB_UPDATE, &pass);
		if (obj->currentid)
			pass++;
		else
			errormsg("obj->currentid is null. This should be set");
		if (obj->currentrecord->id)
			pass++;
		else
			errormsg("obj->currentrecord->id is null.");
		bonddb_moveto(obj, 1);
		bonddb_moveto(obj, 0);
		test_result_int(obj->mode, BONDDB_UPDATE, &pass);
		if (obj->currentid)
			pass++;
		else
			errormsg("obj->currentid is null when returning to "
			         "a record. This should be set");
		bonddb_free(obj);
		retval = test_total(pass, 6);
		break;
	case 23:
		test_announce(testnum, "Select then setvalue() then save() ");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		test_result_int(bonddb_setvalue(obj, "person", "firstname",
		                                "andru2", TRUE), 0, &pass);
		test_result_int(bonddb_save(obj, &errormsg), 0, &pass);
		if (errormsg)
			errormsg(errormsg);
		bonddb_free(obj);
		obj =
		    bonddb_new_sql_withadmin(bonddb,
		                             "SELECT * FROM person WHERE id=2", TRUE,
		                             FALSE, &errmsg);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result(value, "andru2", &pass);
		bonddb_setvalue(obj, "person", "firstname", "andru", TRUE);
		bonddb_save(obj, &errormsg);
		bonddb_free(obj);
		retval = test_total(pass, 3);
		break;
	case 24:
		test_announce(testnum, "new_basetable then add() then save()");
		obj = bonddb_new_basetable(bonddb, "person", &errmsg);
		bonddb_setautosave(obj, TRUE);
		if (bonddb_add(obj, "person") != 0)
			errormsg("bonddb_add() failed.");
		test_result_int(bonddb_setvalue(obj, "person", "firstname",
		                                "george", TRUE), 0, &pass);
		test_result_int(bonddb_setvalue(obj, "person", "lastname",
		                                "sir", TRUE), 0, &pass);
		test_result_int(bonddb_setvalue(obj, "person", "email",
		                                "george@treshna.com", TRUE), 0,
		                &pass);
		test_result_int(bonddb_setvalue(obj, "person", "id", "12", TRUE), 0,
		                &pass);
		bonddb_free(obj);
		obj =
		    bonddb_new_sql_withadmin(bonddb,
		                             "SELECT * FROM person WHERE id=2", TRUE,
		                             FALSE, &errmsg);
		test_result_int(bonddb_numrecord(obj), 1, &pass);
		bonddb_free(obj);
		obj = bonddb_new_sql_withadmin(bonddb,
		                               "DELETE FROM person WHERE id=12",
		                               TRUE, FALSE, &errmsg);
		bonddb_free(obj);
		retval = test_total(pass, 5);
		break;
	case 25:
		test_announce(testnum, "Select then add() save() and moveto()");
		obj = bonddb_new_sql(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, &errmsg);
		test_result_int(bonddb_numrecord(obj), 5, &pass);
		bonddb_add(obj, "person");
		bonddb_setvalue(obj, "person", "firstname", "george", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "sir", TRUE);
		bonddb_setvalue(obj, "person", "id", "12", TRUE);
		bonddb_moveto(obj, 0);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result_int(bonddb_numrecord(obj), 6, &pass);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		test_result(value, "andru", &pass);
		bonddb_moveto(obj, 5);
		test_result_int(bonddb_numrecord(obj), 6, &pass);
		bonddb_write_delete(obj, "person", FALSE);
		bonddb_save(obj, &errormsg);
		bonddb_free(obj);
		retval = test_total(pass, 4);
		break;
	case 26:
		test_announce(testnum, "Select then save() and moveto() ");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		bonddb_add(obj, "person");
		bonddb_setvalue(obj, "person", "firstname", "george", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "sir", TRUE);
		bonddb_setvalue(obj, "person", "id", "12", TRUE);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result(value, "george", &pass);
		bonddb_setvalue(obj, "person", "lastname", "lord", TRUE);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		bonddb_write_delete(obj, "person", FALSE);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		bonddb_free(obj);
		retval = test_total(pass, 1);
		break;
	case 27:
		test_announce(testnum, "add() then delete() before save()");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		bonddb_add(obj, "person");
		bonddb_setvalue(obj, "person", "firstname", "notgeorge", TRUE);
		bonddb_setvalue(obj, "person", "firstname", "george", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "sir", TRUE);
		bonddb_setvalue(obj, "person", "id", "12", TRUE);
		bonddb_write_delete(obj, "person", FALSE);
		test_result_int(bonddb_numrecord(obj), 5, &pass);
		test_result_int(g_list_length(obj->writequeue), 0, &pass);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		bonddb_free(obj);
		retval = test_total(pass, 2);
		break;
	case 28:
		test_announce(testnum, "add() then delete() before save() with '");
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		bonddb_add(obj, "person");
		bonddb_setvalue(obj, "person", "firstname", "O'Riely", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "O'N'\\'\\i'e'''l",
		                TRUE);
		bonddb_setvalue(obj, "person", "id", "12", TRUE);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		test_result_int(bonddb_numrecord(obj), 6, &pass);
		bonddb_write_delete(obj, "person", FALSE);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		bonddb_free(obj);
		retval = test_total(pass, 1);
		break;
	case 29:
		test_announce(testnum,
		              "add() then delete() before save() then getvalue()");
		obj = bonddb_new_sql(bonddb,
		     "SELECT * FROM person ORDER " "BY id", TRUE, &errmsg);
		bonddb_add(obj, "person");
		bonddb_setvalue(obj, "person", "firstname", "O'Riely", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "O'N'\\'\\i'e'''l",
		                TRUE);
		bonddb_setvalue(obj, "person", "id", "3", TRUE);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		bonddb_free(obj);
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		bonddb_moveto(obj, 1);
		test_result_int(bonddb_numrecord(obj), 6, &pass);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result("O'Riely", value, &pass);
		if (pass <= 1)
			break;
		bonddb_getvalue(obj, "person", "lastname", &value);
		test_result("O'N'\\'\\i'e'''l", value, &pass);
		if (pass <= 2)
			break;

		bonddb_write_delete(obj, "person", FALSE);
		bonddb_save(obj, &errormsg);
		if (errormsg)
			errormsg(errormsg);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result("hill", value, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 4);
		break;
	case 30:
		test_announce(testnum, "defaultvalue on backend");
		value = bonddb->l->_db_defaultvalue(bonddb->l,
		                                    bonddb->conn,
		                                    bonddb->conn_remote_write,
		                                    "soldier", "dob");
		debug_output("%s\n", value);
		mem_free(value);
		value = bonddb->l->_db_defaultvalue(bonddb->l,
		                                    bonddb->conn,
		                                    bonddb->conn_remote_write,
		                                    "soldier", "dob");
		debug_output("%s\n", value);
		mem_free(value);
		value = bonddb->l->_db_defaultvalue(bonddb->l,
		                                    bonddb->conn,
		                                    bonddb->conn_remote_write,
		                                    "soldier", "active");
		debug_output("%s\n", value);
		test_result(value, "false", &pass);
		mem_free(value);
		retval = test_total(pass, 1);
		break;
	case 31:
		test_announce(testnum, "add() with defaults");
		obj = bonddb_new_basetable(bonddb, "soldier", &errmsg);
		bonddb_add(obj, "soldier");
		bonddb_setvalue(obj, "soldier", "name", "george", TRUE);
		bonddb_setvalue(obj, "soldier", "rankid", "2", TRUE);
		bonddb_setvalue(obj, "soldier", "countryid", "1", TRUE);
		bonddb_setvalue(obj, "soldier", "id", "12", TRUE);
		bonddb_save(obj, &errormsg);
		test_result_int(bonddb_numrecord(obj), 1, &pass);
		bonddb_free(obj);
		sql = mem_strdup_printf("DELETE FROM soldier WHERE "
		                        "name='george'");
		obj = bonddb_new_sql_withadmin(bonddb, sql, TRUE, FALSE, &errmsg);
		bonddb_free(obj);
		mem_free(sql);
		retval = test_total(pass, 1);
		break;
	case 32:
		test_announce(testnum, "add() check mode then moveto()");
		obj = bonddb_new_basetable(bonddb, "person", &errmsg);
		if (bonddb_add(obj, "person") != 0)
			errormsg("bonddb_add() failed.");
		test_result_int(obj->currentrecord->mode, BONDDB_INSERT, &pass);
		bonddb_moveto(obj, 0);
		bonddb_setvalue(obj, "person", "firstname", "notgeorge", TRUE);
		bonddb_setvalue(obj, "person", "firstname", "george", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "sir", TRUE);
		bonddb_setvalue(obj, "person", "id", "12", TRUE);
		g_assert(obj->currentid == NULL);
		bonddb_save(obj, &errormsg);
		/* Currentid should be set after a record is written */
		g_assert(obj->currentid != NULL);
		bonddb_moveto(obj, 0);
		g_assert(obj->currentid != NULL);
		bonddb_delete(obj, "person", FALSE);
		bonddb_save(obj, &errormsg);
		test_result_int(bonddb_numrecord(obj), 0, &pass);
		test_result_int(g_list_length(obj->writequeue), 0, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 3);
		break;
	case 34:
		{
		/* Extracting array of items */
		gchar **retarr;
		gint numcol, numrow;
		gchar testt[] = "person";
		gchar test1[] = "firstname";
		gchar test2[] = "id";
		gchar test3[] = "lastname";

		test_announce(testnum, "bonddb_array_extract() test");
		list = g_list_append(list, testt);
		list = g_list_append(list, test1);
		list = g_list_append(list, testt);
		list = g_list_append(list, test2);
		list = g_list_append(list, testt);
		list = g_list_append(list, test3);
		obj =
		    bonddb_new_sql_withadmin(bonddb,
		                             "SELECT * FROM person order by id",
		                             TRUE, FALSE, &errmsg);
		retarr =
		    bonddb_array_extract(obj, list, &numrow, &numcol, FALSE);
		test_result(retarr[0], "andru", &pass);
		test_result_int(numcol, 3, &pass);
		test_result_int(numrow, 5, &pass);
		mem_free(retarr);
		g_list_free(list);
		bonddb_free(obj);
		retval = test_total(pass, 3);
		}
	break;
	case 35:								 /* These tests all related to loadby
																  */
	case 36:								 /* These tests all related to loadby
																  */
	case 37:								 /* These tests all related to loadby
																  */
	case 38:								 /* These tests all related to loadby
																  */
	case 39:								 /* These tests all related to loadby
																  */
	case 40:								 /* These tests all related to loadby
																  */
	case 41:								 /* These tests all related to loadby
																  */
	case 42:								 /* These tests all related to loadby
																  */
	case 43:								 /* These tests all related to loadby
																  */
		retval = bonddb_ref_test(bonddb, testnum);
		break;
	case 45:
		test_announce(testnum, "bonddb_delete() test");
		obj = bonddb_new_sql(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, &errmsg);
		bonddb_add(obj, "person");
		bonddb_setvalue(obj, "person", "firstname", "notgeorge", TRUE);
		bonddb_setvalue(obj, "person", "firstname", "george", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "sir", TRUE);
		bonddb_setvalue(obj, "person", "id", "12", TRUE);
		bonddb_save(obj, &errormsg);
		bonddb_delete(obj, "person", FALSE);
		test_result_int(bonddb_numrecord(obj), 5, &pass);
		test_result_int(g_list_length(obj->writequeue), 0, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 2);
		break;
	case 46:
		test_announce(testnum, "bonddb_delete() test intense");
		obj = bonddb_new_sql(bonddb, "SELECT * FROM person ORDER "
		                               "BY id::integer", TRUE, &errmsg);
		bonddb_add(obj, "person");
		bonddb_setvalue(obj, "person", "firstname", "notgeorge", TRUE);
		bonddb_setvalue(obj, "person", "firstname", "george", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "sir", TRUE);
		bonddb_setvalue(obj, "person", "id", "12", TRUE);
		bonddb_save(obj, &errormsg);
		bonddb_add(obj, "person");
		bonddb_setvalue(obj, "person", "firstname", "notgeorge2", TRUE);
		bonddb_setvalue(obj, "person", "firstname", "george2", TRUE);
		bonddb_setvalue(obj, "person", "lastname", "sir", TRUE);
		bonddb_setvalue(obj, "person", "id", "13", TRUE);
		bonddb_save(obj, &errormsg);
		test_result_int(bonddb_numrecord(obj), 7, &pass);
		bonddb_free(obj);
		obj = bonddb_new_sql(bonddb, "SELECT * FROM person ORDER "
		                               "BY id::integer", TRUE, &errmsg);
		test_result_int(bonddb_numrecord(obj), 7, &pass);
		/* Dummy moves similiar to moving records in bond */
		bonddb_moveto(obj, 5);
		bonddb_getvalue(obj, "person", "id", &value);
		bonddb_getvalue(obj, "person", "firstname", &value);
		bonddb_moveto(obj, 6);
		bonddb_getvalue(obj, "person", "id", &value);
		bonddb_getvalue(obj, "person", "firstname", &value);

		bonddb_moveto(obj, 5);
		bonddb_delete(obj, "person", FALSE);
		bonddb_save(obj, &errormsg);

		bonddb_getvalue(obj, "person", "id", &value);
		test_result(value, "13", &pass);

		bonddb_delete(obj, "person", FALSE);
		bonddb_save(obj, &errormsg);

		bonddb_getvalue(obj, "person", "id", &value);
		test_result(value, "7", &pass);

		test_result_int(bonddb_numrecord(obj), 5, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 5);
		break;
	case 50:								 /* Tests for filtering records */
	case 51:								 /* Tests for filtering records */
	case 52:								 /* Tests for filtering records */
	case 53:								 /* Tests for filtering records */
		retval = bonddb_filter_test(bonddb, testnum);
		break;
	case 54:
		test_announce(testnum, "setvalue() with apostrophes");
		tstr = "fred '' \"\" \\\\ \\ ' \" `` ` ";
		obj = bonddb_new_sql_withadmin(bonddb, "SELECT * FROM person ORDER "
		                               "BY id", TRUE, FALSE, &errmsg);
		test_result_int(bonddb_setvalue(obj, "person", "firstname",
		                                tstr, FALSE), 0, &pass);
		test_result_int(bonddb_getvalue(obj, "person", "firstname",
		                                &value), 0, &pass);
		if (obj->currentid)
			pass++;
		else
			errormsg("obj->currentid is null. This should be set");
		test_result(value, tstr, &pass);
		bonddb_moveto(obj, 1);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result(value, "hill", &pass);
		bonddb_moveto(obj, 0);
		bonddb_getvalue(obj, "person", "firstname", &value);
		test_result(value, tstr, &pass);
		test_result_int(g_list_length(obj->writequeue), 1, &pass);
		bonddb_free(obj);
		retval = test_total(pass, 7);
		break;
	default:
		break;
		}
	/* debugmsg("returning %d",retval); */
	return retval;
	}

int
main(int argc, char *argv[])
	{
	gint retval = 0, test, result, runall = 0;
	BonddbGlobal *bonddb;

	if (argc <= 1)
		/* run your a specific test here on default */
		test = 0;
	else
		test = atoi(argv[1]);
	bc_init(argc, argv);
	bonddb = bonddb_init(NULL, "pgsql", NULL);
	if (bonddb == NULL)
		{
		errormsg("Failed to connect to the database");
		return -1;
		}
	if (test == 0)
		{
		runall = 1;
		for (test = 1; test < NUMTEST; test = test + 1)
			{
			result = bonddb_runtest(bonddb, test);
			if (result)
				{
				debug_output("Test %d returned %d\n", test, result);
				retval = retval - result;
				}
			}
		}
	else
		retval = bonddb_runtest(bonddb, test);
	if (runall == 1)
		{
		if (retval)
			debug_output("Test returned %d\n", retval);
		else
			debug_output("ALL TESTS PASSED SUCCESSFULLY\n");
		}
	bonddb_cleanup(bonddb);
	mem_display();
	bc_cleanup();

	return retval;
	}
