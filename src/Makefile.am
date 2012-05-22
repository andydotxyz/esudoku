## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@etk_cflags@ @enhance_cflags@

bin_PROGRAMS = esudoku

if BUILD_ETK
etk_src = gui_etk.h \
gui_etk.c
endif

esudoku_SOURCES = \
esudoku.h \
macros.h \
core.h \
core.c \
util.h \
util.c \
rules.h \
rules.c \
$(etk_src) \
main.c

esudoku_LDADD = @etk_libs@ @enhance_libs@

EXTRA_DIST = esudoku.glade

