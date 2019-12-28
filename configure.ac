#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

AC_PREREQ(2.61)
AC_INIT([mcds], [1.4], [tbrown@freeshell.org])
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
AX_COMPILER_VENDOR
AC_PROG_CC
AC_PROG_CC_STDC

# Checks for header files
AC_HEADER_STDC
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

dnl locate gpgme and gpg
AH_TEMPLATE([GPGME_CONFIG], [Defined to the full path of gpgme-config])
AH_TEMPLATE([GPG_CONFIG], [Defined to the full path of gpgconf])
AH_TEMPLATE([GPG], [Defined to the full path of gpg/gpg2])

AC_ARG_ENABLE([gpgme],
        AS_HELP_STRING([--disable-gpgme], [do not use GPGME support]))

AS_IF([test x$enable_gpgme != xno],
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
echo "C compiler                 : $ax_cv_c_compiler_vendor"
echo "C command                  : $CC $CFLAGS"
echo "Linker                     : $LD $LDFLAGS $LIBS"
echo "Enable GPGME               : $enable_gpgme"
echo
