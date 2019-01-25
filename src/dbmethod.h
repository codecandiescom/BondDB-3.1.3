#ifndef dbmethod_h
#define dbmethod_h

#include <glib.h>
#define MAXERRORLEN 30

typedef struct {
	gboolean ok;
	gchar errormsg[MAXERRORLEN+1];
} DbParseResult;


typedef enum {
	DB_METHOD_ARG_TYPE_VARIABLE, /* Means a fieldname */
	DB_METHOD_ARG_TYPE_CONSTANT /* i.e. string value*/
} DbMethodArgType;

typedef struct {
	DbMethodArgType type;
	gchar *value;
} DbMethodArg;

typedef enum {DB_CASUAL_HARD, DB_CASUAL_SOFT, DB_CASUAL_ANY } DbCasualType;

typedef struct {
	gchar * callbackname;
	GList * arglist;
	DbCasualType casualtype;
} DbMethodDef;

DbMethodDef *
db_method_new(DbCasualType type, gchar *callbackname, GList *arglist);

DbMethodArg *
db_method_arg_new (DbMethodArgType type, gchar * value);

void
db_method_arg_cleanup (DbMethodArg * arg);

void
db_method_cleanup (DbMethodDef * methoddef);

void
db_method_list_cleanup (GList *methods);

DbParseResult
db_method_parse(DbMethodDef **method, gchar * text);

gchar *
db_method_to_string(DbMethodDef *method);

DbMethodDef *
db_method_find (GList * methods, gchar * callbackname);

#endif
