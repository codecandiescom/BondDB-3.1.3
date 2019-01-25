## Process this file with automake to produce Makefile.in

INCLUDES = @GLIB_CFLAGS@

parser.c: parser.y
	bison -v -d -o parser.c -p sql parser.y

lexer.c: lexer.l parser.c
	flex -olexer.c -Psql lexer.l

noinst_PROGRAMS = sql_parser_test

lib_LTLIBRARIES = libbondsql.la

libbondsql_la_SOURCES = parser.c lexer.c sql_parser.c mem.c sql_display.c \
		sql_tree.c 
libbondsqlincdir = $(includedir)/bond
libbondsqlinc_HEADERS = sql_parser.h
libbondsql_la_LDFLAGS = @GLIB_LIBS@

sql_parser_test_SOURCES = sql_parser_test.c
sql_parser_test_LDFLAGS = @GLIB_LIBS@
sql_parser_test_DEPENDENCIES =
sql_parser_test_LDADD = $(top_builddir)/bondsql/.libs/libbondsql.a 

tests: sql_parser_test
