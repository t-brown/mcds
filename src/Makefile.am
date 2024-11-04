localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
AUTOMAKE_OPTIONS = nostdinc

bin_PROGRAMS = mcds

mcds_CPPFLAGS = $(CURL_CFLAGS)                  \
                $(XML_CFLAGS)                   \
                $(SECRET_CFLAGS)                \
                $(GPGME_CFLAGS)

mcds_LDADD = $(LTLIBINTL)                       \
             $(CURL_LIBS)                       \
             $(XML_LIBS)                        \
             $(SECRET_LIBS)                     \
             $(GPGME_LIBS)

mcds_SOURCES = defs.h                           \
               options.h                        \
               main.c                           \
               mem.c            mem.h           \
               curl.c           curl.h          \
               carddav.c        carddav.h       \
               xml.c            xml.h           \
               vcard.c          vcard.h         \
               rc.c             rc.h            \
	       prompt.c         prompt.h

if WANT_GPGME
mcds_SOURCES += decrypt.c       decrypt.h
endif

if WANT_LIBSECRET
mcds_SOURCES +=	secret.c         secret.h
endif

noinst_HEADERS = gettext.h
dist_man_MANS = mcds.1
