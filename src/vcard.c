
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
	/* Regex patterns */
	static const char r[] = "%s(.*):(.*)";     /* Whole result */
	static const char t[] = "^%s([A-Za-z;=])*:(.*%s.*)"; /* Query term  */

	int plen = 0;			/* Length of snprintf()'s */

	int rerr = 0;			/* Regex error code */
	size_t rlen = 0;		/* Regex error string length */
	char *rstr = NULL;		/* Regex error string */

	size_t qlen = 0;		/* Length of the query string */
	char *q = NULL;			/* Regex pattern for query */
	char *qt = NULL;		/* Quoted query term */
	regex_t rq = {0};		/* Regex precompiled query */
	char *qres = NULL;		/* Result of the query */

	size_t slen = 0;		/* Length of the search string */
	char *s = NULL;			/* Regex pattern for search */
	regex_t rs = {0};		/* Regex precompiled search */

	regmatch_t match[3] = {0};	/* Regex matches */

	/* Generate a quoted query term */
	if (quote(options.term, &qt)) {
		warnx(_("Unable to build quoted term."));
		return(EXIT_FAILURE);
	}

	/* Compile the regex for the query */
	qlen = strlen(t) -4
		+ strlen(sterm_name[options.query])
		+ strlen(qt) +1;
	q = xmalloc(qlen*sizeof(char));

	plen = snprintf(q, qlen, t, sterm_name[options.query], qt);
	if (plen < 0 || (size_t)plen != qlen -1) {
		warnx(_("Unable to build regex pattern."));
		return(EXIT_FAILURE);
	}

	if ((rerr = regcomp(&rq, q, REG_EXTENDED|REG_NEWLINE|REG_ICASE)) != 0) {
		rlen = regerror(rerr, &rq, NULL, 0);
		rstr = xmalloc((rlen+1)*sizeof(char));
		regerror(rerr, &rq, rstr, rlen);
		warnx(_("Unable to compile regex '%s': %s\n"), q, rstr);
		if (rstr) {
			free(rstr);
			rstr = NULL;
		}
		return(EXIT_FAILURE);
	}

	/* Compile the regex for the search */
	slen = strlen(r) -2
		+ strlen(sterm_name[options.search]) +1;
	s = xmalloc((slen)*sizeof(char));

	plen = snprintf(s, slen, r, sterm_name[options.search]);
	if (plen < 0 || (size_t)plen != slen -1) {
		warnx(_("Unable to build regex pattern."));
		return(EXIT_FAILURE);
	}

	if ((rerr = regcomp(&rs, s, REG_EXTENDED|REG_NEWLINE)) != 0) {
		rlen = regerror(rerr, &rs, NULL, 0);
		rstr = xmalloc((rlen+1)*sizeof(char));
		regerror(rerr, &rs, rstr, rlen);
		warnx(_("Unable to compile regex '%s': %s\n"), s, rstr);
		if (rstr) {
			free(rstr);
			rstr = NULL;
		}
		return(EXIT_FAILURE);
	}

	if (options.verbose) {
		fprintf(stderr, "Regex for query term: %s\n", q);
		fprintf(stderr, "Regex for search term: %s\n", s);
	}

	/* Look for the query term in the original card */
	rerr = regexec(&rq, &card[0], 3, match, 0);
	if (rerr != 0) {
		goto rtn;
	}

	qlen = (int)(match[2].rm_eo - match[2].rm_so);
	qres = xmalloc(qlen+1);
	memcpy(qres, card+match[2].rm_so, qlen);
	if (qres[qlen-1] == '\r') {
		qres[qlen-1] = '\0';
	} else {
		qres[qlen] ='\0';
	}

	/* Grab all the fields that we wanted */
	rerr = regexec(&rs, card, 3, match, 0);
	while (rerr == 0) {
		/* TODO: For addresses convert ";" to "\n" */
		slen = match[2].rm_eo - match[2].rm_so;
		if (card[match[2].rm_eo -1] == '\r') {
			slen -= 1;
		}
		printf("%.*s\t%s\n", (int)slen, card + match[2].rm_so, qres);

		card += match[0].rm_eo;
		rerr = regexec(&rs, card, 3, match, REG_NOTBOL);
	}

	regfree(&rq);
	regfree(&rs);

rtn:
	if (qres) {
		free(qres);
		qres = NULL;
	}
	if (qt) {
		free(qt);
		qt = NULL;
	}

	return(rerr);
}

/**
 * Generate a quoted string for regex's.
 *
 * \param[in]  term   The search term unquoted.
 * \param[out] quoted The search term quoted for regex.
 *
 * \retval 0 If there were no errors.
 **/
int
quote(const char *term, char **quoted)
{
	int i = 0;
	int j = 0;
	int len = 0;

	len = strlen(term);
	j = len;

	/* find out how many extra characters we might need */
	for (i = 0; i < len; ++i) {
		switch (term[i]) {
			case '(':
			case ')':
			case '+':
			case '-':
			case '.':
			case ' ':
				++j;
				break;
		}
	}

	/* create the quoted string */
	(*quoted) = xmalloc((j+1) * sizeof(char));
	j = 0;
	for (i = 0; i < len; ++i) {
		switch (term[i]) {
			case '(':
			case ')':
			case '+':
			case '-':
			case '.':
			case ' ':
				(*quoted)[j] = '\x5c';
				(*quoted)[++j] = term[i];
				break;
			default:
				(*quoted)[j] = term[i];
				break;
		}
		++j;
	}

	return(EXIT_SUCCESS);
}
/**
 * \}
 **/
