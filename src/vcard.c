
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
 * \file carddav.c
 * Routines to query and search a vcard.
 *
 * \ingroup carddav
 * \{
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <regex.h>
#include <locale.h>
#include "gettext.h"
#include "defs.h"
#include "options.h"
#include "mem.h"
#include "vcard.h"

/**
 * Search a query's result. This will run regexs over the result
 * to filter the data.
 * The first regex will be to obtain the name (FN property).
 * While the second one will be to find all requested fields.
 *
 * It will print all matches found to stdout.
 *
 * \parm[in] card The vcard.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
search(const char *card)
{

	int rerr = 0;			/* Regex error code */
	size_t rlen = 0;		/* Regex error string length */
	char *rstr = NULL;		/* Regex error string */

	regex_t fnr;			/* Precompiled fn regex */
	regmatch_t fnm[2];		/* Regex fn pattern match */
	const char fnp[] = "FN:(.*)";	/* Regex fn pattern */

	regex_t flr;			/* Precompiled vcard regex */
	regmatch_t flm[3];		/* Regex pattern match */
	const char fr[] = "^%s([A-Za-z;=])+:(.*)"; /* Regex pattern */
	char *flp  = NULL;		/* Filled in regex pattern */
	size_t fln = 0;			/* Size of initial field regex */

	size_t len = 0;			/* Length of the name */
	char *name = NULL;		/* Name field */
	size_t flen = 0;		/* Length of the field */

	/* Create the regex pattern for the field */
	fln += strlen(fr) + strlen(sterm_name[options.search]) - 1;
	flp = xmalloc((fln)*sizeof(char));
	if (snprintf(flp, fln, fr, sterm_name[options.search]) >= fln) {
		warnx(_("Unable to build regex pattern."));
		return(EXIT_FAILURE);
	}

	/* Compile the regex pattern for the field */
	if ((rerr = regcomp(&flr, flp, REG_EXTENDED|REG_NEWLINE)) != 0) {
		rlen = regerror(rerr, &flr, NULL, 0);
		rstr = xmalloc((rlen+1)*sizeof(char));
		regerror(rerr, &flr, rstr, rlen);
		warnx(_("Unable to compile regex '%s': %s\n"), flp, rstr);
		if (rstr) {
			free(rstr);
			rstr = NULL;
		}
		return(EXIT_FAILURE);
	}

	/* Compile the regex pattern for the full name */
	if ((rerr = regcomp(&fnr, fnp, REG_EXTENDED|REG_NEWLINE)) != 0) {
		rlen = regerror(rerr, &fnr, NULL, 0);
		rstr = xmalloc((rlen+1)*sizeof(char));
		regerror(rerr, &fnr, rstr, rlen);
		warnx(_("Unable to compile regex '%s': %s\n"), fnp, rstr);
		if (rstr) {
			free(rstr);
			rstr = NULL;
		}
		return(EXIT_FAILURE);
	}

	/* Find the name */
	rerr = regexec(&fnr, &card[0], 2, fnm, 0);
	if (rerr != 0) {
		goto rtn;
	}

	len = (int)(fnm[1].rm_eo - fnm[1].rm_so);
	name = xmalloc(len+1);
	memcpy(name, card+fnm[1].rm_so, len);
	if (name[len-1] == '\r') {
		name[len-1] = '\0';
	} else {
		name[len] ='\0';
	}

	/* Grab the field we wanted */
	rerr = regexec(&flr, card, 3, flm, 0);
	while (rerr == 0) {
		/* For addresses convert ";" to "\n" */
		flen = flm[2].rm_eo - flm[2].rm_so;
		if (card[flm[2].rm_eo -1] == '\r') {
			flen -= 1;
		}
		printf("%.*s\t%s\n", (int)flen, card + flm[2].rm_so, name);

		card += flm[0].rm_eo;
		rerr = regexec(&flr, card, 3, flm, REG_NOTBOL);
	}

rtn:
	if (rstr) {
		free(rstr);
		rstr = NULL;
	}
	if (name) {
		free(name);
		name = NULL;
	}
	if (flp) {
		free(flp);
		flp = NULL;
	}
	regfree(&flr);
	regfree(&fnr);
	return(rerr);
}

/**
 * \}
 **/
