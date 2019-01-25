/*
 * Author: Francis Lee <francis@treshna.com>
 * Date: June 20, 2002
 *
 * "DB comment constraint" looks for a comment field in Postgresql, then
 * check our own specific syntax. Formally, in LEX expression,
 * [01]: FUNCTIONNAME "(" TABLE "." FIELD "," TABLE "." FIELD )"
 * For instance,
 * 0: GetDefaultPrice(product.price, orderitem.finalprice)   or
 * 1: GetDefaultPrice(product.price, orderitem.finalprice)
 *
 * So this was created to overcome BOND's some problems of the database and
 * userinterface interaction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include <glib.h>

/*
 * The list of GLIB use here:
 * TYPE:
 *  gboolean : {TRUE, FALSE}
 *  gchar
 *
 * FUNCTION:
 *  g_list_append
 */



#include "dbwrapper.h"
#include "dbgather.h"
#include "bc.h"
#include "dbconstraint.h"


#define PG_SEPERATOR "\\000"

#include "dbcommentcallback.h"

static DbDatabaseDef *db;


gboolean
check_if_old_syntax(gchar * commentstr);

gboolean
db_comment_callback_build_constraints(DbDatabaseDef *given_db);

static DbMethodDef *
db_comment_callback_build_callback(gchar * commentstr);

/*
* Francis: I need to modify this code.
* This function convert from raw PostgreSQL comment to Bond DbConstraintDef
* data structure.
*
*/

GList *
db_build_old_casual_constraints(GList * constraints, DbDatabaseDef * db)
{

	gint i, j;
	gboolean update;
	DbFieldDef *currfield;
	
	GList *commentwalk;
	
	DbConstraintDef *cons;

	g_assert(db);

	/* For each table... */
	for (i = 0; i < db->numtable; i++)
	{
		/* For each field... */ 
		for (j = 0; j < db->table[i]->num; j++)
		{			
			currfield = db->table[i]->field[j];
			
			/* If comment field is null, then check the next field */
			if (currfield->comment == NULL)
				continue;
			
			/* Francis: HERE COMES THE MODIFICATION
			Just checking the if commentwalk exist is not enough.
			Now I need to change syntax and determin proper data stucture adding
			*/ 
			for (commentwalk = g_list_first(currfield->comment);
				commentwalk != NULL;
				commentwalk = g_list_next(commentwalk) )
			{
				
				/* Francis: This turns out to be just an error flag */
				update = FALSE;
				
				
				/* BEGIN: True modification */

debugmsg("Checking %s\n", (gchar*)commentwalk->data);

				if ( ! check_if_old_syntax((gchar *)commentwalk->data) ) {
					/* Skip */
					continue;
				}
				/* END: True modification */
				
				/* Build constraint */
				cons            = mem_alloc(sizeof(DbConstraintDef));
				cons->table[0]  = mem_strdup(db->table[i]->name);
				cons->column[0] = mem_strdup(currfield->name);

				cons->casual    = db_casual_type(commentwalk->data);
				cons->table[1]  = db_casual_table(commentwalk->data);
				cons->column[1] = db_casual_field(commentwalk->data);

				/* Francis : Hmm, here the sanity was checked. I'm impressed.
				
				This must be trying to say that the two tables in casual
				relationship must have non-NULL value
				*/
				if (! (cons->casual == -1 ||
					cons->table[0] == NULL || cons->column[0] == NULL ||
					cons->table[1] == NULL || cons->column[1] == NULL))
				{
					/* Then, check if a casual exists. If not exist, it must be
					a really just normal comment (i.e. a self note) */
					if ( g_list_find(constraints, commentwalk->data) != NULL )	{
						constraints = g_list_append(constraints, cons);
						update = TRUE;
					}
				}
				
				/* If it was a mere comment (a note for human reading*, then
				it is not worth to keep in the memory, so it must be freed now.
				*/
				if (update == FALSE) {
					/* First, free its memeber data */
					db_constraints_free(cons);
					mem_free(cons);
				}
			}
		}
	}
	
	return constraints;
}


/**
 * Francis: This function will several sub (or child) functions
 * For each table, it has field(s).
 * For each field, it may have a list of comments
 * Now if the comment turns out a casual callback syntax, we build
 * a nice data sturcture, which can be easily accessable programmatically.
 */
gboolean
db_comment_callback_build_constraints(DbDatabaseDef *given_db)
{
	gint i, j;
	gchar * commentstr;
	GList * walk;
	
	DbFieldDef *currfield;
	/*DbCommentCallbackDef *callback;*/
	DbMethodDef *method;

	/* Set to static extern variable 'db', not to pass to subfunction all along */
	db = given_db;
	g_assert(db);
	

	/* For each table... */
	for (i = 0; i < db->numtable; i++)
	{
		/* For each field... */ 
		for (j = 0; j < db->table[i]->num; j++)
		{			
			currfield = db->table[i]->field[j];
			
			/* If comment field is null, then check the next field */
			if (currfield->comment == NULL) {
				/* Skip */
				continue;
			}

			/* Now, each field has comment list. So for each comment... */
			for (walk = g_list_first(currfield->comment); walk;
				walk = g_list_next(walk))
			{
				commentstr = (gchar*)walk->data;
/*
debugmsg("*** %s ***\n", commentstr);				
*/
				/* For each raw PostgreSQL column comment string, build a data
				structure easily access form */
				method =
					db_comment_callback_build_callback(commentstr);

				if (method != NULL) {
					/* Now append to the comment callback list.  */
					db->methods =
						g_list_append(db->methods, method);
				}

				/* Make the field to point to the method */
				currfield->method = method;
			}
			
		}
	}
	
	return TRUE;
}



/*
 * Author: Francis
 */


static DbMethodDef *
db_comment_callback_build_callback(gchar * commentstr)
{
	DbMethodDef *retval = NULL;
	DbParseResult res;
	
	g_assert(db);

	res = db_method_parse (&retval, commentstr);

	if (!res.ok)  {

		if ( check_if_old_syntax(commentstr) ) {

			/* It must be just a old syntex. */
			/* Do nothing. It will be handled by db_getcasualconstraint() */
			return NULL;
		} else {
			warningmsg("The casual relationship specified "
						"in comment string is WRONG");
			exit (1);
		}
	}
		
	return retval;
}



/* [01]; tablename.fieldname */
gboolean
check_if_old_syntax(gchar * commentstr)
{
	int count;
	gchar * ch;
	
	g_assert(commentstr);
	
	ch = commentstr;

#define ignoreblank() for (; isspace(*ch); ch++);
#define nextnonblank() for (ch++; isspace(*ch); ch++);

	ignoreblank();
	
	if ( !(*ch == '0' || *ch == '1' || *ch == '2') ) {
		putchar(*ch);
		return FALSE;
	}

	nextnonblank();

	/* The second character is semicolon ';' */
	if (*ch != ';') {
		return FALSE;
	}
	
	nextnonblank();

	/* The sequence of [a-zA-Z\_]+ follows */
	for (count = 0; *ch != '\0' && *ch != '.'; ch++, count++) {
	}
	/* Character "." follows after that */
	if (count == 0 || *ch != '.') {
		return FALSE;
	}
	/* The sequence of [a-zA-Z\_]+ follows */
	for (count = 0; *ch != '\0' && *ch != ' ' && *ch != '\t'; ch++, count++);

	if (count == 0) {
		return FALSE;
	}

	return TRUE;
}



/**
 * Return: FALSE if parsing fails, TRUE if it succeeds.
 * "hard"|"soft"|"any" ":" FUNCTIONNAME "(" (TABLENAME "." FIELDNAME)+ ")"
 */
#if 0
/* this function seems to be unused. "#if 0"'ed it out to stifle a warning ... */

static gboolean
parse_comment_callback_old(gchar * commentstr)
{
#define MAX_MODES 3

	gchar * funcname;
	gchar * ch;

	gchar *modes[MAX_MODES] = {"hard", "soft", "any"};

	gboolean matched;

	int idx;
	int count;
	
	ch = commentstr;	

	matched = FALSE;
	for (idx = 0; idx < MAX_MODES; idx++) {
		/* If one of { hard | soft | any } matches */
		if (!strncasecmp (commentstr, modes[idx], strlen(modes[idx]))) {
			ch += strlen(modes[idx]);
			matched = TRUE;
			break;
		}
	}
	
	/* If the mode name doesn't match, report syntax failure */
	if (!matched) {
		return FALSE;
	}
	
	/* A colon should follow after mode */
	if (*ch != ':') {
		return FALSE;
	}
	
	/* Then a function name should follow. Go forward until parant symbol */
	for (count = 0; ch[count] != '\0' && ch[count] != '('; count++)
		;
	
	/* If you hit, null character, it failed to parse. */
	if (ch[count] == '\0') {
		return FALSE;
	}	
	
	funcname = g_strndup(ch, count);
	
	ch += count;
	
	return TRUE;
}

#endif
