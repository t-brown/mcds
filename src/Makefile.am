localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

bin_PROGRAMS = mcds

mcds_CPPFLAGS = $(CURL_CFLAGS)			\
		$(XML_CFLAGS)

mcds_LDFLAGS = $(LTLIBINTL)			\
	       $(CURL_LIBS)			\
	       $(XML_LIBS)

mcds_SOURCES = main.h		main.c		\
	       defs.h		options.h	\
	       mem.h		mem.c		\
	       curl.h		curl.c		\
	       carddav.h	carddav.c


noinst_HEADERS = gettext.h
