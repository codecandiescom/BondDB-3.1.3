#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef UNIX
#include <glob.h>
#endif

#include <glib.h>

#include "bcconfig.h"
#include "bcmem.h"
#include "bcdebug.h"

extern GHashTable *bondconfighash;
GHashTable *bondconfighash = NULL;
extern GList *bondexpectionfilelist;
GList *bondexpectionfilelist = NULL;

#ifndef WIN32
#define NUMBONDCONFIGPATH 7

static const gchar *bondconfigpath[NUMBONDCONFIGPATH] =
	{
	"./",
	"../etc/bond/",
	"~/.bond/",
	"/usr/local/etc/bond/",
	"/etc/bond/",
	"/etc/",
	"/usr/local/etc/"
	};
#elif WIN32
#define NUMBONDCONFIGPATH 1

static const gchar *bondconfigpath[NUMBONDCONFIGPATH] =
	{
	""
	};

#endif

static void
bcconfig_freestr(void *str)
	{
	mem_free(str);
	}

static void
parseline(gchar * buffer)
	{
#define __FUNCTION__ "0parseline"			 /* disable mem error reporting */
	gint i, j, k, slen;
	gchar *key = NULL, *value;

	/* GList *list; */

	if (buffer == NULL || buffer[0] == 0)
		return;
	if (buffer[0] == '#' || buffer[0] == ';')
		return;
	key = NULL;
	value = NULL;
	slen = strlen(buffer);
	for (i = 0; i < slen; i++)
		if (buffer[i] == '=')
			{
			/* get key */
			key = mem_calloc(i + 1);
			for (j = 0; j < i; j++)
				key[j] = tolower(buffer[j]);
			key[i] = 0;
			for (j = i - 1; j >= 0; j--)
				{
				if (key[j] == ' ' || key[j] == '\n')
					key[j] = 0;
				else
					break;
				}
			if (key[0] == 0)
				{
				mem_free(key);
				key = NULL;
				}
			/* get value */
			value = mem_calloc((slen - i) + 1);
			k = 0;
			for (j = i + 1; j < slen; j++)
				{
				if ((k == 0 && buffer[j] == ' ') || buffer[j] == '\n'
				        || buffer[j] == '\r')
					continue;
				value[k] = buffer[j];
				k++;
				}
			value[k] = 0;

			if (k > 1)
				for (j = k - 1; j > 0; j--)
					{
					if (value[j] == ' ' || value[j] == '\t'
					        || value[j] == '\n')
						value[j] = 0;
					else
						break;
					}

			if (value[0] == 0)
				{
				mem_free(value);
				value = NULL;
				}
			break;
			}
	if (key && value)
		{
		if (strcmp(key, "debug_only") == 0)
			{
			mem_free(key);
			bondexpectionfilelist =
			    g_list_prepend(bondexpectionfilelist, value);
			}
		else
			{
			g_hash_table_insert(bondconfighash, key, value);
			}
		}
	else if (key)
		mem_free(key);
	else if (value)
		mem_free(value);
	}

#undef __FUNCTION__

static void
bcconfig_parse(FILE * in)
	{
	gchar buffer[1000] = "";

	while (!feof(in))
		{
		fgets(buffer, 999, in);
		parseline(buffer);
		}
	}

/* In case been called from another directory */
static gchar *
bcconfig_findfile_ensurefine(gchar * appfile)
	{
#define __FUNCTION__ "0"						 /* disable mem error reporting */
	gint i, num;

	g_assert(appfile);
	appfile = strdup(appfile);
	g_assert(appfile);
	num = strlen(appfile);
#ifdef WIN32

	if (num > 4 && ! g_ascii_strcasecmp(appfile+num-4,".exe"))
		appfile[num - 4] = 0;
#endif

	for (i = num - 1; i >= 0; i--)
		{
#ifndef WIN32
		if (appfile[i] == '/')
#elif WIN32

		if (appfile[i] == '\\')
#endif

			{
			return appfile + (i + 1);
			}
		}
	return appfile;
#undef __FUNCTION__								 /* enable mem error reporting */
	}

static FILE *
bcconfig_findfile(gchar * appfile, gchar * configfile, gchar ** retfile)
	{
#define __FUNCTION__ "0bcconfig_findfile"	 /* disable mem error reporting */
	FILE *file;
	gint i;
	gchar *pattern;
	gchar *filename = NULL;

#ifndef WIN32

	glob_t g;
	gint count;

	*retfile = NULL;
	g_assert(appfile);
	appfile = bcconfig_findfile_ensurefine((gchar *) appfile);

	if (configfile == NULL)
		filename = mem_strdup_printf("%s.conf", appfile);
	else
		filename = mem_strdup(configfile);

       if ((file = fopen(filename, "r")) != NULL)
		{
		*retfile = filename;
		return file;
		}
	mem_free(filename);

	for (i = 0; i < NUMBONDCONFIGPATH; i++)
		{
		pattern = mem_strdup_printf("%s%s.conf", bondconfigpath[i], appfile);
		if (glob(pattern, 0, NULL, &g) == 0)
			for (count = 0; count < g.gl_pathc; count++)
				if ((file = fopen(g.gl_pathv[count], "r")) != NULL)
					{
					globfree(&g);
					*retfile = pattern;
					return file;
					}
		mem_free(pattern);
		}
	globfree(&g);
#elif defined(WIN32)

	GDir *g;
	GError *e = NULL;
	gchar *directory;

	*retfile = NULL;
	g_assert(appfile);
	appfile = bcconfig_findfile_ensurefine((gchar *) appfile);
	if (configfile == NULL)
		filename = mem_strdup_printf("%s.conf", appfile);
	else
		filename = mem_strdup_printf(configfile);

	if ((file = fopen(filename, "r")) != NULL)
		{
		*retfile = filename;
		return file;
		}
	mem_free(filename);
	filename = NULL;
	pattern = mem_strdup_printf("%s.conf", appfile);

	for (i = 0; i < NUMBONDCONFIGPATH; i++)
		{
		directory = mem_strdup_printf("%s", bondconfigpath[i]);

		g = g_dir_open(directory, 0, &e);

		if (g && !e)
			{
			while ((filename = (gchar *) g_dir_read_name(g)))
				{
				if (!strcmp(pattern, filename))
					{
					*retfile =
					    mem_strdup_printf("%s%s", directory, filename);
					if ((file = fopen(filename, "r")) != NULL)
						{
						g_dir_close(g);
						return file;
						}
					}
				}
			g_dir_close(g);
			}
		}
#endif
	return NULL;
#undef __FUNCTION__								 /* enable mem error reporting */
	}

gint
bcconfig_init(gint argc, gchar ** argv)
	{
#define __FUNCTION__ "0bcconfig_init"		 /* disable mem error reporting */
	gint i;
	gchar *appfile = argv[0];
	gchar *configfile = NULL;
	gchar *retfile;
	gchar *s;
	int n = 0;
	FILE *in;

	g_assert(!bondconfighash);

	bondconfighash =
	    g_hash_table_new_full(g_str_hash, g_str_equal, bcconfig_freestr,
	                          bcconfig_freestr);

	parseline(
#ifdef WIN32
	    "os=WIN32\n"
#else
#  ifdef UNIX
	    "os=UNIX\n"
#  else
	    "#os = UNKNOWN\n"
#warning "unrecognised target operating system"
#  endif
#endif
	);

	if (argc > 1)
		{
		for (i = 1; i < argc; i++)
			{
			if (i < argc - 1 && strcmp(argv[i], "-c") == 0 )
				{
				configfile = argv[++i];
				continue;
				}
			if (strcmp(argv[i], "-v") == 0)
				{
				++i;
				continue;
				}
			if(strstr(argv[i], "=") != NULL)
				parseline(argv[i]);
			}
		}
	if ((in = bcconfig_findfile(appfile, configfile, &retfile)) == NULL)
		{
		error_output("Failed to find a bond configuration file: %s.conf.\n"
		             "Make sure it is located in your current directory or "
		             "in /etc/bond/\n", appfile);
		return -1;
		}

	g_assert(retfile);
	/* create hash for config file */
	g_hash_table_insert(bondconfighash, mem_strdup("_config_file"), retfile);

	/* parse the config file */
	debug_output("Reading configuration file: %s.\n", retfile);
	bcconfig_parse(in);
	fclose(in);

	/* Overriding defaults with shell.conf file */
	/* Shell.conf is designed for network information as a single config
	 * file which all apps can use */
	if ((in = bcconfig_findfile(bcconfig_getvalue("shell_conf")?:"shell",NULL, &retfile)))
		{
		debug_output("Reading shared configuration file %s\n",retfile);
		bcconfig_parse(in);
		fclose(in);
		g_hash_table_insert(bondconfighash, mem_strdup("_shell_conf"), retfile);
		}
    else
	    debug_output("Shared configuration file not found: %s.conf\n",bcconfig_getvalue("shell_conf")?:"shell" );
	debug_line(NULL, NULL, Debug_Line);			
	if (argc > 1)
		{
		for (i = 1; i < argc ; i++)
			{
			if (i < argc - 1 && strcmp(argv[i], "-c") == 0)
				++i;
			if (i < argc - 1 && strcmp(argv[i], "-v") == 0)
				{
				debug_output("Override argument %s with %s\n",argv[i],argv[i+1]);
				parseline(argv[++i]);
				}
			else
				{	
				s = mem_strdup_printf("argv[%d]=%s\n", ++n, argv[i]);
				debug_output("Adding Argument Parse Line with %s\n",s);
				mem_free(s);
				}
			}
		}

	bcconfig_debug();								 /* load debug parameters from settings */
	return 0;
#undef __FUNCTION__								 /* enable mem error reporting */
	}

void
bcconfig_cleanup(void)
	{
	GList *walk;

	/*
		printf("\n\n\n\n\BCCONFIG_CLEANUP CALLED!!\n\n\n\n");
	*/
	g_hash_table_destroy(bondconfighash);
	bondconfighash = NULL;
	for (walk = g_list_first(bondexpectionfilelist); walk != NULL;
	        walk = walk->next)
		mem_free((gchar *) walk->data);
	g_list_free(bondexpectionfilelist);
	bondexpectionfilelist = NULL;
	}

gchar *
bcconfig_getvalue(gchar * key)
	{
	if (!bondconfighash)
		return NULL;
	return g_hash_table_lookup(bondconfighash, key);
	}

void
bcconfig_setvalue(gchar * key, gchar * newvalue)
	{
	if (!bondconfighash)
		return;
	if (g_hash_table_lookup(bondconfighash, key))
		g_hash_table_remove(bondconfighash, key);
	g_hash_table_insert(bondconfighash, mem_strdup(key), mem_strdup(newvalue));
	}

gint
bcconfig_debug()
	{
	gchar *tmpstr, *str = NULL;
	DebugInformation *di;

	di = debug_getdebuginfo();
	g_assert(di);

	if ((tmpstr = bcconfig_getvalue("debug_stream_stdout")))
		{
		if (strcmp(tmpstr, "null") == 0)
			di->debug_stream = NULL;
		else if (strcmp(tmpstr, "stdout") == 0)
			di->debug_stream = stdout;
		else if (strcmp(tmpstr, "stderr") == 0)
			di->debug_stream = stderr;
		else
			di->debug_stream = fopen(tmpstr, "a+");
		str = tmpstr;
		if (di->debug_stream == NULL)
			di->debug_stream = stdout;
		}
	if ((tmpstr = bcconfig_getvalue("debug_stream_stderr")))
		{
		if (strcmp(tmpstr, "null") == 0)
			di->error_stream = NULL;
		else if (strcmp(tmpstr, "stdout") == 0)
			di->error_stream = stdout;
		else if (strcmp(tmpstr, "stderr") == 0)
			di->error_stream = stderr;
		else if (str != NULL && strcmp(tmpstr, str) == 0)
			di->error_stream = di->debug_stream;
		else
			di->error_stream = fopen(tmpstr, "a+");
		if (di->error_stream == NULL)
			di->error_stream = stderr;
		if (di->error_stream == NULL)
			di->error_stream = stderr;
		}
	if ((tmpstr = bcconfig_getvalue("debug_stream_dump")))
		{
		if (strcmp(tmpstr, "stdout") == 0)
			di->dump_stream = stdout;
		else if (strcmp(tmpstr, "stderr") == 0)
			di->dump_stream = stderr;
		else if (str != NULL && strcmp(tmpstr, str) == 0)
			di->dump_stream = di->debug_stream;
		else
			di->dump_stream = fopen(tmpstr, "a+");
		if (di->dump_stream == NULL)
			di->dump_stream = stdout;
		}
	if ((tmpstr = bcconfig_getvalue("debug_enabled")))
		if (strcmp(tmpstr, "false") == 0)
			di->debuglevel = 0;
	if ((tmpstr = bcconfig_getvalue("debug_level")))
		di->debuglevel = atoi(tmpstr);
	if ((tmpstr = bcconfig_getvalue("debug_showbuffer")))
		di->showbuffer = atoi(tmpstr);
#ifndef WIN32
	di->colour = TRUE;
	if ((tmpstr = bcconfig_getvalue("debug_colour")))
		if (strcmp(tmpstr, "false") == 0)
			di->colour = FALSE;
#elif WIN32

	di->colour = FALSE;
	if ((tmpstr = bcconfig_getvalue("debug_colour")))
		if (strcmp(tmpstr, "true") == 0)
			di->colour = TRUE;
#endif

	if ((tmpstr = bcconfig_getvalue("debug_breakonerror")))
		if (strcmp(tmpstr, "true") == 0)
			di->breakonerror = TRUE;
	if ((tmpstr = bcconfig_getvalue("debug_breakonwarning")))
		if (strcmp(tmpstr, "true") == 0)
			di->breakonwarning = TRUE;
	if ((tmpstr = bcconfig_getvalue("debug_showmessages")))
		if (strcmp(tmpstr, "false") == 0)
			di->showmessages = FALSE;
	if ((tmpstr = bcconfig_getvalue("debug_showlinenumbers")))
		if (strcmp(tmpstr, "false") == 0)
			di->showlinenumbers = FALSE;
	if ((tmpstr = bcconfig_getvalue("debug_excludeall")))
		if (strcmp(tmpstr, "true") == 0)
			di->excludeall = TRUE;
	if (bondexpectionfilelist)
		{
		di->excludeall = TRUE;
		di->exceptionfiles = bondexpectionfilelist;
		}

	return 0;
	}
