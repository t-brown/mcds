
/*
 * Copyright (C) 2014  Timothy Brown
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file main.c
 * Main entry point for the program MCDS.
 *
 * \ingroup main
 * \{
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>

#include <locale.h>
#include "gettext.h"

#include <getopt.h>
#include <curl/curl.h>

#include "defs.h"
#include "options.h"
#include "mem.h"
#include "rc.h"
#include "curl.h"
#include "carddav.h"
#include "xml.h"


/* Initalise extern definitions */
#define X(a, b) b,
char *sterm_name[] = {			/**< Search term names */
	STERMS_TABLE
};
#undef X

struct opts options = {0};		/**< Program options */

/* Internal functions */
static void print_usage(void);
static void print_version(void);
static int  parse_argv(int, char **, char **);
static const char *program_name(void);

/**
 * The main entry point of the program.
 *
 * \param argc   Number of command line arguments.
 * \param argv  Reference to the pointer to the argument array list.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
main(int argc, char **argv)
{

	char *file = NULL;	/* config file */
	char *res = NULL;	/* query result */
	CURL *hdl = NULL;	/* Curl handle */

#ifdef HAVE_PLEDGE
	if (pledge("stdio rpath inet dns proc exec unveil", NULL) == -1) {
		err(1, "pledge");
	}
#endif

#ifdef HAVE_SETLOCALE
	setlocale(LC_ALL, "");
#endif
#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif

	if (parse_argv(argc, argv, &file)) {
		return(EXIT_FAILURE);
	}

	if (read_rc(file)) {
		return(EXIT_FAILURE);
	}

#ifdef HAVE_UNVEIL
	if (unveil(NULL, NULL) == -1) {
		warn(_("Unable to disable further unveil"));
		return(EXIT_FAILURE);
	}
#endif

	if (options.verbose) {
		fprintf(stderr, "%s options are:\n", program_name());
		fprintf(stderr, "  URL               : %s\n", options.url);
		fprintf(stderr, "  SSL Verify        : %d\n", options.verify);
		fprintf(stderr, "  Use .netrc        : %d\n", options.netrc);
		fprintf(stderr, "  Use libsecret     : %d\n", options.libsecret);
		fprintf(stderr, "  Password prompted : %d\n", options.pwprompt);
		fprintf(stderr, "  Username          : %s\n", options.username);
		fprintf(stderr, "  Password          : %s\n", options.password);
		fprintf(stderr, "  Query term        : %s\n", options.term);
		fprintf(stderr, "  Query             : %s\n",
				sterm_name[options.query]);
		fprintf(stderr, "  Search            : %s\n",
				sterm_name[options.search]);
	}

	if (cinit(&hdl)) {
		return(EXIT_FAILURE);
	}
	if (query(hdl, &res)) {
		return(EXIT_FAILURE);
	}
	if (cfini(&hdl)) {
		return(EXIT_FAILURE);
	}

	if (parse_xml(res)) {
		return(EXIT_FAILURE);
	}

	if (options.url) {
		free(options.url);
		options.url = NULL;
	}
	if (options.term) {
		free(options.term);
		options.term = NULL;
	}
	if (options.username) {
		free(options.username);
		options.username = NULL;
	}
	if (options.password) {
		free(options.password);
		options.password = NULL;
	}
	if (res) {
		free(res);
		res = NULL;
	}

	if (file) {
		free(file);
		file = NULL;
	}

	return(EXIT_SUCCESS);
}

/**
 * Parse the command line arguments.
 *
 * \param[in] argc Number of command line arguments.
 * \param[in] argv Reference to the pointer to the argument array list.
 * \param[out] file A configuration file.
 *
 * \retval 0 If there were no errors.
 **/
static int
parse_argv(int argc, char **argv, char **file)
{
	int opt = 0;
	int opt_index = 0;
	char *soptions = "c:hpq:s:u:Vv";        /* short options structure */
	static struct option loptions[] = {     /* long options structure */
		{"config",     required_argument,  NULL,  'c'},
		{"help",       no_argument,        NULL,  'h'},
		{"password",   no_argument,        NULL,  'p'},
		{"query",      required_argument,  NULL,  'q'},
		{"search",     required_argument,  NULL,  's'},
		{"url",        required_argument,  NULL,  'u'},
		{"version",    no_argument,        NULL,  'V'},
		{"verbose",    no_argument,        NULL,  'v'},
		{NULL,         1,                  NULL,  0}
	};

	/* set the default field to search */
	options.query  = name;
	options.search = email;

	/* parse the arguments */
	while ((opt = getopt_long(argc, argv, soptions, loptions,
				  &opt_index)) != -1) {
		switch (opt) {
		case 'c':
			*file = strdup(optarg);
			break;
		case 'h':
			print_usage();
			break;
		case 'p':
			options.pwprompt = 1;
			break;
		case 'q':
			if (optarg[0] == 'a' ||
			    optarg[0] == 'A' ) {
				options.query = address;
			} else if (optarg[0] == 'e' ||
				   optarg[0] == 'E' ) {
				options.query = email;
			} else if (optarg[0] == 'n' ||
				   optarg[0] == 'N' ) {
				options.query = name;
			} else if (optarg[0] == 't' ||
				   optarg[0] == 'T' ) {
				options.query = telephone;
			}
			break;
		case 's':
			if (optarg[0] == 'a' ||
			    optarg[0] == 'A' ) {
				options.search = address;
			} else if (optarg[0] == 'e' ||
				   optarg[0] == 'E' ) {
				options.search = email;
			} else if (optarg[0] == 'n' ||
				   optarg[0] == 'N' ) {
				options.search = name;
			} else if (optarg[0] == 't' ||
				   optarg[0] == 'T' ) {
				options.search = telephone;
			}
			break;
		case 'u':
			if (options.url) {
				free(options.url);
			}
			options.url = xmalloc((strlen(optarg)+1)*sizeof(char));
			strcpy(options.url, optarg);
			break;
		case 'V':
			print_version();
			break;
		case 'v':
			options.verbose = 1;
			break;
		default:
			print_usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1) {
		warnx(_("Must specify a term to query for."));
		print_usage();
	}

	options.term = xmalloc((strlen(argv[0]) +1) * sizeof(char));
	strcpy(options.term, argv[0]);

	return(EXIT_SUCCESS);
}

/**
 * Prints a short program usage statement, explaining the
 * command line arguments and flags expected.
 **/
static void
print_usage(void)
{
	printf(_("\
usage: %s [-c config] [-h] [-q a|e|n|t] [-s a|e|n|t] [-u URL] [-V] [-v] string\n\
  -c, --config       A configuration file to use.\n\
  -h, --help         Display this help and exit.\n\
  -p, --password     Prompt for a password.\n\
  -q, --query  a|e|n|t Query term (default name). Known terms are:\n\
                     a = address\n\
                     e = email\n\
                     n = name\n\
                     t = telephone\n\
  -s, --search a|n|e|t Search term (default email). Known terms are:\n\
                     a = address\n\
                     e = email\n\
                     n = name\n\
                     t = telephone\n\
  -u, --url          The URL of the carddav server to query.\n\
  -V, --version      Display version information and exit.\n\
  -v, --verbose      Verbose mode.\n\
  string             The query string to look for within the query term.\n\
"), program_name());
	exit(EXIT_FAILURE);
}

/**
 * Prints the program version number, copyright information and
 * compile date.
 **/
static void
print_version(void)
{
	printf(_("%s (GNU %s) %s\n"), program_name(), PACKAGE, VERSION);
	printf(_("\
Copyright (C) %s Timothy Brown.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\n"), "2019");
	printf(_("Compiled on %s at %s:\n"
		 " -  %s GPGME support.\n"
		 " - %s libsecret support.\n\n"),
	       __DATE__, __TIME__,
	       ngettext("with", "with-out", HAVE_GPGME),
	       ngettext("with", "with-out", HAVE_LIBSECRET)
	       );
	exit(EXIT_FAILURE);
}

static const char *
program_name(void)
{
#if HAVE_GETPROGNAME
	  return getprogname();
#else
#if HAVE_PROGRAM_INVOCATION_SHORT_NAME
	    return program_invocation_short_name;
#else
	      return "unknown";
#endif /* HAVE_PROGRAM_INVOCATION_SHORT_NAME */
#endif /* HAVE_GETPROGNAME */
}

/**
 * \}
 **/
