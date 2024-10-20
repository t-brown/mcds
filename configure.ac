#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

AC_PREREQ(2.61)
AC_INIT([mcds], [1.7], [tbrown@freeshell.org])
AC_CONFIG_AUX_DIR([build-aux])

# Define our M4 macro directory
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST(PACKAGE_VERSION)

# Internationalization
AM_GNU_GETTEXT_VERSION([0.19])
AM_GNU_GETTEXT([external])

# Initialise automake
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Check there is a source file
AC_CONFIG_SRCDIR([src/main.c])
# Put our generated config header in the source directory
AC_CONFIG_HEADERS([src/config.h])

# Checks the compiler vendor
AC_PROG_CC

# GPGME is compiled with _FILE_OFFSET_BITS=64 on i386
AC_SYS_LARGEFILE

# Checks for header files
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for functions and libraries
AC_CHECK_FUNCS([getprogname \
		memset \
		pledge program_invocation_short_name \
		unveil])
AC_FUNC_MALLOC

# Shouldn't need this on newer automakes
AM_PROG_CC_C_O

AC_CONFIG_FILES([Makefile
		 po/Makefile.in
		 src/Makefile
                 ])

PKG_CHECK_MODULES([CURL], [libcurl])
PKG_CHECK_MODULES([XML], [libxml-2.0])
PKG_CHECK_MODULES([SECRET], [libsecret-1],, [HAVE_PKG_LIBSECRET=0])

AC_ARG_ENABLE([libsecret],
        AS_HELP_STRING([--disable-libsecret], [do not use libsecret support]))

AS_IF([test "$HAVE_PKG_LIBSECRET" != "0"],
         [AS_IF([test x$enable_libsecret != xno],
                [AC_DEFINE([HAVE_LIBSECRET], [1], [Define to 1 if you have libsecret support])
		 AC_SUBST(LIBSECRET_CFLAGS)
		 AC_SUBST(LIBSECRET_LIBS)
                 enable_libsecret=yes
                ],
                [AC_DEFINE([HAVE_LIBSECRET], [0], [Define to 1 if you have libsecret support])
                 enable_libsecret=no
		]
          )],
          [AC_DEFINE([HAVE_LIBSECRET], [0], [Define to 1 if you have libsecret support])
           enable_libsecret=no
          ]
)

dnl locate gpgme and gpg
AH_TEMPLATE([GPGME_CONFIG], [Defined to the full path of gpgme-config])
AH_TEMPLATE([GPG_CONFIG], [Defined to the full path of gpgconf])
AH_TEMPLATE([GPG], [Defined to the full path of gpg/gpg2])

AC_ARG_ENABLE([gpgme],
        AS_HELP_STRING([--disable-gpgme], [do not use GPGME support]))

m4_ifdef([AM_PATH_GPGME],
         [AS_IF([test x$enable_gpgme != xno],
                [AM_PATH_GPGME([],
                               [AC_DEFINE([HAVE_GPGME], [1], [Define to 1 if you have GPGME support])
                                AM_CONDITIONAL([WANT_GPGME], [test x = x])
                               ],
                               [AC_MSG_ERROR([GPGME not found, if you do not want GPGME support, rerun with --disable-gpgme])])
                 AC_DEFINE_UNQUOTED([GPGME_CONFIG], ["$GPGME_CONFIG"])
                 AC_PATH_PROG([GPG_CONFIG], [gpgconf])
                 AS_IF([test x$GPG_CONFIG != x],
                       [AC_DEFINE_UNQUOTED([GPG_CONFIG], ["$GPG_CONFIG"])],
                       [AC_MSG_ERROR([gpgconf is needed with GPGME])]
                 )
                 AC_PATH_PROGS([GPG], [gpg gpg2])
                 AS_IF([test x$GPG != x],
                       [AC_DEFINE_UNQUOTED([GPG], ["$GPG"])],
                       [AC_MSG_ERROR([gpg or gpg2 is needed with GPGME])]
                 )
                 enable_gpgme=yes
                ],
                [AC_DEFINE(HAVE_GPGME, 0, [Define to 1 if you have GPGME support])
                 AM_CONDITIONAL([WANT_GPGME], [test x = y])]
          )],
          [AC_DEFINE(HAVE_GPGME, 0, [Define to 1 if you have GPGME support])
           AM_CONDITIONAL([WANT_GPGME], [test x = y])
           enable_gpgme=no
          ]
)



AC_OUTPUT
echo
echo
echo "========================================================================"
echo "$PACKAGE_NAME              version $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "Installation prefix        : $prefix"
echo "C command                  : $CC $CFLAGS"
echo "Linker                     : $LD $LDFLAGS $LIBS"
echo "Enable GPGME               : $enable_gpgme"
echo "Enable libsecret           : $enable_libsecret"
echo
