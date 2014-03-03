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
 * \file curl.c
 * Routines to interact with curl.
 *
 * \ingroup curl
 * \{
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <curl/curl.h>
#include <locale.h>
#include "gettext.h"
#include "defs.h"
#include "curl.h"

/**
 * Initalise a curl handle.
 *
 * \parm[in]  url The URL to connect to.
 * \parm[out] hdl The curl handle.
 *
 * \retval 0 If it was sucessful.
 * \retval 1 If the initialization failed.
 **/
int
cinit(const char *url, CURL **hdl)
{
	CURLcode res = 0;       /* curl error response code */

	if (*hdl != NULL) {
		warnx(_("Unable to initialize non-null curl handle."));
		return(EXIT_FAILURE);
	}

	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (res) {
		warnx(_("Unable to initialize curl."));
		return(EXIT_FAILURE);
	}

	*hdl = curl_easy_init();
	if (*hdl == NULL) {
		warnx(_("Unable to initialize curl handle."));
		return(EXIT_FAILURE);
	}

	res = curl_easy_setopt(*hdl, CURLOPT_URL, url);
	res = curl_easy_setopt(*hdl, CURLOPT_SSL_VERIFYPEER, 0L);
	res = curl_easy_setopt(*hdl, CURLOPT_NETRC, 1L);
	res = curl_easy_setopt(*hdl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);

	return(EXIT_SUCCESS);
}

/**
 * Finalise a curl handle.
 *
 * \parm[in] hdl The curl handle.
 *
 * \retval 0 If it was sucessful.
 * \retval 1 If the initialization failed.
 **/
int
cfini(CURL **hdl)
{
	CURLcode res = 0;       /* curl error response code */

	if (*hdl == NULL) {
		warnx(_("Unable to finalise null curl handle."));
		return(EXIT_FAILURE);
	}

	curl_easy_cleanup(*hdl);
	curl_global_cleanup();

	return(EXIT_SUCCESS);
}

/**
 * \}
 **/
