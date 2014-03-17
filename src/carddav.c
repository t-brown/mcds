
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

/** Curl response data structure **/
struct r_data {
	size_t size;
	char * data;
};

/** Search callback fuction **/
static size_t query_cb(void *, size_t, size_t, void *);

/** Search string **/
static const char sterm[] =
"<?xml version='1.0' encoding='utf-8' ?>"
"<C:addressbook-query xmlns:D='DAV:' xmlns:C='urn:ietf:params:xml:ns:carddav'>"
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

	size_t len = 0;
	char *s = NULL;
	CURLcode res = CURLE_OK;
	struct curl_slist *hdrs = NULL;
	struct r_data buffer;

	if (*result != NULL) {
		warnx(_("Will not to write results to non-null pointer."));
		return(EXIT_FAILURE);
	}

	len = strlen(sterm) + strlen(sterm_name[options.search])
		+ strlen(name) +1;
	s = xmalloc(len*sizeof(char));
	if (snprintf(s, len, sterm, sterm_name[options.search], name) >= len) {
		warnx(_("Unable to build search string."));
		return(EXIT_FAILURE);
	}

	/* Response buffer, will be realloc'ed by the call back */
	buffer.data = xmalloc(1);
	buffer.size = 0;

	if (options.verbose) {
		fprintf(stderr, "  Sending    :\n%s\n", s);
	}

	hdrs = curl_slist_append(hdrs, "Content-Type: text/xml");
	hdrs = curl_slist_append(hdrs, "Depth: 1");

	curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, "REPORT");
	curl_easy_setopt(hdl, CURLOPT_POSTFIELDS, s);
	curl_easy_setopt(hdl, CURLOPT_HTTPHEADER, hdrs);
	curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, query_cb);
	curl_easy_setopt(hdl, CURLOPT_WRITEDATA, (void *)&buffer);

	res = curl_easy_perform(hdl);
	if (res != CURLE_OK) {
		warnx(_("Unable to search for %s: %s"),
				name, curl_easy_strerror(res));
		return(EXIT_FAILURE);
	}
	/* Write out a blank line for mutt */
	printf("\n");

	if (buffer.size == 0) {
		warnx(_("Unable to obtain a result."));
		return(EXIT_FAILURE);
	}
	*result = xmalloc(buffer.size+1);
	memcpy(*result, buffer.data, buffer.size);
	(*result)[buffer.size] = '\0';

	if (options.verbose) {
		fprintf(stderr, "Retrieved:\n======\n%s\n======\n", *result);
	}

	curl_slist_free_all(hdrs);

	if (s) {
		free(s);
		s = NULL;
	}

	if (buffer.data) {
		free(buffer.data);
		buffer.data = NULL;
		buffer.size = 0;
	}

	return(EXIT_SUCCESS);
}


/**
 * Query's callback routine. That gets called when curl has
 * a response.
 *
 * Note curl normally writes out every 16k (as defined
 * by CURL_MAX_WRITE_SIZE in curl.h). So if the response is
 * larger than 16k, this callback will be called multiple
 * times.
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
	size_t len = 0;
	struct r_data *res = (struct r_data *)mem;

	len = size * nmemb;
	res->data = realloc(res->data, res->size + len + 1);
	if (res->data == NULL) {
		warn(_("Unable to extend the response data buffer"));
		return(0);
	}

	memcpy(&(res->data[res->size]), contents, len);
	res->size += len;
	res->data[res->size] = '\0';

	return(len);

}

/**
 * \}
 **/
