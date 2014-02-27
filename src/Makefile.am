localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

bin_PROGRAMS = mcds

mcds_LDFLAGS = $(LTLIBINTL)
mcds_SOURCES = main.h main.c

noinst_HEADERS = gettext.h
