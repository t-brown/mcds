
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
#include <curl/curl.h>
#include <regex.h>
#include <locale.h>
#include "gettext.h"
#include "defs.h"
#include "options.h"
#include "mem.h"
#include "carddav.h"

/** Search callback fuction **/
static size_t query_cb(void *, size_t, size_t, void *);

/** Search string **/
static const char sterm[] =
"<?xml version='1.0' encoding='utf-8' ?>"
"<C:addressbook-query xmlns:D='DAV:'xmlns:C='urn:ietf:params:xml:ns:carddav'>"
"<D:prop><C:address-data><C:prop name='FN'/><C:prop name='%s'/>"
"</C:address-data></D:prop>"
"<C:filter test='anyof'><C:prop-filter name='FN'>"
"<C:text-match collation='i;unicode-casemap' match-type='contains'>%s"
"</C:text-match></C:prop-filter></C:filter></C:addressbook-query>";

/**
 * Query for a name from the carddav server.
 *
 * \parm[in] hdl     Curl handle.
 * \parm[in] name    The name to search for.
 * \parm[out] result The results from the query.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
query(CURL *hdl, const char *name, char **result)
{

	char *s = NULL;
	CURLcode res = 0;
	struct curl_slist *hdrs = NULL;

	if (*result != NULL) {
		warnx(_("Will not to write results to non-null pointer."));
		return(EXIT_FAILURE);
	}

	if (asprintf(&s, sterm, sterm_name[options.search], name) == -1) {
		warnx(_("Unable to build search string."));
		return(EXIT_FAILURE);
	}

	if (options.verbose) {
		fprintf(stderr, "Searching for:\n%s\n", s);
	}

	hdrs = curl_slist_append(hdrs, "Content-Type: text/xml");
	hdrs = curl_slist_append(hdrs, "Brief:t");

	curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, "REPORT");
	curl_easy_setopt(hdl, CURLOPT_POSTFIELDS, s);
	curl_easy_setopt(hdl, CURLOPT_HTTPHEADER, hdrs);
	curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, query_cb);
	curl_easy_setopt(hdl, CURLOPT_WRITEDATA, result);

	res = curl_easy_perform(hdl);
	if (res != CURLE_OK) {
		warnx(_("Unable to search for %s: %s"),
				name, curl_easy_strerror(res));
		return(EXIT_FAILURE);
	}
	/* Write out a blank line for mutt */
	printf("\n");

	if (options.verbose) {
		fprintf(stderr, "Retrieved: \n%s\n", *result);
	}

	curl_slist_free_all(hdrs);

	if (s) {
		free(s);
		s = NULL;
	}

	return(EXIT_SUCCESS);
}


/**
 * Query's callback routine. That gets called when curl has
 * a response.
 *
 * \parm[in] contents The contents received by curl.
 * \parm[in] size     The size of a member returned.
 * \parm[in] nmemb    The number of members returned.
 * \parm[out] mem     The contents copied back as a string.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
static
size_t
query_cb(void *contents, size_t size, size_t nmemb, void *mem)
{
	char **res = (char **)mem;
	size_t len = 0;

	len = size * nmemb;
	*res = xmalloc(len);
	memcpy(*res, contents, len);
	(*res)[len] = '\0';

	return(len);

}

/**
 * Search a query's result. This will run a regex with the options
 * search field to print to stdout all the matches.
 *
 * \parm[in] res  The query result.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
search(const char *res)
{

	int rerr = 0;			/* Regex error code */
	size_t rlen = 0;		/* Regex error string length */
	char *rstr = NULL;		/* Regex error string */

	regex_t fn;			/* Precompiled full name regex */
	regmatch_t fpm[3];		/* Regex pattern match */
	const char fpt[] = "FN:(.*)\r"; /* Regex patter for full name */

	regex_t vcard;			/* Precompiled vcard regex */
	regmatch_t vpm[3];		/* Regex pattern match */
	char *vpt  = NULL;		/* Regex pattern */


	/* Compile the regex pattern for the full name */
	if ((rerr = regcomp(&fn, fpt, REG_EXTENDED|REG_NEWLINE)) != 0) {
		rlen = regerror(rerr, &fn, NULL, 0);
		rstr = xmalloc((rlen+1)*sizeof(char));
		regerror(rerr, &fn, rstr, rlen);
		warnx(_("Unable to compile regex '%s': %s\n"), fpt, rstr);
		if (rstr) {
			free(rstr);
			rstr = NULL;
		}
		return(EXIT_FAILURE);
	}

	/* Create the regex pattern */
	if (asprintf(&vpt, "^%s([A-Za-z;=])+:(.*)\r",
				sterm_name[options.search]) == -1) {
		warnx(_("Unable to build regex pattern."));
		return(EXIT_FAILURE);
	}

	/* Compile the regex pattern */
	if ((rerr = regcomp(&vcard, vpt, REG_EXTENDED|REG_NEWLINE)) != 0) {
		rlen = regerror(rerr, &vcard, NULL, 0);
		rstr = xmalloc((rlen+1)*sizeof(char));
		regerror(rerr, &vcard, rstr, rlen);
		warnx(_("Unable to compile regex '%s': %s\n"), vpt, rstr);
		if (rstr) {
			free(rstr);
			rstr = NULL;
		}
		return(EXIT_FAILURE);
	}

	/* Grab the full name */
	rerr = regexec(&fn, &res[0], 2, fpm, 0);
	if (rerr != 0) {
		regfree(&fn);
		regfree(&vcard);
		if (vpt) {
			free(vpt);
			vpt = NULL;
		}
		return(EXIT_FAILURE);
	}

	/* Grab the field we wanted */
	rerr = regexec(&vcard, &res[0], 3, vpm, REG_NOTBOL);

	while (rerr == 0) {
		printf("%.*s\t%.*s\n",
				(int)(vpm[2].rm_eo - vpm[2].rm_so),
				res + vpm[2].rm_so,
				(int)(fpm[1].rm_eo - fpm[1].rm_so),
				res + fpm[1].rm_so);
		res += vpm[0].rm_eo;
		rerr = regexec(&fn, res, 2, fpm, REG_NOTBOL);
		rerr = regexec(&vcard, res, 3, vpm, REG_NOTBOL);
	}

	/* For addresses convert ";" to "\n" */

	regfree(&fn);
	regfree(&vcard);

	if (vpt) {
		free(vpt);
		vpt = NULL;
	}
	return(EXIT_SUCCESS);
}

/**
 * \}
 **/
