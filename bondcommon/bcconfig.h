#ifndef bcconfig_h
#define bcconfig_h

gint bcconfig_init(gint argc, gchar **argv);
void bcconfig_cleanup(void);
gchar *bcconfig_getvalue(gchar *key) __attribute__ ((pure));
void bcconfig_setvalue(gchar * key, gchar *newvalue);
gint bcconfig_debug(void);

#endif
