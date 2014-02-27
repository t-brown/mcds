#!/bin/sh
set -e -v
gettextize --force --copy
aclocal -I m4
autoheader
automake --add-missing
autoconf

