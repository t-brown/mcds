#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

AC_PREREQ(2.61)
AC_INIT([mcds], 0.1, Timothy.Brown@terralimno.com)
AC_CONFIG_AUX_DIR([build-aux])

# Create the VERSION file, which contains the package version
echo AC_PACKAGE_VERSION>VERSION
AC_SUBST(PACKAGE_VERSION)

# Internationalization
AM_GNU_GETTEXT_VERSION([0.18.3])
AM_GNU_GETTEXT([external])

# Initialise automake
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Define our M4 macro directory
AC_CONFIG_MACRO_DIR([m4])

# Check there is a source file
AC_CONFIG_SRCDIR([src/main.c])
# Put our generated config header in the source directory
AC_CONFIG_HEADERS([src/config.h])

# Checks the compiler vendor
AX_COMPILER_VENDOR

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h string.h unistd.h])

# Checks for functions and libraries
AC_CHECK_FUNCS([memset getprogname program_invocation_short_name])
AC_FUNC_MALLOC

AC_CONFIG_FILES([ Makefile
		 po/Makefile.in
		 src/Makefile
                 ])

PKG_CHECK_MODULES([libcurl], [libcurl])
PKG_CHECK_MODULES([libxml2], [libxml-2.0])

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
echo "C compiler                 : $ax_cv_c_compiler_vendor $CC"
echo "C command                  : $CC $CFLAGS"
echo "Linker                     : $LD $LDFLAGS $LIBS"
echo
