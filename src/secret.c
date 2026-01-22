/*
 * Copyright (C) 2024  Andrew Bower
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
 * \file secret.c
 * Routines to manage access to credential store.
 *
 * \ingroup secret
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
#include <termios.h>
#include "gettext.h"
#include "defs.h"
#include "mem.h"
#include "options.h"
#include "prompt.h"
#include "secret.h"

#if HAVE_LIBSECRET
#include <libsecret/secret.h>

#define MCDS_NAMESPACE "com.github.t-brown.mcds"
#define MCDS_SECRET_SCHEMA_NAME MCDS_NAMESPACE ".password"
#define MCDS_SECRET_KEY_URL "url"
#define MCDS_SECRET_KEY_USER "user"

static const SecretSchema mcds_secret_schema = {
	.name = MCDS_SECRET_SCHEMA_NAME,
	.flags = SECRET_SCHEMA_NONE,
	.attributes = {
		{ MCDS_SECRET_KEY_URL,  SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ MCDS_SECRET_KEY_USER, SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ "NULL", 0 }
	}
};

int
store_password(void)
{
	GError *error = NULL;

	secret_password_store_sync(&mcds_secret_schema,
				   SECRET_COLLECTION_DEFAULT,
				   "Mutt CardDAV Search user credentials",
				   options.password,
				   NULL, &error,
				   MCDS_SECRET_KEY_URL, options.url,
				   MCDS_SECRET_KEY_USER, options.username,
				   NULL);
	if (error) {
		warnx(_("error storing password with libsecret: %s"), error->message);
		g_error_free(error);
		return(EXIT_FAILURE);
	}
	return(EXIT_SUCCESS);
}

int
lookup_password(void)
{
	GError *error = NULL;

	gchar *password = secret_password_lookup_sync(&mcds_secret_schema,
						      NULL, &error,
						      MCDS_SECRET_KEY_URL, options.url,
						      MCDS_SECRET_KEY_USER, options.username,
						      NULL);
	if (error) {
		warnx(_("error retrieving password with libsecret: %s"), error->message);
		g_error_free(error);
		return(EXIT_FAILURE);
	}

	if (password) {
		options.password = strdup(password);
	}
	secret_password_free(password);
	return(EXIT_SUCCESS);
}

int
clear_password(void)
{
	GError *error = NULL;

	gboolean removed = secret_password_clear_sync(&mcds_secret_schema,
						      NULL, &error,
						      MCDS_SECRET_KEY_URL, options.url,
						      MCDS_SECRET_KEY_USER, options.username,
						      NULL);
	if (error) {
		/* This is a non-fatal condition. */
		warnx(_("error clearing password with libsecret: %s"), error->message);
		g_error_free(error);
	}
	return(!error && removed ? EXIT_SUCCESS : EXIT_FAILURE);
}
#endif


/**
 * \}
 **/
