
/*
 * Copyright (C) 2019  Timothy Brown
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
 * \file decrypt.c
 * Routines to decrypt the password file.
 *
 * \ingroup decrypt
 * \{
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <locale.h>
#include <fcntl.h>
#include <unistd.h>
#include <gpgme.h>
#include "gettext.h"
#include "defs.h"
#include "options.h"
#include "mem.h"

#ifndef LINE_MAX
#define LINE_MAX          sysconf(_SC_LINE_MAX)
#endif

/**
 * Decrypt the password file and save the password
 * in the options structure.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
decrypt(char *filename)
{
	int fd = 0;              /* Password file descriptor */
	size_t ret = 0;          /* Number of bytes read from buffer */
	size_t total = 0;        /* Total number of bytes read */
	gpgme_error_t err = {0}; /* GPGME error type */
	gpgme_ctx_t ctx = {0};   /* GPGME control context */
	gpgme_data_t in = {0};   /* GPGME encrypted input */
	gpgme_data_t out = {0};  /* GPGME decrypted output */
	char tmp[LINE_MAX];      /* GPGME data read */

	gpgme_check_version(NULL);
	if ((err = gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP)) != 0) {
		warnx(_("Unable to initalize GPGME: %s"), gpgme_strerror(err));
		return(EXIT_FAILURE);
	}

	if ((fd = open(filename, O_RDONLY)) < 0) {
		warn(_("Unable to open password file: %s"), filename);
		return(EXIT_FAILURE);
	}

	if ((err = gpgme_new(&ctx)) != 0) {
		warnx(_("Unable to create new context: %s"),
			gpgme_strerror(err));
		return(EXIT_FAILURE);
	}

	if ((err = gpgme_data_new_from_fd(&in, fd)) != 0) {
		warnx(_("Unable to get encrypted password: %s"),
			gpgme_strerror(err));
		return(EXIT_FAILURE);
	}

	if ((err = gpgme_data_new(&out)) != 0) {
		warnx(_("Unable to create GPGME data: %s"),
			gpgme_strerror(err));
		return(EXIT_FAILURE);
	}

	if ((err = gpgme_op_decrypt(ctx, in, out)) != 0) {
		warnx(_("Unable to decrypt password: %s"), gpgme_strerror(err));
		return(EXIT_FAILURE);
	}

	if ((err = gpgme_data_seek(out, 0, SEEK_SET)) !=0) {
		warnx(_("Unable to seek: %s"), gpgme_strerror(err));
		return(EXIT_FAILURE);
	}

	while ((ret = gpgme_data_read(out, tmp, LINE_MAX)) > 0) {
		total += ret;
		options.password = realloc(options.password, total);
		if (!options.password) {
			warn(_("Unable to realloc password."));
			return(EXIT_FAILURE);
		}
		memcpy(&(options.password[total - ret]), tmp, ret);
	}
	options.password[total-1] = '\0';

	if (close(fd) < 0) {
		warn(_("Unable to close password file."));
		return(EXIT_FAILURE);
	}

	gpgme_data_release(in);
	gpgme_data_release(out);
	gpgme_release(ctx);

	return(EXIT_SUCCESS);
}

/**
 * \}
 **/
