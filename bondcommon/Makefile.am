lib_LTLIBRARIES = libbondcommon.la

libbondcommon_la_SOURCES = \
			 bc.h \
			 bcmem.h \
			 bcdebug.h \
			 bcconfig.h \
			 bcgarbage.h \
			 \
			 bc.c \
			 bcmem.c  \
			 bcdebug.c  \
			 bcconfig.c \
			 bcgarbage.c
			 
			 
libbondcommon_la_LDFLAGS = @LDFLAGS@ @GLIB_LIBS@
libbondcommon_la_LIBADD = 
libbondcommon_la_DEPENDENCIES = 

libbondcommonincdir = $(includedir)/bond
libbondcommoninc_HEADERS = bcconfig.h bc.h bcmem.h bcdebug.h bcgarbage.h

INCLUDES = -ansi -DUNIX @GLIB_CFLAGS@
