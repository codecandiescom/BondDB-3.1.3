/**
 * debug.c module.
 * treshna Enterprises Ltd
 * 
 * This file provides message features for logging, error messages and other
 * useful functions associated with debugging. Different debugging levels
 * can be set up, you can control how what areas you report messages from,
 * and where to ignore from.  Colouring can be used.
 * 
 * 
 */

/** I AM TIGER */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <glib.h>

/* This is normally defined in autoconf */

/* #define GTK_DEBUGNOTE 0 */

/* If GTK debugging is enabled, then use it */
#if GTK_DEBUGNOTE

#include <gtk/gtk.h>
#define MSG_BUF_SIZE 1000

#endif

/* Protype headers */
#include "debug.h"

/* Francis: Remained for the old debug API... */
gint debug_linenumshow;
gint debug_info;
FILE *debug_stream;

/* Define globaldebuginfo */
static DebugInformation globaldebuginfo;
static gint debug_comparestring(gpointer a, gpointer b);

/**
 * Set default values.
 */
void
debug_init(void)
	{
	debug_stream = stdout;

	globaldebuginfo.debug_stream = stdout;	 /* debug_stream */
	globaldebuginfo.error_stream = stderr;	 /* error_stream */
	globaldebuginfo.dump_stream = NULL;		 /* where it all goes in the rubbish */
	globaldebuginfo.showlinenumbers = TRUE; /* showlinenumbers */
	globaldebuginfo.showmessages = TRUE;	 /* showmessages */
	globaldebuginfo.breakonerror = FALSE;	 /* breakonerror */
	globaldebuginfo.breakonwarning = FALSE; /* breakonwarning */
	globaldebuginfo.excludeall = FALSE;		 /* excludeall */
	globaldebuginfo.colour = TRUE;			 /* colour */
	globaldebuginfo.testinprogress = TRUE;	 /* testinprogress */
	globaldebuginfo.errortoallstreams = FALSE;
	globaldebuginfo.debuglevel = 100;		 /* debuglevel */
	globaldebuginfo.exceptionfiles = NULL;	 /* exceptionfiles */
	globaldebuginfo.currentcolour = mem_strdup("");	/* 0 length string. */
	}

/**
 * debug_getdebuginfo:
 * Obtaining the structure you can manipulate the detail of the debug.
 * However, but please do not try anything too much this structure dependant
 * work. I could have provided functions (or methods) for each member of
 * structure...
 */

DebugInformation *
debug_getdebuginfo(void)
	{
	return &globaldebuginfo;
	}

/**
 * debug_appendexceptionfile:
 * @filename: Filename to add.
 * 
 * Add a file into the exception list so that debugging information is now displayed
 * on it. 
 *
 * Returns: Absolutly nothing.
 */
void
debug_appendexceptionfile(gchar * filename)
	{
	globaldebuginfo.exceptionfiles = (GList *) g_list_prepend(globaldebuginfo.exceptionfiles, (gchar *) filename);
	}

static gchar *
truefalse(gboolean val)
	{
	return val ? "TRUE" : "FALSE";
	}

static gint
debug_comparestring(gpointer a, gpointer b)
	{
	return strcmp((const char *)a, (const char *)b);
	}

static gint
debug_printwhereabout(gchar * file, gint line, gchar * colour, gchar * function)
	{
	gint retval = 0;

	if (globaldebuginfo.showlinenumbers)
		retval = fprintf(globaldebuginfo.debug_stream, " <%s%s:%d %s();> ", colour, file, line, function);
	else
		retval = fprintf(globaldebuginfo.debug_stream, " <%s%s: %s();> ", colour, file, function);

	return retval;
	}

/**
 * debug_msg:
 * @fmt: The format follows standard C library's printf() function series
 *
 *
 * Returns: the number of characters printed excluding the location info
 */
gint
debug_msg(gchar * file, gint line, gchar * function, gint level, DebugMsgType msgtype, const char *fmt, ...)
	{
	gint retval = 0;
	va_list args;

	g_assert(level >= 0 && level <= 100);
	/* i say . Um if within debug level and u have no exclude all, or if you have exclude all and your in the right file
	   then show debugging info. right yes thats what it does */
	if (level <= globaldebuginfo.debuglevel &&
	        (globaldebuginfo.excludeall == FALSE || (globaldebuginfo.excludeall == TRUE &&
	                g_list_find_custom(globaldebuginfo.exceptionfiles, file,
	                                   debug_comparestring))))
		{
		/* To use colour or not to use colour ... */
		if (msgtype == Debug_Msg && globaldebuginfo.colour == TRUE)
			retval += debug_printwhereabout(file, line, DEBUG_MSGCOLOUR, function);
		else if (globaldebuginfo.colour == TRUE && globaldebuginfo.currentcolour != NULL)
			retval += debug_printwhereabout(file, line, globaldebuginfo.currentcolour, function);
		else
			retval += debug_printwhereabout(file, line, "", function);
		/* Print the actual message */
		va_start(args, fmt);
		retval += vfprintf(globaldebuginfo.debug_stream, fmt, args);
		va_end(args);
		fprintf(globaldebuginfo.debug_stream, "%s", DEBUG_RESETCOLOUR);
		if (msgtype != Debug_Printf)
			fprintf(globaldebuginfo.debug_stream, "\n");
		}
	/* This is the dump stream where the unwanted go */
	else if (globaldebuginfo.dump_stream != NULL)
		{
		retval += fprintf(globaldebuginfo.dump_stream, " <%s:%d %s();> ", file, line, function);

		va_start(args, fmt);
		retval += vfprintf(globaldebuginfo.dump_stream, fmt, args);
		va_end(args);
		if (msgtype != Debug_Printf)
			fprintf(globaldebuginfo.dump_stream, "\n");
		}

	return retval;
	}

/* high level error message to stderr */
gint
debug_error(gchar * file, gint line, gchar * function, const char *fmt, ...)
	{
	gint retval = 0;				  /* Let's keep this init althoug it's useless */
	va_list args;

	va_start(args, fmt);

	if (globaldebuginfo.colour)
		{
		fprintf(globaldebuginfo.error_stream, "%sError %s: %d: %s(): ", DEBUG_ERRORCOLOUR, file, line, function);
		retval = vfprintf(globaldebuginfo.error_stream, fmt, args);
		fprintf(globaldebuginfo.error_stream, "%s\n", DEBUG_RESETCOLOUR);
		if (globaldebuginfo.errortoallstreams == TRUE)
			{
			fprintf(globaldebuginfo.debug_stream, "%sError %s: %d: %s(): ", DEBUG_ERRORCOLOUR, file, line, function);
			retval = vfprintf(globaldebuginfo.debug_stream, fmt, args);
			fprintf(globaldebuginfo.debug_stream, "%s\n", DEBUG_RESETCOLOUR);
			}
		}
	else
		{
		retval = fprintf(globaldebuginfo.error_stream, "Error %s: %d: %s(): ", file, line, function);
		vfprintf(globaldebuginfo.error_stream, fmt, args);

		if (globaldebuginfo.errortoallstreams == TRUE)
			{
			retval = fprintf(globaldebuginfo.debug_stream, "Error %s: %d: %s(): ", file, line, function);
			vfprintf(globaldebuginfo.debug_stream, fmt, args);
			}
		}
	va_end(args);

	if (globaldebuginfo.breakonerror)
		{
		g_assert(FALSE);
		}

	return retval;
	}

/**
 * debug_warning:
 * 
 * 
 * 
 */
gint
debug_warning(gchar * file, gint line, gchar * function, const gchar * fmt, ...)
	{
	gint retval = 0;				  /* Let's keep this init althoug it's useless */
	va_list args;

	va_start(args, fmt);

	if (globaldebuginfo.colour)
		{
		fprintf(globaldebuginfo.error_stream, "%sWarning %s: %d: %s(): ", DEBUG_ERRORCOLOUR, file, line, function);
		retval = vfprintf(globaldebuginfo.error_stream, fmt, args);
		fprintf(globaldebuginfo.error_stream, "%s\n", DEBUG_RESETCOLOUR);

		if (globaldebuginfo.errortoallstreams == TRUE)
			{
			fprintf(globaldebuginfo.debug_stream, "%sWarning %s: %d: %s(): ", DEBUG_ERRORCOLOUR, file, line,
			        function);
			retval = vfprintf(globaldebuginfo.debug_stream, fmt, args);
			fprintf(globaldebuginfo.debug_stream, "%s\n", DEBUG_RESETCOLOUR);
			}
		}
	else
		{
		retval = fprintf(globaldebuginfo.error_stream, "Warning %s: %d: %s(): ", file, line, function);
		vfprintf(globaldebuginfo.error_stream, fmt, args);

		if (globaldebuginfo.errortoallstreams == TRUE)
			{
			retval = fprintf(globaldebuginfo.debug_stream, "Warning %s: %d: %s(): ", file, line, function);
			vfprintf(globaldebuginfo.debug_stream, fmt, args);
			}
		}
	va_end(args);

	if (globaldebuginfo.breakonwarning)
		{
		g_assert(NULL);
		}

	return retval;
	}

gint
debug_line(gchar * file, gchar * function, DebugLineType linetype)
	{
	gint retval = 0;

	/* Now thats an if statement */
	if ((linetype == Debug_MinorHeader || linetype == Debug_MinorFooter) && globaldebuginfo.debuglevel >= 40)
		;
	else if ((linetype == Debug_Header || linetype == Debug_Footer) && globaldebuginfo.debuglevel >= 10)
		;
	else
		return 0;
	if (globaldebuginfo.excludeall == FALSE)
		;
	else
		if (globaldebuginfo.excludeall == TRUE &&
		        g_list_find_custom(globaldebuginfo.exceptionfiles, file, debug_comparestring))
			;
		else
			return 0;
	switch (linetype)
		{
	case Debug_Header:
		retval += fprintf(globaldebuginfo.debug_stream, "\n\n__________ --> %s: %s() __________\n", file, function);
		break;
	case Debug_Footer:
		retval += fprintf(globaldebuginfo.debug_stream, "__________ %s() <-- __________\n\n", function);
		break;
	case Debug_MinorHeader:
		retval += fprintf(globaldebuginfo.debug_stream, "\n..........  --> %s: %s() ..........\n", file, function);
		break;
	case Debug_MinorFooter:
		retval += fprintf(globaldebuginfo.debug_stream, ".......... %s() <-- ..........\n\n", function);
		break;
	default:
		retval += fprintf(globaldebuginfo.debug_stream, "\nUnkown line type!\n");
		break;
		}
	return retval;
	}

void
debug_printstatus(void)
	{
	GList *lI = NULL;
	FILE *output = stdout;

	fprintf(output, "Show line numbers: %s\n", truefalse(globaldebuginfo.showlinenumbers));
	fprintf(output, "Show message: %s\n", truefalse(globaldebuginfo.showmessages));
	fprintf(output, "Break on error: %s\n", truefalse(globaldebuginfo.breakonerror));
	fprintf(output, "Break on warning: %s\n", truefalse(globaldebuginfo.breakonwarning));
	fprintf(output, "Exclude all debug message: %s\n", truefalse(globaldebuginfo.excludeall));
	fprintf(output, "Use ANSI terminal colour: %s\n", truefalse(globaldebuginfo.colour));
	fprintf(output, "Test in progress: %s\n", truefalse(globaldebuginfo.testinprogress));

	fprintf(output, "Debug level (0-100): %d\n", globaldebuginfo.debuglevel);

	fprintf(output, "Debug exceptions files (%s):\n",
	        (globaldebuginfo.excludeall) ? "to be included" : "to be excluded");

	fprintf(output, "Current colour: %s\n", globaldebuginfo.currentcolour);

	/* Farncis: lI = list Iterator. Intentionally avoided 'l' or 'li' because in case I need to change those name or
	   search in editor they have to be unique enough. Just image if typed 'l' or 'li' the search will highlight
	   virtually all source code lines !!! */
	for (lI = g_list_first(globaldebuginfo.exceptionfiles); lI; lI = g_list_next(lI))
		{
		fprintf(output, "\t%s", (gchar *) lI->data);
		}

	}

/**
 * debug_begintest:
 * 
 * This call is used in unit tests where you want to compare the results with
 * pre-stored results.  
 */
void
debug_begintest(void)
	{
	globaldebuginfo.testinprogress = TRUE;
	globaldebuginfo.debuglevel = 0;
	}

/**
 * debug_begintestingzone:
 *
 * Inform the debugging message that this is a testing zone so dont display any
 * debug messages unless your inside a debug_begintest() and debug_endtest().
 * 
 */
void
debug_begintestingzone(void)
	{
	globaldebuginfo.testinprogress = FALSE;
	globaldebuginfo.olddebuglevel = globaldebuginfo.debuglevel;
	globaldebuginfo.debuglevel = 100;
	}

/**
 * debug_endtestingzone:
 * 
 * End the testing zone and go back to normal operation.
 */
void
debug_endtestingzone(void)
	{
	globaldebuginfo.testinprogress = TRUE;
	globaldebuginfo.debuglevel = globaldebuginfo.olddebuglevel;
	}

/**
 * debug_endtest:
 *
 * Call this after you have finished a unit test, this will keep the system in the
 * testing zone though.
 */
void
debug_endtest(void)
	{
	globaldebuginfo.testinprogress = FALSE;
	globaldebuginfo.debuglevel = 100;
	}

void
debug_selftest(void)
	{
	DebugInformation *di;

	debug_init();
	debug_printstatus();
	di = debug_getdebuginfo();
	di->breakonerror = FALSE;
	di->colour = TRUE;
	debug_error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "This is errormsg %s %d", "number", 1);
	debug_warning(__FILE__, __LINE__, __PRETTY_FUNCTION__, "This is warningmsg %s %d", "number", 1);
	}

FILE *
debug_debugstream_get(void)
	{
	return globaldebuginfo.debug_stream;
	}

FILE *
debug_errorstream_get(void)
	{
	return globaldebuginfo.error_stream;
	}

/**
 * debug_debugstream_set:
 * @stream: 
 * 
 * Set the output stream, it uses stdio FILE streams.  This is where most
 * messages are logged to.  
 */
void
debug_debugstream_set(FILE * stream)
	{
	if (globaldebuginfo.debug_stream != stdout && globaldebuginfo.debug_stream != stderr
	        && globaldebuginfo.debug_stream != NULL)
		fclose(globaldebuginfo.debug_stream);
	globaldebuginfo.debug_stream = stream;
	}

/**
 * debug_errorstream_set:
 * @stream: 
 * 
 * Set the error stream, it uses stdio FILE streams. Everything to the error
 * streams is also outputted to debug streams.
 */
void
debug_errorstream_set(FILE * stream)
	{
	if (globaldebuginfo.error_stream != stdout && globaldebuginfo.error_stream != stderr
	        && globaldebuginfo.error_stream != NULL)
		fclose(globaldebuginfo.error_stream);
	globaldebuginfo.error_stream = stream;
	}

/**
 * debug_cleanup:
 * 
 * Free up all the streams and depart. 
 *
 */
void
debug_cleanup(void)
	{
	if (debug_stream != stdout && debug_stream != stderr && debug_stream != NULL)
		{
		fclose(debug_stream);
		debug_stream = NULL;
		}
	if (globaldebuginfo.currentcolour)
		{
		mem_free(globaldebuginfo.currentcolour);
		globaldebuginfo.currentcolour = NULL;
		}
	}

/**
 * debug_setcolour:
 *
 * Set the current colour for debugging messages. You need to reset the colour
 * to disable it.
 */
void
debug_setcolour(gchar * colour)
	{
	/* Need some fancy stuff here, or maybe not. Could set colours by a number or hex or something more easier than
	   those strings and then set like that. */

	if (globaldebuginfo.currentcolour != NULL)
		mem_free(globaldebuginfo.currentcolour);
	globaldebuginfo.currentcolour = mem_strdup(colour);
	}

void
debug_usecolour(gboolean use)
	{
	globaldebuginfo.colour = use;
	}

/*****************************************************************
 
 Francis:
 Old functions start from here
 I left them due to compatability reason.
 For example, if you use old obsolete APIs, such as
 debug_getstream(), you will get old FILE pointer's value.
 
******************************************************************/

/**
 * setdebuglinenum:
 * @d: 
 * 
 * Marked as OBSOLETE.
 */
void
setdebuglinenum(gint d)
	{
	debug_linenumshow = d;
	}

/**
 * setdebuginfo:
 * @d: 
 * 
 * Marked as OBSOLETE.
 */
void
setdebuginfo(gint d)
	{
	debug_info = d;
	}

/**
 * hidedebuginfo:
 * @d: 
 * 
 * Marked as OBSOLETE.
 */
void
hidedebuginfo()
	{
	debug_stream = fopen("debugmsg.log", "wt");
	}

/**
 * showdebuginfo:
 * @d: 
 * 
 * Marked as OBSOLETE.
 */
void
showdebuginfo()
	{
	if (debug_stream != stdout && debug_stream != stderr && debug_stream != NULL)
		fclose(debug_stream);
	debug_stream = stdout;
	}

/* talk about emtpy C files */

void
debug_printinfo(gchar * file, gint line, gchar * function, void *ptr)
	{
	if (debug_linenumshow == 0)
		fprintf(debug_stream, " <%s:%d %s();>\n", file, line, function);
	else
		{
		fprintf(debug_stream, " <%s: %s();>\n", file, function);
		}
	}

FILE *
debug_getstream(void)
	{
	return debug_debugstream_get();
	}

#if GTK_DEBUGNOTE

static FILE *globalfp = NULL;

/* example-start helloworld helloworld.c */

/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below. */
static void hello(GtkWidget * widget, gpointer data);
static void launch_control_panel();
static void debug_showwin(gchar * message);

static void
hello(GtkWidget * widget, gpointer data)
	{
	gchar *message;
	long filesize;

	g_print("Hello World\n");

	if (!globalfp)
		{
		return ;
		}

	fseek(globalfp, 0L, SEEK_END);
	filesize = ftell(globalfp);
	rewind(globalfp);

	printf("size: %ld\n", filesize);

	message = malloc((int)sizeof(gchar) * (filesize + 1));
	fread(message, sizeof(gchar), filesize, globalfp);
	message[(int)filesize] = '\0';
	printf("%s\n", message);
	debug_showwin(message);
	fclose(globalfp);

	unlink("./debug.log");
	globalfp = NULL;
	}

static void
launch_control_panel()
	{
	/* GtkWidget is the storage type for widgets */
	GtkWidget *window;
	GtkWidget *button;

	/* create a new window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Debug Manager");

	/* Sets the border width of the window. */
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	/* Creates a new button with the label "Hello World". */
	button = gtk_button_new_with_label("Show Next");

	gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
	                          GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));

	gtk_signal_connect_object(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));

	/* When the button receives the "clicked" signal, it will call the function hello() passing it NULL as its argument.
	   The hello() function is defined above. */
	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(hello), NULL);

	/* This will cause the window to be destroyed by calling gtk_widget_destroy(window) when "clicked".  Again, the
	   destroy signal could come from here, or the window manager. */

	/*
	   gtk_signal_connect_object (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT
	   (window)); */

	/* This packs the button into the window (a gtk container). */
	gtk_container_add(GTK_CONTAINER(window), button);

	/* The final step is to display this newly created widget. */
	gtk_widget_show(button);

	/* and the window */
	gtk_widget_show(window);

	return ;
	}

/* example-end */

static void
debug_showwin(gchar * message)
	{
	GtkWidget *dialog, *label, *okay_button;
	GtkWidget *text, *scrollwin;
	gchar *title;
	static int counts = 0;
	int pos;

	/* Create the widgets */

	title = g_strdup_printf("Notifier %d", counts++);
	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), title);
	gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, FALSE);
	g_free(title);

	// label = gtk_label_new(message);
	/* Text Area */
	text = gtk_text_new(NULL, NULL);
	gtk_widget_ref(text);
	gtk_object_set_data_full(GTK_OBJECT(dialog), "text", text, (GtkDestroyNotify) gtk_widget_unref);

	gtk_editable_insert_text(GTK_EDITABLE(text), message, strlen(message), &pos);

	/* Scrolled Window */
	scrollwin = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrollwin);
	gtk_object_set_data_full(GTK_OBJECT(dialog), "scrollwin", scrollwin, (GtkDestroyNotify) gtk_widget_unref);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	/* OK button */
	gtk_window_set_modal((GtkWindow *) dialog, TRUE);
	okay_button = gtk_button_new_with_label("OK!");

	/* Ensure that the dialog box is destroyed when the user clicks ok. */

	gtk_signal_connect_object(GTK_OBJECT(okay_button), "clicked",
	                          GTK_SIGNAL_FUNC(gtk_widget_destroy), (gpointer) dialog);

	/* Add the label, and show everything we've added to the dialog. */

	gtk_container_add(GTK_CONTAINER(scrollwin), text);
	gtk_container_add(GTK_CONTAINER(scrollwin), text);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), scrollwin);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->action_area), okay_button);

	gtk_widget_show_all(dialog);

	}

/**
 * debugnote:
 * 
 * A popup window is shown with the message.
 * 
 */
void
debugnote(const char *fmt, ...)
	{
	va_list ap;
	gchar *title;
	char message[MSG_BUF_SIZE];
	static int notecount = 0;
	GtkWidget *dialog, *label, *okay_button;

	/* Comment out to enable more than 30 pops, else it will quit */
	g_assert(notecount < 30);
	va_start(ap, fmt);
	vsnprintf(message, MSG_BUF_SIZE, fmt, ap);
	va_end(ap);

	/* Create the widgets */

	dialog = gtk_dialog_new();
	title = g_strdup_printf("=== Note %d ===", notecount++);
	gtk_window_set_title(GTK_WINDOW(dialog), title);
	g_free(title);

	gtk_window_set_modal((GtkWindow *) dialog, TRUE);
	label = gtk_label_new(message);
	okay_button = gtk_button_new_with_label("Okay");

	/* Ensure that the dialog box is destroyed when the user clicks ok. */

	gtk_signal_connect_object(GTK_OBJECT(okay_button), "clicked",
	                          GTK_SIGNAL_FUNC(gtk_widget_destroy), (gpointer) dialog);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->action_area), okay_button);

	/* Add the label, and show everything we've added to the dialog. */

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), label);
	gtk_widget_show_all(dialog);
	}

/**
 * debugwin:
 * 
 * Display a window containing debugging output.
 * 
 */
void
debugwin(const char *fmt, ...)
	{
	va_list ap;

	static int count = 0;
	static gboolean launched = FALSE;

	if (!globalfp)
		{
		globalfp = fopen("./debug.log", "w+");
		}

	fprintf(globalfp, "%3d ", count);

	va_start(ap, fmt);
	vfprintf(globalfp, fmt, ap);
	va_end(ap);

	fprintf(globalfp, "\n");
	fflush(globalfp);

	if (!launched)
		{
		launched = TRUE;
		launch_control_panel();
		}

	count++;
	}

#else

void
debugnote(const char *fmt, ...)
	{}

void
debugwin(const char *fmt, ...)
	{}

#endif

gint
test_total(gint pass, gint total)
	{

	if (pass != total)
		{
		error_output("TEST FAILED, with %d out of %d\n", pass, total);
		return -1;
		}

	debug_output("TEST PASSED\n");
	return 0;
	}

gint
test_result(gchar * retstr, gchar * shouldbe, gint * pass)
	{
	if (retstr == NULL)
		{
		error_output("Test of %s failed due to NULL value, it should be %s\n", retstr, shouldbe);
		return -2;
		}

	if (strcmp(retstr, shouldbe) == 0)
		{

		(*pass)++;
		return 0;
		}

	error_output("Test result compare of %s and %s failed\n", retstr, shouldbe);
	return -1;
	}

void
test_annonce(gint testnum, gchar * testdescr)
	{
	debug_output("TEST STARTING: (%d) %s\n", testnum, testdescr);
	}
