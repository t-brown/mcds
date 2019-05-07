MCDS
====

[![Build Status](https://travis-ci.com/t-brown/mcds.svg?branch=master)](https://travis-ci.com/t-brown/mcds)

`mcds` is a command line tool primarily used as a search query plugin
for mutt to query a CardDav server.


Prerequisites
-------------

+  [C compiler](https://gcc.gnu.org/)
+  [Curl](https://curl.haxx.se/libcurl/)
+  [LibXML2](http://www.xmlsoft.org/)
+  Optionally [GPGME](https://www.gnupg.org/software/gpgme/index.html)


Building / Installation
-----------------------

`mcds` relies on the GNU build system
[autoconf](https://www.gnu.org/software/autoconf/) and
[automake](https://www.gnu.org/software/automake/).

To install `mcds` with the default options:

    ./configure
    make
    make install

This will install the binrary tool in `/usr/local/bin` and man pages in
`/usr/local/man`. To specify a different installation prefix, use the
--prefix option to configure:

    ./configure --prefix=/opt
    make
    make install

Will install `mcds` in `/opt/{bin,man}`.

Usage
-----

The utility `mcds` queries a CardDav server. For example to query
all email address of people called Fred in your addressbook at the
URL https://example.org/addressbook:

    mcds -u https://example.org/addressbook Fred

If you had an entry for Fred Smith it would return a result like:

    fred.smith@example.org   Fred Smith

mcds can make use of an rc file (`${HOME}/.mcdrc`). Please read the
accompanying man page for more examples and the specifications of the
rc file.

### Typical URLs.

The typical URL to query for various CardDav servers.

+ [Davical](https://www.davical.org/)
    `https://example.org/caldav.php/username/addresses`

+ [Owncloud](https://owncloud.org/)
    `http://demo.owncloud.org/remote.php/carddav/addressbooks/username/contacts`

+ [Gmail](https://gmail.com/)
    `https://www.googleapis.com/carddav/v1/principals/username@example.com/lists/default`
where one needs to subsitute your email address for `username@example.com`.


