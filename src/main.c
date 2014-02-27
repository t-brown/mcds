
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include <locale.h>
#include "gettext.h"

#include <getopt.h>

#include "defs.h"

/* Internal functions */
static void print_usage(void);
static void print_version(void);
static int  parse_argv(int, char **);
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

	/*
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	*/

	if (parse_argv(argc, argv)) {
		return(EXIT_FAILURE);
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
parse_argv(int argc, char **argv)
{
	int opt = 0;
	int opt_index = 0;
	char *soptions = "hVv";                 /* short options structure */
	static struct option loptions[] = {     /* long options structure */
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'V'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, 0}
	};
	char *tmp = NULL;       /* Temporary string for basenaem */
	char *dtmp = NULL;      /* Temporary string destination */
	char *ptr = NULL;       /* Pointer for basename */

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
			/*
			options.verbose = 1;
			*/
			break;
		default:
			print_usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc == 1) {
		warnx(_("Must specify a destination"));
		print_usage();
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
Copyright (C) %s Ontario Institute for Cancer Research.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\n"), "2009");
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
