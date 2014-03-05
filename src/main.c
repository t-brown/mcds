
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
 * Main entry point for the program.
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

#include <locale.h>
#include "gettext.h"

#include <getopt.h>
#include <curl/curl.h>

#include "defs.h"
#include "options.h"
#include "mem.h"
#include "curl.h"
#include "carddav.h"


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
static int  parse_argv(int, char **, char **, char **);
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

	char *url  = NULL;	/* CardDav URL */
	char *name = NULL;	/* Name to search/query for */
	char *res  = NULL;	/* query result */
	CURL *hdl  = NULL;	/* Curl handle */

#ifdef HAVE_SETLOCALE
	setlocale(LC_ALL, "");
#endif
#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif

	if (parse_argv(argc, argv, &url, &name)) {
		return(EXIT_FAILURE);
	}

	if (cinit(url, &hdl)) {
		return(EXIT_FAILURE);
	}

	if (query(hdl, name, &res)) {
		return(EXIT_FAILURE);
	}

	if (search(res)) {
		return(EXIT_FAILURE);
	}

	if (cfini(&hdl)) {
		return(EXIT_FAILURE);
	}

	if (url) {
		free(url);
		url = NULL;
	}

	return(EXIT_SUCCESS);
}

/**
 * Parse the command line arguments.
 *
 * \param[in] argc Number of command line arguments.
 * \param[in] argv Reference to the pointer to the argument array list.
 *
 * \retval 0 If there were no errors.
 **/
static int
parse_argv(int argc, char **argv, char **url, char **name)
{
	int opt = 0;
	int opt_index = 0;
	char *soptions = "hVvs:u:";             /* short options structure */
	static struct option loptions[] = {     /* long options structure */
		{"help",       no_argument,        NULL,  'h'},
		{"version",    no_argument,        NULL,  'V'},
		{"verbose",    no_argument,        NULL,  'v'},
		{"search",     required_argument,  NULL,  's'},
		{"url",        required_argument,  NULL,  'u'},
		{NULL,         1,                  NULL,  0}
	};

	/* set the default field to search */
	options.search = email;

	/* parse the arguments */
	while ((opt = getopt_long(argc, argv, soptions, loptions,
				  &opt_index)) != -1) {
		switch (opt) {
		case 'V':
			print_version();
			break;
		case 'h':
			print_usage();
			break;
		case 'v':
			options.verbose = 1;
			break;
		case 's':
			if (optarg[0] == 'a' ||
			    optarg[0] == 'A' ) {
				options.search = address;
			} else if (optarg[0] == 't' ||
				   optarg[0] == 'T' ) {
				options.search = telephone;
			}
			break;
		case 'u':
			*url = xmalloc(strlen(optarg +1) * sizeof(char));
			strcpy(*url, optarg);
			break;
		default:
			print_usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1) {
		warnx(_("Must specify a name to search for."));
		print_usage();
	}

	*name = (char *) xmalloc((strlen(argv[0]) +1) * sizeof(char));
	strcpy(*name, argv[0]);

	if (options.verbose) {
		fprintf(stderr, "%s options are:\n", program_name());
		fprintf(stderr, "  URL        : %s\n", *url);
		fprintf(stderr, "  Search name: %s\n", *name);
		fprintf(stderr, "  Query      : %s\n",
				sterm_name[options.search]);
	}

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
usage: %s [-h] [-V] [-v] [-a] [-p] [-r] [-t #] src dst\n\
  -h, --help       display this help and exit.\n\
  -V, --version    display version information and exit.\n\
  -v, --verbose    verbose mode.\n\
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
There is NO WARRANTY, to the extent permitted by law.\n\n"), "2014");
	printf(_("Compiled on %s at %s.\n\n"), __DATE__, __TIME__);
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
