#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "safescanner.h"
#include "dbmethod.h"
#include "dbgather.h"
#include "bc.h"

static void
db_method_parse_error_cleanup(gchar * tmp_callbackname, GList * tmp_arglist);

static gboolean
check_method_validity (gchar * funcname, GList * arglist);

/**
 * db_method_new:
 * @callbackname:
 * @arglist:
 *
 * 
 */
DbMethodDef *
db_method_new (DbCasualType type, gchar *callbackname, GList *arglist)
{
	DbMethodDef * retval;
	
	retval = g_new0(DbMethodDef, 1);
	
	retval->casualtype = type;
	retval->callbackname = callbackname;
	retval->arglist = arglist;

	return retval;
}

DbMethodArg *
db_method_arg_new(DbMethodArgType type, gchar * value)
	{
	DbMethodArg *retval;

	retval = g_new(DbMethodArg, 1);

	retval->type = type;
	retval->value = value;

	return retval;
	}

void
db_method_arg_cleanup(DbMethodArg * arg)
	{
	g_free(arg->value);
	g_free(arg);
	}

/**
 * db_method_cleanup:
 *
 * To destroy a structure of type DbMethodDef with all the elements of arglist
 */
void
db_method_cleanup(DbMethodDef * methoddef)
	{
	GList *walk;

	for (walk = g_list_first(methoddef->arglist); walk; walk = g_list_next(walk))
		{
		db_method_arg_cleanup((DbMethodArg *) walk->data);
		}
	g_list_free(methoddef->arglist);

	g_free(methoddef);
	}

/*
 * db_method_list_cleanup:
 * This is a destroyer of list of DbMethodDef *
 * Especially designed for DbDatabaseDef's db->methods
 */
void
db_method_list_cleanup(GList * methods)
	{
	GList *walk;

	for (walk = g_list_first(methods); walk; walk = g_list_next(walk))
		{
		db_method_cleanup((DbMethodDef *) walk->data);
		}
	g_list_free(methods);
	}

/**
 * 
 * db_method_parse:
 * Parse the name of the widget if is conforms to bond method
 * syntax form

 * @method: *method should be set null to emphasize
 * @text: A string to be parsed
 *
 * Returns: FALSE if parsing fails, TRUE if it succeeds.
 * "hard"|"soft"|"any" ":" FUNCTIONNAME "("
 *                         (TABLENAME "." FIELDNAME) | STRING )+ ")"
 *
 */
DbParseResult
db_method_parse(DbMethodDef ** method, gchar * text)
	{
	SafeTokenType tokentype;
	SafeTokenValue tokenval;
	SafeScanner *scanner = NULL;
	DbParseResult retval;
	DbMethodArg *arg;

	DbCasualType mode = 0; /* Assume the default value is 0 */
	gchar *tmp_callbackname = NULL;
	GList *tmp_arglist = NULL;
	int argcount = 0;


	g_assert(method != NULL);
	g_assert(*method == NULL); /* Memory will be allocated in this function */
	g_assert(text != NULL);
	
	/* Assume parsing will fail */
	memset(&retval,0,sizeof(DbParseResult));
	retval.ok = FALSE;

	if (strchr (text, '(') == NULL || strchr (text, ')') == NULL)
		{
		strncpy(retval.errormsg, "non-function form", MAXERRORLEN);
		return retval;
		}

	/* Create a new scanner */
	/* Passing NULL means to use default config */	
	scanner = safe_scanner_new(NULL);
	scanner->config->scan_string_sq = TRUE;
	scanner->config->scan_string_dq = TRUE;
	scanner->config->scan_float = FALSE; /* Turn off floating number feature */


	safe_scanner_input_text(scanner, (gchar *) text, strlen((gchar *) text));



	/* The second argument is 'scope_id'. The default scope_id is 0
	So here I use 1 for scope_id to distiguish it */

	safe_scanner_scope_add_symbol (scanner, 1, "hard", GINT_TO_POINTER(0));
	safe_scanner_scope_add_symbol (scanner, 1, "soft", GINT_TO_POINTER(1));
	safe_scanner_scope_add_symbol (scanner, 1, "any", GINT_TO_POINTER(2));

	/* At the beginning, {"hard" | "soft" | "any"} should NOT be treated
	as IDENTIFIER. Thus, setting scope to 1 */
	safe_scanner_set_scope (scanner, 1);
	tokentype = safe_scanner_peek_next_token (scanner);


	if (tokentype == SAFE_TOKEN_SYMBOL) {

		tokentype = safe_scanner_get_next_token (scanner);
		tokenval = safe_scanner_cur_value (scanner);
		mode = GPOINTER_TO_INT(tokenval.v_symbol);

		if (mode >= 0 && mode <= 2) {
			/* OK */
		}else {
			fprintf(stderr, "This can't happen!!! Added other symbol?\n");
			exit(EXIT_FAILURE); /* Just die */
		}

		/* Scanning mode identifier done */
		safe_scanner_set_scope(scanner, 0);

		/* Get a colon */
		tokentype = safe_scanner_get_next_token(scanner);
		if (tokentype != ':') {
			return retval;
		}

	}
	

	/* Get a callback name */
	tokentype = safe_scanner_get_next_token(scanner);

	if (tokentype != SAFE_TOKEN_IDENTIFIER) {
		strncpy(retval.errormsg, "near callback name", MAXERRORLEN);
		return retval;
	}
	
	tokenval = safe_scanner_cur_value(scanner);	
	tmp_callbackname = g_strdup(tokenval.v_identifier);

	/* Get '(' */
	tokentype = safe_scanner_get_next_token(scanner);
	if (tokentype != SAFE_TOKEN_LEFT_PAREN)
		{
		strncpy(retval.errormsg, "near '(' expected", MAXERRORLEN);
		db_method_parse_error_cleanup (tmp_callbackname, tmp_arglist);
		return retval;
	}

	/* Hmm, I must take care of the dot? */
	scanner->config->cset_identifier_nth = G_CSET_a_2_z "_" G_CSET_A_2_Z "0123456789" G_CSET_LATINS G_CSET_LATINC ".";

	/* Here goes the callback's arguments It will be either a fieldname (which can be of form tablename.fieldname) */
	while (safe_scanner_peek_next_token(scanner) != SAFE_TOKEN_RIGHT_PAREN &&
	        safe_scanner_peek_next_token(scanner) != SAFE_TOKEN_EOF)
		{

		argcount++;
		/* Identifier | ',' | String */
		tokentype = safe_scanner_get_next_token(scanner);
		tokenval = safe_scanner_cur_value(scanner);

		if (tokentype == SAFE_TOKEN_IDENTIFIER)
			{
			arg = db_method_arg_new(DB_METHOD_ARG_TYPE_VARIABLE, g_strdup(tokenval.v_identifier));

			tmp_arglist = g_list_append(tmp_arglist, arg);

			}
		else if (tokentype == SAFE_TOKEN_STRING)
			{
			arg = db_method_arg_new(DB_METHOD_ARG_TYPE_CONSTANT, g_strdup(tokenval.v_string));
			tmp_arglist = g_list_append(tmp_arglist, arg);
			}
		else
			{
			gchar *tmpstr;

			tmpstr = g_strdup_printf("Passing argument %d is wrong", argcount);
			strncpy(retval.errormsg, tmpstr, MAXERRORLEN);
			g_free(tmpstr);

			db_method_parse_error_cleanup (tmp_callbackname, tmp_arglist);
			return retval;
		}

		/* If next token is comma just comsume it */
		if (safe_scanner_peek_next_token(scanner) == SAFE_TOKEN_COMMA)
			safe_scanner_get_next_token(scanner);
		}

	/* Get ')' */
	tokentype = safe_scanner_get_next_token(scanner);
	if (tokentype != SAFE_TOKEN_RIGHT_PAREN) {
		db_method_parse_error_cleanup (tmp_callbackname, tmp_arglist);
		return retval;
	}


	/* So far, the syntax was OK, now check if it makes sense */
	if ( !check_method_validity (tmp_callbackname, tmp_arglist) ) {
		db_method_parse_error_cleanup (tmp_callbackname, tmp_arglist);
		strncpy(retval.errormsg, "'table.field' doesn't exist", MAXERRORLEN);
		return retval;
	}

	(*method) = db_method_new (mode, tmp_callbackname, tmp_arglist);
	
	retval.ok = TRUE;
	return retval;
}




static void
db_method_parse_error_cleanup(gchar * tmp_callbackname, GList * tmp_arglist)
{
	GList *walk;
	DbMethodArg *arg;

	if (tmp_callbackname != NULL) {
		g_free (tmp_callbackname);
	}
	

	if (tmp_arglist != NULL) {

		for (walk = g_list_first(tmp_arglist);
			walk != NULL;
			walk = g_list_next(walk)) {

			arg = (DbMethodArg *)walk->data;
			if (arg != NULL) {
				g_free(arg->value);
			}
		}

		g_list_free (tmp_arglist);
	}

}


gchar *
db_method_to_string(DbMethodDef * method)
	{
	GString *retval;
	gchar *retstr;
	GList *walk;
	DbMethodArg *arg;
	gchar *tmpstr;

	retval = g_string_new("method: ");
	retval = g_string_append(retval, method->callbackname);
	retval = g_string_append(retval, "\n");

	tmpstr = g_strdup_printf ("casual type: %d\n", method->casualtype);
	retval = g_string_append(retval, tmpstr);
	g_free (tmpstr);

	retval = g_string_append(retval, "args:\n");

	for (walk = g_list_first(method->arglist); walk != NULL; walk = g_list_next(walk))
		{
		arg = (DbMethodArg *) walk->data;

		tmpstr = g_strdup_printf("\t(%s, %s)\n",
		                         (arg->type == DB_METHOD_ARG_TYPE_VARIABLE) ? "Var" : "Const", arg->value);
		retval = g_string_append(retval, tmpstr);
		g_free(tmpstr);
		}

	retstr = retval->str;
	g_string_free(retval, FALSE);

	return retstr;
	}

/**
 * db_method_find
 * @methods:
 * @callbacks:
 *
 * This is the api for listing the all widget callbacks
 * (I guess this could be put in the BOND rather than BOND DB)
 *
 * Returns: NULL if there is no such method
 */
DbMethodDef *
db_method_find(GList * methods, gchar * callbackname)
	{
	GList *walk;
	DbMethodDef *currnode, *retval = NULL;

	g_assert(methods);

	for (walk = g_list_first(methods); walk; walk = g_list_next(walk))
		{
		currnode = (DbMethodDef *) walk->data;
		if (strcmp(currnode->callbackname, callbackname) == 0)
			{
			retval = currnode;
			break;
			}
		}

	return retval;
	}

/**
 * TRUE if the method is valid
 **/
static gboolean
check_method_validity (gchar * funcname, GList * arglist)
{
	int idx1, idx2;
	GList * walk;
	gboolean tablematched, fieldmatched;
	gchar **name = NULL;

	DbDatabaseDef *db;
	DbMethodArg *arginfo;
	
	db = db_get_current_database ();

	/* Check minimum sanity */
	g_assert(funcname);
	/*g_assert(arglist);*/ /* Hmm. It is true can be void */
	g_assert(db);

	
	for (walk = g_list_first(arglist); walk; walk = g_list_next(walk)) {

		arginfo = (DbMethodArg*)walk->data;
		if (arginfo->type != DB_METHOD_ARG_TYPE_VARIABLE)
			continue;

		name = g_strsplit (arginfo->value, ".", 2);

		tablematched = FALSE;
		/* For each table... */
		for (idx1 = 0; idx1 < db->numtable; idx1++) {
		
			/* For each table name, find if the table name exists in the list */
			if (!strcmp(name[0], db->table[idx1]->name)) {
				
				tablematched = TRUE;				

				/* Now check field */
				fieldmatched = FALSE;
				for (idx2 = 0; idx2 < db->table[idx1]->num; idx2++) {
					
					/* Field name match? */
					if (!strcmp(name[1],
						db->table[idx1]->field[idx2]->name ) )
					{
						fieldmatched = TRUE;
						break;
					}
				}
				
				/* If field not matched */
				if (!fieldmatched) {
					warningmsg("The field %s in table %s does not exist.",
						name[1], db->table[idx1]->name);

					g_strfreev (name);
					return FALSE;
				}
				break;
			}
		}
		
		if (!tablematched) {
			warningmsg("The table %s does not exist.", name[0]);
			g_strfreev (name);
			return FALSE;
		}
	}

	if (name) {
		g_strfreev (name);
	}

	/* All the lists has been matched. So return true */
	return TRUE;
}



void
reporttype(SafeTokenType ty)
{
	switch (ty) {
		case   SAFE_TOKEN_EOF:
		puts("SAFE_TOKEN_EOF\n");
		break;

		case   SAFE_TOKEN_LEFT_PAREN:
		puts("SAFE_TOKEN_LEFT_PAREN\n");
		break;
		case   SAFE_TOKEN_RIGHT_PAREN:
		puts("SAFE_TOKEN_RIGHT_PAREN\n");
		break;
		case   SAFE_TOKEN_LEFT_CURLY:
		puts("SAFE_TOKEN_LEFT_CURLY\n");
		break;
		case   SAFE_TOKEN_RIGHT_CURLY:
		puts("SAFE_TOKEN_RIGHT_CURLY\n");
		break;
		case   SAFE_TOKEN_LEFT_BRACE:
		puts("SAFE_TOKEN_LEFT_BRACE\n");
		break;
		case   SAFE_TOKEN_RIGHT_BRACE:
		puts("SAFE_TOKEN_RIGHT_BRACE\n");
		break;
		case   SAFE_TOKEN_EQUAL_SIGN:
		puts("SAFE_TOKEN_EQUAL_SIGN\n");
		break;
		case   SAFE_TOKEN_COMMA:
		puts("SAFE_TOKEN_COMMA\n");
		break;

		case   SAFE_TOKEN_NONE:
		puts("SAFE_TOKEN_NONE\n");
		break;

		case   SAFE_TOKEN_ERROR:
		puts("SAFE_TOKEN_ERROR\n");
		break;

		case   SAFE_TOKEN_CHAR:
		puts("SAFE_TOKEN_CHAR\n");
		break;
		case   SAFE_TOKEN_BINARY:
		puts("SAFE_TOKEN_BINARY\n");
		break;
		case   SAFE_TOKEN_OCTAL:
		puts("SAFE_TOKEN_OCTAL\n");
		break;
		case   SAFE_TOKEN_INT:
		puts("SAFE_TOKEN_INT\n");
		break;
		case   SAFE_TOKEN_HEX:
		puts("SAFE_TOKEN_HEX\n");
		break;
		case   SAFE_TOKEN_FLOAT:
		puts("SAFE_TOKEN_FLOAT\n");
		break;
		case   SAFE_TOKEN_STRING:
		puts("SAFE_TOKEN_STRING\n");
		break;

		case   SAFE_TOKEN_SYMBOL:
		puts("SAFE_TOKEN_SYMBOL\n");
		break;
		case   SAFE_TOKEN_IDENTIFIER:
		puts("SAFE_TOKEN_IDENTIFIER\n");
		break;
		case   SAFE_TOKEN_IDENTIFIER_NULL:
		puts("SAFE_TOKEN_IDENTIFIER_NULL\n");
		break;

		case   SAFE_TOKEN_COMMENT_SINGLE:
		puts("SAFE_TOKEN_COMMENT_SINGLE\n");
		break;
		case   SAFE_TOKEN_COMMENT_MULTI:
		puts("SAFE_TOKEN_COMMENT_MULTI\n");
		break;
		case   SAFE_TOKEN_LAST:
		puts("SAFE_TOKEN_LAST\n");
		break;

		case '.':
		puts("A Dot!\n");
		break;

		default:
		printf("UNKNOWN %d\n", ty);
	}
}
