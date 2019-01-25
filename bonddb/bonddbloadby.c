#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bonddbobj.h"
#include "bonddbloadby.h"
#include "bonddbfield.h"
#include "sql_parser.h"
#include "bc.h"

/* Load by is when you create forign key relationships to specify
 * how sql statements are joined. They use where not joins though */

/* returns true if @tablename is in @obj table list */
static gboolean
bonddb_ref_verifytable(BonddbClass *obj, gchar *tablename)
{
	GList *walk;
	if (obj->tablelist == NULL)
	{
		/* debugmsg("No table list in %s, looking for %s",
				obj->query, tablename); */
		return TRUE;
	}
	for (walk=g_list_first(obj->tablelist);walk!=NULL;walk=walk->next)
		if (strcmp(walk->data,tablename) == 0)
			return TRUE;
	
	/* debugmsg("failed on false, can't "
	 "find %s in %s",tablename,obj->query); */
	return FALSE; 
}
	
static void 
bonddb_ref_debugref(BonddbReference *r)
{
	debug_output("\tFrom: %s.%s\n\tTo: %s.%s\n",
			r->fromtable,r->fromfield,r->totable,r->tofield);
}


static BonddbReference *
bonddb_ref_find(BonddbClass *destobj, BonddbClass *srcobj, 
		gchar *desttablename)
{
	BonddbReference *r, *rval = NULL;
	GList *walk;
	gint bestmatch = 9999;

	g_assert(srcobj);
	g_assert(destobj);
	/* debugmsg("%d items in src, %d in dest %s",
	 	g_list_length(srcobj->reflist), 
		g_list_length(destobj->reflist),desttablename); */
	/* Try and guess relationships if needed to */
	if (srcobj->reflist == NULL && destobj->reflist == NULL)
	{
		debugmsg("No relationship between objects found for loading"
			" %s. Attempting auto detection",desttablename);
		bonddb_ref_autoadd(srcobj);
		bonddb_ref_autoadd(destobj);
		if (srcobj->reflist == NULL && destobj->reflist == NULL)
			warningmsg("reflist is still null");
	}
	for (walk=g_list_first(srcobj->reflist);walk!=NULL;walk=walk->next)
	{
		r = walk->data;
		/* This allows debuging information of reference */
		/* bonddb_ref_debugref(r); */
		/* This is a normal one to many relationship, one record
		 * in src corresponds to many records in dest */
		if (strcmp(r->totable,desttablename) == 0)
		{
			if (bonddb_ref_verifytable(srcobj, r->fromtable) == 
					FALSE)
				continue;
			/* m.essage("Match found on %s.%s => %s.%s", 
				r->fromtable,r->fromfield,
				r->totable,r->tofield); */
			r->onetomany = TRUE;
			if (r->tocount < bestmatch && r->tocount != -1)
				{
				bestmatch = r->tocount;
				rval = r;
				}
			if (r->fromcount < bestmatch && r->fromcount != -1)
				{
				bestmatch = r->fromcount;
				rval = r;
				}
			if (bestmatch == 9999 && r->fromcount == -1 
				&& r->tocount == -1)
				rval = r;
		}
	}
	for (walk=g_list_first(destobj->reflist);walk!=NULL;walk=walk->next)
	{
		r = walk->data;
		/* debugmsg("Comparing %s to %s\n%s",r->fromtable,desttablename,
				destobj->query);
		bonddb_ref_debugref(r); */
		/* In this case the relationship goes the other way
		 * with a many to one record relationship. Ie one person from
		 * many sales */
		if (strcmp(r->fromtable,desttablename) == 0)
		{
			if (bonddb_ref_verifytable(srcobj, r->totable) == 
					FALSE)
				continue;
			/* m.essage("Match found second on %s.%s => %s.%s",
				r->totable,r->tofield,
				r->fromtable,r->fromfield); */
			r->onetomany = FALSE;
			if (r->tocount < bestmatch && r->tocount != -1)
				{
				bestmatch = r->tocount;
				rval = r;
				}
			if (r->fromcount < bestmatch && r->fromcount != -1)
				{
				bestmatch = r->fromcount;
				rval = r;
				}
			if (bestmatch == 9999 && r->fromcount == -1 
				&& r->tocount == -1)
				rval = r;
				
		}
		/* if (rval == NULL)
			debugmsg("not found second on %s.%s => %s.%s",
				r->totable,r->tofield,
				r->fromtable,r->fromfield); */
	}
	if (rval == NULL)
		debugmsg("Match not found object loadby %s.",
			destobj->firsttable); 
	return rval;
}

/* For when you add new records, need to remeber joining keys */
gint
bonddb_ref_addbirth(BonddbClass *destobj, gchar *leftfield, gchar *rightvalue)
{
	gchar *_rightvalue;
	if (destobj->birthhash == NULL)
		destobj->birthhash = g_hash_table_new_full(g_str_hash,
			g_str_equal, (GDestroyNotify)mem_free_raw, (GDestroyNotify)mem_free_raw);

	if ((_rightvalue = g_hash_table_lookup(destobj->birthhash,leftfield)))
	{
		debugmsg("Birth hash has already been applied."); 
		if (rightvalue && _rightvalue)
			if (strcmp(rightvalue,_rightvalue)==0)
				return 0;
		g_hash_table_remove(destobj->birthhash, leftfield);
	}
	g_hash_table_insert(destobj->birthhash,mem_strdup(leftfield),
			mem_strdup(rightvalue));
	return 0;
}

/* If onetomany is TRUE then it means its a many to one relationship
 * If onetomany is FALSE then it means that its a one to many relationship.
 *
 */
static gchar *
bonddb_ref_getvalue(BonddbClass *srcobj, BonddbReference *r)
{
	gchar *retstr = NULL;
	
	switch (r->onetomany)
	{
	case TRUE:
		/* debugmsg("%s.%s from srcobj %s", 
		 	r->fromtable,r->fromfield, srcobj->query); */
		bonddb_getvalue(srcobj, r->fromtable, r->fromfield, &retstr);
		if (retstr == NULL)
			errormsg("Unable to get value %s.%s on sql %s",
				r->fromtable,r->fromfield,srcobj->query);
		break;
	case FALSE:
		/* debugmsg("%s.%s from srcobj %s", 
		 	r->totable,r->tofield,srcobj->query); */
		bonddb_getvalue(srcobj, r->totable, r->tofield, &retstr);
		if (retstr == NULL)
			errormsg("Unable to get value %s.%s on sql %s",
					r->totable,r->tofield,srcobj->query);
		break;
	}
	return retstr;
}

static gint
bonddb_ref_get_where(BonddbClass *srcobj, BonddbReference *r, 
		gchar **leftfield, gchar **rightfield)
{
	gint retval = 1;
	gchar *value;

	*leftfield = NULL;
	*rightfield = NULL;

	switch (r->onetomany)
	{
	case FALSE:
		value = bonddb_ref_getvalue(srcobj, r);
		if (!value)
			return -1;			
		*leftfield = mem_strdup_printf("%s.%s",
			r->fromtable,r->fromfield);
		*rightfield = value;
		retval = 0;
		break;
	case TRUE:
		value = bonddb_ref_getvalue(srcobj, r);
		if (!value)
			return -1;			
		*leftfield = mem_strdup_printf("%s.%s",
			r->totable,r->tofield);
		*rightfield = value;
		retval = 0;
		break;
	}
	return retval;
}

static gint
bonddb_ref_loadbyself(BonddbClass *srcobj, gchar *tablename, gchar **retwhere)
{
	BonddbId *id;
	gchar *where;
	id = bonddb_id_get_currentrow(srcobj, tablename);
	*retwhere = NULL;
	if (id == NULL)
	{
		errormsg("Unable to extract id from %s, loadby failed.",
			srcobj->query);
		return -1;
	}
	mem_verify(id);
	where = bonddb_id_get_str(srcobj, id, tablename);
	*retwhere = where;
	return 0;
}

static gboolean
bonddb_ref_checkwhere(sql_statement *s)
{
	if (s->type != SQL_select)
		return FALSE;
	if (((sql_select_statement*)s->statement)->where)
		return TRUE;
	return FALSE;
}

static gint
bonddb_ref_add_loadby(BonddbClass *destobj, BonddbClass *srcobj, 
		gchar *desttablename, gboolean noloadself)
{
	gint retval;
	gchar *leftfield, *rightfield;
	gchar *query, *where;
	BonddbReference *r;
	g_assert(srcobj);
	
	/* debugmsg("looking for match to %s\nbondrel src:%s\n"
		"bondrel dest:%s",desttablename,
		srcobj->query,destobj->query); */
	/* Special case if doing a loadby by yourself */
	if (srcobj->firsttable && noloadself == FALSE &&
		strcmp(srcobj->firsttable, desttablename) == 0)
	{
		debugmsg("loading by self, %s = %s",srcobj->firsttable, 
		 		desttablename); 
		retval = bonddb_ref_loadbyself(srcobj, desttablename, &where);
		if (retval)
			return -1;
		if (bonddb_ref_checkwhere(destobj->statement) == FALSE)
			query = mem_strdup_printf("%s WHERE %s", 
					destobj->query, where);
		else
			query = mem_strdup_printf("%s AND %s", 
					destobj->query, where);
		/* m.essage("query %s",query); */
		mem_free(destobj->query);
		mem_free(where);
		destobj->query = query;
		return 0;
	}
	/* Figure out the constraints and how to modify the sql statement. */
	r = bonddb_ref_find(destobj, srcobj, desttablename);
	if (r == NULL)
		return -1;
	bonddb_ref_debugref(r);

	retval = bonddb_ref_get_where(srcobj, r, &leftfield, 
			&rightfield);
	if (retval)
	{
		/* optional warning , remove me */
		/* warningmsg("get_where failed in bondrel."); */
		return -2;
	}
	/* m.essage("Found %s = %s",leftfield,rightfield); */
	retval = bonddb_ref_addbirth(destobj, leftfield, rightfield);
	if (retval)
	{
		errormsg("bondrel Adding birth values failed.");
		return -3;
	}
	sql_statement_append_where(destobj->statement, 
			leftfield, rightfield, SQL_and, SQL_eq);
 
	/* Random debugging code. Can be removed later on */
/*	 {
	gchar *tmpstr = sql_stringify(destobj->statement);
	debugmsg("Loadby worked\nSql: %s",tmpstr);  
	} */
	mem_free(leftfield);
	/* Flagging generatequery = TRUE will tell bonddb to generate
	 * the sql statement later on when it really needs to execute the 
	 * query. */
	/* m.essage("Succesffuly loaded sql"); */
	destobj->generatequery = TRUE;
	return 0;
}

/**
 * db_loadobjectbyobjects()
 * @param destobj :
 * @param srcobjs :
 *
 * Modify destobj sql statement to be subset of @srcobjs. So a where
 * clause will be added for each srcobjs statement and a birth will be 
 * generated.
 */
gint 
bonddb_loadobjectbyobjects(BonddbClass * destobj, GList * srcobjs, gchar **errmsg)
{
	gint pass = 0;
	GList *walk, *tablewalk;
	gboolean noloadself = FALSE;
	g_assert(destobj);
	
	if (destobj->statement == NULL)
		bonddb_parsesql_proper(destobj, FALSE);
	if (destobj->birthhash)
	{
		g_hash_table_destroy(destobj->birthhash);
		destobj->birthhash = NULL;
	}
	if (srcobjs == NULL)
	{
		errormsg("No src objects for loadobjectbyobjects");
		return -1;
	}

	for (tablewalk=g_list_first(destobj->tablelist);tablewalk!=NULL;
			tablewalk=tablewalk->next)
	{
		for (walk=g_list_first(srcobjs);walk!=NULL;walk=walk->next)
		{
			/* debugmsg("looking for loadby dest %s",
			  tablewalk->data); */
			if (bonddb_ref_add_loadby(destobj, walk->data,
				(gchar*)tablewalk->data,noloadself))
			{
				/* Not really an error. Purposefully empty */
			}
			else 
			{
				pass ++;
				/* This is for complex references to stop
				 * doing loadby yourself */
				if (pass == 1)
					noloadself = TRUE;
			}

		}
	}
	if (pass > 0)
	{
		/* query will not be correct here as it is generated
		 * later on */
		debugmsg("Loadby successfully created");
		return 0;
	}
	else	
	{
		if (!destobj->query)
			return -2;
		debuglog(50,"Loadby failed on %s. Clearing the object.",
			destobj->query);
		bonddb_blank(destobj, errmsg);
		return -2;
	}
}

static BonddbReference *
bonddb_reference_new(gchar *fromtable, gchar *fromfield,
		gchar *totable, gchar *tofield)
{
	BonddbReference *ref;
	ref = mem_calloc(sizeof(BonddbReference));
	ref->fromtable = mem_strdup(fromtable);
	ref->fromfield = mem_strdup(fromfield);
	ref->totable = mem_strdup(totable);
	ref->tofield = mem_strdup(tofield);
	ref->tocount = -1;
	ref->fromcount = -1;
	return ref;
}

/**
 * bonddb_ref_add()
 * @param obj :
 * @param fromtable :
 * @param fromfield :
 * @param totable :
 * @param tofield :
 *
 * Manually adds a reference in @obj to @totable.@tofield referenced
 * from @fromtable.@fromfield.
 */
gint 
bonddb_ref_add(BonddbClass *obj, gchar *fromtable, gchar *fromfield,
		gchar *totable, gchar *tofield)
{
	BonddbField *f;
	BonddbReference *ref;
	g_assert(obj);
	g_assert(fromtable);
	g_assert(fromfield);
	g_assert(totable);
	g_assert(tofield);

	ref = bonddb_reference_new(fromtable,fromfield,totable,tofield);
	if (obj->fieldhash)
	{
		/* debugmsg("finding column references"); */
		f = bonddb_field_get(obj, totable, tofield);
		if (f)
			ref->tocount = f->fieldposinsql;
		f = bonddb_field_get(obj, fromtable, fromfield);
		if (f)
			ref->fromcount = f->fieldposinsql;
	}
	
	obj->reflist = g_list_prepend(obj->reflist,ref);
	return 0;
}

static void 
bonddb_ref_autoadd_copy(BonddbClass *obj, GList *reflist)
{
	GList *walk;
	BonddbReference *r;
	
	for (walk=g_list_first(reflist);walk!=NULL;walk=walk->next)
	{
		r = walk->data;
		bonddb_ref_add(obj, r->fromtable, r->fromfield,
			r->totable, r->tofield);
	}
}

gint
bonddb_ref_autoadd(BonddbClass *obj)
{
	GList *tablewalk, *reflist;
	g_assert(obj);
	
	if (obj->tablelist == NULL)
	{
		if (bonddb_parsesql_proper(obj, FALSE))
		{
			debuglog(40,"No parsing possible, so no automatic "
				"reference adding.");
			return -1;
		}
	}
	for (tablewalk=g_list_first(obj->tablelist);
		tablewalk!=NULL;tablewalk=tablewalk->next)
	{
		reflist = obj->bonddb->l->_db_findreference(obj->bonddb->l,
				obj->conn, tablewalk->data);
		bonddb_ref_autoadd_copy(obj, reflist);
		obj->bonddb->l->_db_freereference(reflist);
	}
	return 0;
}

gint
bonddb_ref_clear(BonddbClass *obj)
{
	BonddbReference *ref;
	GList *walk;
	for (walk=g_list_first(obj->reflist);walk!=NULL;walk=walk->next)
	{
		ref = walk->data;
		mem_free(ref->totable);
		mem_free(ref->tofield);
		mem_free(ref->fromtable);
		mem_free(ref->fromfield);
		mem_free(ref);
	}
	return 0;
}

/* Run tests */
gint
bonddb_ref_test(BonddbGlobal *bonddb, gint testnum)
{
	gint retval = 0;
	gint pass = 0;
	gchar *value;
	gchar *errmsg;
	GList *list;
	BonddbClass *obj, *weapon;

	switch (testnum)
	{
	case 35:
		test_announce(testnum,"loadbyobjects()");
		obj = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier", TRUE, FALSE, &errmsg);
		weapon = bonddb_new_basetable(bonddb,"weapon",&errmsg);
		bonddb_ref_add(obj, "soldier", "id",
			"weapon", "soldierid"); 
		bonddb_ref_add(obj, "soldier", "countryid",
			"country", "id"); 
		bonddb_ref_add(weapon, "weapon", "soldierid",
			"soldier", "id"); 
		list = NULL;
		list = g_list_append(list,obj);
		bonddb_loadobjectbyobjects(weapon, list,&errmsg);
		g_list_free(list);
		bonddb_sql_runnow(weapon, &errmsg);
		test_result(weapon->query,"select * from weapon where "
			"(weapon.soldierid = 2)",&pass);
		test_result_int(bonddb_numrecord(weapon),2,&pass);
		list = NULL;
		bonddb_free(obj);
		bonddb_free(weapon);
		retval = test_total(pass,2);
		break;
	case 36:
		{
		BonddbClass *rank, *country, *soldier;
		
		test_announce(testnum,"loadbyobjects() multiple sources");
		country = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM country", TRUE,FALSE,&errmsg);
		rank = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM rank "
			"WHERE id>= 3 ORDER BY id", TRUE, FALSE, &errmsg);
		soldier = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier, weapon "
			"WHERE soldier.id = weapon.soldierid", FALSE, FALSE, &errmsg);
		bonddb_ref_add(rank, "rank", "rankid",
			"country", "countryid");
		bonddb_ref_add(rank, "rank", "id",
			"soldier", "rankid");
		bonddb_ref_add(country, "country", "id",
			"soldier", "countryid");
		bonddb_ref_add(soldier, "soldier", "rankid",
			"rank", "id");
		list = NULL;
		list = g_list_append(list,rank);
		list = g_list_append(list,country);
		bonddb_loadobjectbyobjects(soldier, list,&errmsg);
		g_list_free(list);
		list = NULL;
		bonddb_sql_runnow(soldier, &errmsg);
		/* m.essage(soldier->query); */
		test_result(soldier->query,"select * from soldier, weapon "
			"where ((soldier.id = weapon.soldierid) and "
			"((soldier.rankid = 3) and (soldier.countryid = 1)))",
			&pass);
		test_result_int(bonddb_numrecord(soldier),2,&pass);
		bonddb_free(soldier);
		bonddb_free(rank);
		bonddb_free(country);
		retval = test_total(pass,2);
		}
		break;	
	case 37:
		/* Finding reference */
		/* This test is broken for libgda due to a gda bug. - also currently broken for windows */
		test_announce(testnum,"find references");
	list = bonddb->l->_db_findreference(bonddb->l,bonddb->conn,
				"soldier");
		test_result_int(g_list_length(list),2,&pass);
		bonddb->l->_db_freereference(list);
		retval = test_total(pass,1);
		break;
	case 38:
		{
		BonddbClass *rank, *country, *soldier;
		test_announce(testnum,"references ref_autoadd() test");
		country = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM country", TRUE, FALSE, &errmsg);
		rank = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM rank "
			"WHERE id>= 3 ORDER BY id", TRUE, FALSE, &errmsg);
		soldier = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier, weapon "
			"WHERE soldier.id = weapon.soldierid", FALSE, FALSE, &errmsg);
		bonddb_ref_autoadd(country);
		bonddb_ref_autoadd(rank);
		bonddb_ref_autoadd(soldier);
		
		list = g_list_append(NULL,rank);
		list = g_list_append(list,country);
		bonddb_loadobjectbyobjects(soldier, list,&errmsg);
		g_list_free(list);
		list = NULL;
		bonddb_sql_runnow(soldier, &errmsg);
		test_result(soldier->query,"select * from soldier, weapon "
			"where ((soldier.id = weapon.soldierid) and "
			"((soldier.rankid = 3) and (soldier.countryid = 1)))",
			&pass);
		test_result_int(bonddb_numrecord(soldier),2,&pass);
		bonddb_free(soldier);
		bonddb_free(rank);
		bonddb_free(country);
		retval = test_total(pass,2);
		break;
		}
	case 39:
		{
		test_announce(testnum,"loadbyobjects() then add()");
		obj = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier", TRUE, FALSE, &errmsg);
		weapon = bonddb_new_basetable(bonddb,"weapon",&errmsg);
		bonddb_ref_add(obj, "soldier", "id",
			"weapon", "soldierid"); 
		bonddb_ref_add(obj, "soldier", "countryid",
			"country", "id"); 
		bonddb_ref_add(weapon, "weapon", "soldierid",
			"soldier", "id"); 
		list = g_list_append(NULL,obj);
		bonddb_loadobjectbyobjects(weapon, list,&errmsg);
		g_list_free(list);
		bonddb_sql_runnow(weapon, &errmsg);
		test_result(weapon->query,"select * from weapon where "
			"(weapon.soldierid = 2)",&pass);
		test_result_int(bonddb_numrecord(weapon),2,&pass);
		bonddb_add(weapon,"weapon");
		bonddb_getvalue(weapon,"weapon","soldierid",&value);
		test_result(value,"2",&pass);
		list = NULL;
		bonddb_free(obj);
		bonddb_free(weapon);
		retval = test_total(pass,3);
		}
		break;
	case 40:
		{
		BonddbClass *soldier;
		test_announce(testnum,"loadbyobjects() many to one");
		obj = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM weapon", TRUE, FALSE, &errmsg);
		soldier = bonddb_new_basetable(bonddb,"soldier",&errmsg);
		bonddb_ref_autoadd(obj);
		bonddb_ref_autoadd(soldier);
		list = g_list_append(NULL,obj);
		bonddb_loadobjectbyobjects(soldier, list,&errmsg);
		g_list_free(list);
		bonddb_sql_runnow(soldier, &errmsg);
		test_result(soldier->query,"select * from soldier where "
			"(soldier.id = 1)",&pass);
		test_result_int(bonddb_numrecord(soldier),1,&pass);
		list = NULL;
		bonddb_free(obj);
		bonddb_free(soldier);
		retval = test_total(pass,2);
		break;
		}
	case 41:
		{
		BonddbClass *soldier;
		test_announce(testnum,"loadbyobjects() from oneself");
		soldier = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier WHERE "
			"rankid > 2", TRUE, FALSE, &errmsg);
		obj = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier",FALSE, FALSE, &errmsg);
		bonddb_ref_autoadd(obj);
		bonddb_ref_autoadd(soldier);
		list = g_list_append(NULL,soldier);
		bonddb_loadobjectbyobjects(obj, list,&errmsg);
		bonddb_sql_runnow(obj, &errmsg);
		test_result_int(bonddb_numrecord(obj),1,&pass);
		bonddb_free(obj);
		bonddb_free(soldier);
		retval = test_total(pass,1);
		break;
		}
	case 42:
		{
		BonddbClass *soldier;
		test_announce(testnum,"loadbyobjects() from oneself twice");
		soldier = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier WHERE "
			"rankid > 2", TRUE, FALSE, &errmsg);
		obj = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier",FALSE, FALSE, &errmsg);
		bonddb_ref_autoadd(obj);
		bonddb_ref_autoadd(soldier);
		list = g_list_append(NULL,soldier);
		bonddb_loadobjectbyobjects(obj, list,&errmsg);
		bonddb_sql_runnow(obj, &errmsg);
		test_result_int(bonddb_numrecord(obj),1,&pass);
		bonddb_clear(obj);
		bonddb_sql(obj,"SELECT * FROM soldier", FALSE, &errmsg);
		bonddb_loadobjectbyobjects(obj, list,&errmsg);
		bonddb_sql_runnow(obj, &errmsg);
		test_result_int(bonddb_numrecord(obj),1,&pass);
		bonddb_free(obj);
		bonddb_free(soldier);
		retval = test_total(pass,2);
		break;
		}
	case 43:
		{
		BonddbClass *soldier;
		test_announce(testnum,"loadbyobjects() multi tables");
		soldier = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM soldier, "
			"country WHERE soldier.countryid = country.id "
			"ORDER BY soldier.id", 	FALSE, FALSE, &errmsg);
		obj = bonddb_new_sql_withadmin(bonddb,"SELECT * FROM rank, "
			"insurance WHERE rank.insuranceid=insurance.id ORDER "
			"BY rank.id", TRUE, FALSE, &errmsg);
		bonddb_moveto(obj,1);
		bonddb_ref_autoadd(obj);
		bonddb_ref_autoadd(soldier);
		list = g_list_append(NULL,obj);
		bonddb_loadobjectbyobjects(soldier, list,&errmsg);
		bonddb_sql_runnow(soldier, &errmsg);
		test_result_int(bonddb_numrecord(soldier),1,&pass);
		bonddb_getvalue(soldier,"soldier","id",&value);
		test_result(value,"3",&pass);
		bonddb_free(obj);
		bonddb_free(soldier);
		retval = test_total(pass,2);
		break;
		}
	default:
		break;
	}
	return retval;
}
