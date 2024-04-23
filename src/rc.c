
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
 * \file rc.c
 * Routines to read a configuration/rc file.
 *
 * \ingroup rc
 * \{
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <locale.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gettext.h"
#include "defs.h"
#include "options.h"
#include "mem.h"

#ifndef LINE_MAX
#define LINE_MAX          sysconf(_SC_LINE_MAX)
#endif

/**
 * Read the rc file and parse it into the options.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
read_rc(const char *file)
{

	int i  = 0;                    /* Temporary loop indexer */
	int ln = 0;                    /* Line number */
	int len = 0;                   /* String length */
	char *home = NULL;             /* Home directory */
	char *pfile = NULL;            /* Password file */
	char *abs_file = NULL;         /* Absolute filename */
	FILE *ifd = NULL;              /* File descriptor */
	char line[LINE_MAX];           /* Read line from file */
	char *lptr = NULL;             /* Line pointer for strsep */
	char *tmp  = NULL;             /* Temporary pointer for read variables*/
	char *vals[2] = {0};           /* Key, value read from a line */
	struct stat buf = {0};         /* Stat information */

	static const char nfile[] = ".netrc";  /* Netrc file */
	static const char rfile[] = ".mcdsrc"; /* Default rc file */

	if (file == NULL) {
		home = getenv("HOME");
		if (home == NULL) {
			warnx(_("Unable to obtain home directory"));
			return(EXIT_FAILURE);
		}
		len = strlen(home) + strlen(rfile) + 2;
		abs_file = xmalloc(len*sizeof(char));
		if (snprintf(abs_file, len, "%s/%s", home, rfile) >= len) {
			warnx(_("Unable to build rc file string"));
			return(EXIT_FAILURE);
		}
	} else {
		abs_file = strdup(file);
		if (abs_file == NULL) {
			warn(_("Unable to duplicate string"));
			return(EXIT_FAILURE);
		}
	}

#ifdef HAVE_UNVEIL
	if (unveil(abs_file, "r") == -1) {
		warn(_("Unable to unveil %s"), abs_file);
		return(EXIT_FAILURE);
	}
#endif

	/* fail silently in case the user does not have a rc file */
	if (stat(abs_file, &buf) == -1) {
		if (errno == ENOENT) {
			return(EXIT_SUCCESS);
		}
	}

	if ((ifd = fopen(abs_file, "r")) == NULL) {
		return(EXIT_FAILURE);
	}

	while (fgets(line, LINE_MAX, ifd) != NULL) {
		++ln;
		lptr = line;
		i = 0;
		while ((tmp = strsep(&lptr, " \t=")) != NULL) {
			if (tmp[0] != '\0') {
				len = strlen(tmp);
				vals[i] = xmalloc(len +1);
				if (tmp[len-1] == '\n') {
					strncpy(vals[i], tmp, len-1);
				} else {
					strncpy(vals[i], tmp, len);
				}
				++i;
				if (i == 2) {
					break;
				}
			}
		}
		if (strncmp("url", vals[0], 3) == 0) {
			if (options.url == NULL) {
				len = strlen(vals[1]);
				options.url = xmalloc(len+1);
				strncpy(options.url, vals[1], len);
			}
		} else if (strncmp("verify", vals[0], 7) == 0) {
			if ((vals[1][0] == 'y') || (vals[1][0] == 'Y')) {
				options.verify = 1;
			} else {
				options.verify = 0;
			}
		} else if (strncmp("netrc", vals[0], 5) == 0) {
			if ((vals[1][0] == 'y') || (vals[1][0] == 'Y')) {
				options.netrc = 1;
			} else {
				options.netrc = 0;
			}
		} else if (strncmp("password_file", vals[0], 13) == 0) {
			len = strlen(vals[1]);
			pfile = xmalloc(len+1);
			strncpy(pfile, vals[1], len);
		} else if (strncmp("username", vals[0], 8) == 0) {
			if (options.username == NULL) {
				len = strlen(vals[1]);
				options.username = xmalloc(len+1);
				strncpy(options.username, vals[1], len);
			}
		}
		if (vals[0]) {
			free(vals[0]);
			vals[0] = NULL;
		}
		if (vals[1]) {
			free(vals[1]);
			vals[1] = NULL;
		}
	}

	if (fclose(ifd)) {
		warn(_("Unable to close %s"), abs_file);
	}

	if (abs_file) {
		free(abs_file);
		abs_file = NULL;
	}

	if (pfile) {
#if HAVE_GPGME == 1
		if (pfile[0] == '~' && pfile[1] == '/') {
			home = getenv("HOME");
			if (home == NULL) {
				warnx(_("Unable to obtain home directory"));
				return(EXIT_FAILURE);
			}
			len = strlen(home) + strlen(pfile);
			abs_file = xmalloc(len*sizeof(char));
			if (snprintf(abs_file, len, "%s/%s", home, pfile +2) >= len) {
				warnx(_("Unable to build password file string"));
				return(EXIT_FAILURE);
			}
		} else {
			abs_file = strdup(pfile);
			if (abs_file == NULL) {
				warn(_("Unable to duplicate password file string"));
				return(EXIT_FAILURE);
			}
		}
#ifdef HAVE_UNVEIL
		if (unveil(abs_file, "r") == -1) {
			warn(_("Unable to unveil %s"), abs_file);
			return(EXIT_FAILURE);
		}
#endif
		if (pfile) {
			free(pfile);
			pfile = NULL;
		}

		if (abs_file) {
			/* Defer password file access so that errors don't
			 * prevent processing of options like --version and
			 * --help which should not be affected by
			 * configuration errors. */
			options.password_file = abs_file;
			abs_file = NULL;
		}
#else
		if (options.verbose == 1) {
			warnx(_("Encrypted password files are not supportred, ignoring."));
		}
#endif
	}

#ifdef HAVE_UNVEIL
	if (options.verify == 1) {
		if (unveil("/etc/ssl", "r") == -1) {
			warn(_("Unable to unveil %s"), "/etc/ssl/");
			return(EXIT_FAILURE);
		}
	}
	if (options.netrc == 1) {
		home = getenv("HOME");
		if (home == NULL) {
			warnx(_("Unable to obtain home directory"));
			return(EXIT_FAILURE);
		}
		len = strlen(home) + strlen(nfile) + 2;
		abs_file = xmalloc(len*sizeof(char));
		if (snprintf(abs_file, len, "%s/%s", home, nfile) >= len) {
			warnx(_("Unable to build password file string"));
			return(EXIT_FAILURE);
		}
		if (unveil(abs_file, "r") == -1) {
			warn(_("Unable to unveil %s"), abs_file);
			return(EXIT_FAILURE);
		}
		if (abs_file) {
			free(abs_file);
			abs_file = NULL;
		}
	}
#endif

	return(EXIT_SUCCESS);
}

/**
 * \}
 **/
