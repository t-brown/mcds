localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
AUTOMAKE_OPTIONS = nostdinc

bin_PROGRAMS = mcds

mcds_CPPFLAGS = $(CURL_CFLAGS)			\
		$(XML_CFLAGS)

mcds_LDFLAGS = $(LTLIBINTL)			\
	       $(CURL_LIBS)			\
	       $(XML_LIBS)

mcds_SOURCES = defs.h		options.h	\
	       main.c				\
	       mem.c		mem.h		\
	       curl.c		curl.h		\
	       carddav.c	carddav.h

noinst_HEADERS = gettext.h
man_MANS = mcds.1
