
if GTK_DEBUGNOTE
INCLUDES = -ggdb `gtk-config --cflags` \
	 -Wall -Wimplicit -Wreturn-type -Wunused -Wswitch -Wcomment \
	 -Wuninitialized -Wparentheses -Wpointer-arith \
	 -Wmissing-prototypes -DGTK_DEBUGNOTE=1 -fprofile-arcs -ftest-coverage 
else
INCLUDES = -ggdb `glib-config --cflags` \
	 -Wall -Wimplicit -Wreturn-type -Wunused -Wswitch -Wcomment \
	 -Wuninitialized -Wparentheses -Wpointer-arith \
	 -Wmissing-prototypes -fprofile-arcs -ftest-coverage 
endif


VERSION = 1.0.0

lib_LTLIBRARIES = libbonddebug.la

##
## libbonddebug.la
##
libbonddebug_la_SOURCES = \
	dbmem.h debug.h \
	\
	dbmem.c debug.c 

libbonddebugincdir = $(includedir)/bond
libbonddebuginc_HEADERS = dbmem.h debug.h
libbonddebug_la_LDFLAGS = 
