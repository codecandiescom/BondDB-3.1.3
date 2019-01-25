#ifndef debug_h
#define debug_h

/* If you want to use rigour memory checking define MEM_DEBUG */
#include "dbmem.h"

#include <stdio.h>

typedef enum
{ Debug_Log, Debug_Msg, Debug_Printf }
DebugMsgType;
typedef enum
{ Debug_Header, Debug_Footer, Debug_MinorHeader, Debug_MinorFooter }
DebugLineType;

typedef struct
	{
	/* Simi-Public stuff (functions should be present to access these, don't use them directly */
	FILE *debug_stream;			  /* Logging stream to output to */
	FILE *error_stream;			  /* Error stream to output to */
	FILE *dump_stream;			  /* Anything that doesn't go to debug_Stream is sent here if not NULL */

	gboolean showlinenumbers;	  /* To show line numbers in debug messagE? (this effects tests) */
	gboolean showmessages;		  /* Show message() entries to the debug_stream. These usually used for one of messages */
	gboolean breakonerror;		  /* do a g_assert(NULL) if you run into error. */
	gboolean breakonwarning;	  /* to g_assert on warnings */
	gboolean excludeall;			  /* set to exclude all debugging messages. please note */
	/* that the default behaviour is to include all */
	gboolean errortoallstreams;  /* Is error messages go to the debug stream also */
	gboolean colour;				  /* To use colour or not */
	gboolean testinprogress;	  /* If you are doing a test and want to record the results */
	/* you can specify that a test is in progress and then everything inside that test is only outputed. */
	gint debuglevel;				  /* Debug level between 0-100, 100 been show everything. 0 show nothing */
	GList *exceptionfiles;		  /* List of files to include all but or exclue all but from debug message */

	/* Private data */
	gint olddebuglevel;
	gchar *currentcolour;		  /* This is set by debug_setcolour */
	}
DebugInformation;

#define DEBUG_MSGCOLOUR "\033[1m"			 /* FYI: \033[1m: Bright */
#define DEBUG_WARNINGCOLOUR "\033[31m"		 /* FYI: \033[36m: Cyan */
#define DEBUG_ERRORCOLOUR "\033[31m"		 /* FYI: \033[31m: Red */
#define DEBUG_RESETCOLOUR "\033[0m"			 /* FYI: \033[0m: Reset */

#ifdef OPTIMISE

/* These are hash defines to nothing, nothing at all which makes it all go faster.
 * Define optimise to disable all debugging messages and debugging checking .
 */

#else

/* Standard debugging functions. Use these not the C Functions */
#define debugmsg(fmt, args...) debug_msg (__FILE__, __LINE__, __PRETTY_FUNCTION__, 40, Debug_Log, fmt, ##args)
#define message(fmt, args...) debug_msg (__FILE__, __LINE__, __PRETTY_FUNCTION__, 80, Debug_Msg, fmt, ##args)
#define warningmsg(fmt, args...) debug_warning (__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##args)
#define errormsg(fmt, args...) debug_error (__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##args)

/* Additional functions */
#define debuglog(level, fmt, args...) debug_msg (__FILE__, __LINE__, __PRETTY_FUNCTION__, level, Debug_Log, fmt, ##args)
#define debugprintf(level, fmt, args...) debug_msg (__FILE__, __LINE__, __PRETTY_FUNCTION__, level, Debug_Printf, fmt, ##args)

/* Something for the prettier stuff in regard to formating */
#define debug_header() debug_line(__FILE__, __PRETTY_FUNCTION__, Debug_Header)
#define debug_footer() debug_line(__FILE__, __PRETTY_FUNCTION__, Debug_Footer)
#define debug_minorheader() debug_line(__FILE__, __PRETTY_FUNCTION__, Debug_MinorHeader)
#define debug_minorfooter() debug_line(__FILE__, __PRETTY_FUNCTION__, Debug_MinorFooter)

#endif /* optimise */

extern FILE *debug_stream;		  /* Marked as obsolete. DONT USE. */

/* NEW API */

gint debug_msg(gchar * file, gint line, gchar * function, gint level, DebugMsgType msgtype, const gchar * fmt, ...);
gint debug_error(gchar * file, gint line, gchar * function, const char *fmt, ...);
gint debug_warning(gchar * file, gint line, gchar * function, const gchar * fmt, ...);
gint debug_line(gchar * file, gchar * function, DebugLineType linetype);

void debug_cleanup(void);
void debug_init(void);

void debug_begintest(void);
void debug_endtest(void);
void debug_begintestingzone(void);
void debug_endtestingzone(void);

void debug_debugstream_set(FILE * stream);
void debug_errorstream_set(FILE * stream);
FILE *debug_debugstream_get(void);
FILE *debug_errorstream_get(void);

DebugInformation *debug_getdebuginfo(void);
void debug_appendexceptionfile(gchar * filename);

void debug_setcolour(gchar * colour);
void debug_usecolour(gboolean use);

/* Testing functions */

void debug_selftest(void);
void debug_printstatus(void);

gint test_total(gint pass, gint total);
gint test_result(gchar * retstr, gchar * shouldbe, gint * pass);
void test_annonce(gint testnum, gchar * testdescr);

/* OLD API */

/* some functions */
void setdebuglinenum(gint d);
void setdebuginfo(gint d);

/* to show debugging info or not.hmm */
void hidedebuginfo(void);
void showdebuginfo(void);

/* Francis: This is replaced by below macro, I thinkg */
FILE *debug_getstream(void);	  /* MARKED AS OBSOLETE */

/* Main print command */
void debug_printinfo(gchar * file, gint line, gchar * function, void *ptr);

/* Backwards compatability */
#define debug_output(fmt,args...) fprintf(debug_getstream(),"" fmt "",##args)
#define error_output(fmt,args...) fprintf(debug_errorstream_get(),"" fmt "",##args);

void debugwin(const char *fmt, ...);
void debugnote(const char *fmt, ...);

/*
#define debugwin(fmt, args...) debug_msg (__FILE__, __LINE__, __PRETTY_FUNCTION__, 40, Debug_Log, fmt, ##args)
#define debugnote(fmt, args...) debug_msg (__FILE__, __LINE__, __PRETTY_FUNCTION__, 99, Debug_Msg, fmt, ##args)
*/

#endif /* #define debug_h */
