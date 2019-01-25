#ifndef dbbirth_h
#define dbbirth_h

#include "dbheader.h"
#include "dbfield.h"

/* Stores how an object was created. */
struct DbBirth
   {
   /* sql statement that was used to create... */
   gchar *sql;
   /* default table to get field values from */
   gchar *tablename;
   /* Array of values that were evaluted */
   gchar *essential;				  /* Is this item essential to birth creation */
   gchar **value;
   DbField **field;				  /* NOTE to self: Fixed chicken or egg dilema. */
   gint num;
   /* Incase its defined by a unique id */
   /* This can be %NULL if there isn't any uniqueid to restrict this object */
   DbUniqueId *id;
   /* Need a cref marking how it came here */
   };

/* Standard create and destroy */
DbBirth *db_birth_create(DbUniqueId * id, gchar * tablename);
void db_birth_free(DbBirth * birth);

/* modify the birth component based on pre-requests */
gint db_birth_addvalue(DbBirth * birth, gchar * fieldname, gchar * value);
gint db_birth_applycref(Object * obj, DbBirth * birth, DbConstraintDef * cref);

/* Apply a birth to an object, making it so */
gint db_birth_save(Object * obj, DbBirth * birth);

/* Sql help along functions */
gchar *db_birth_createsql(DbBirth * birth);
DbBirth *db_birth_createfromsql(gchar * sql);

/* Is an event effect this Object */
gboolean db_birth_iseffected(Object * objcheck, Object * objchanged, DbCache * cache);

/* CHICKENS! */
gint db_birth_springchicken(Object * obj);

#endif
