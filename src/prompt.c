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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * \file prompt.c
 * Routine to prompt for a password.
 *
 * \ingroup prompt
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
#include "options.h"
#include "mem.h"
#include "prompt.h"


int
prompt_password(void)
{
	struct termios tios_save;
	struct termios tios;
	FILE *console;
	char *password = NULL;
	int console_fd;
	ssize_t len;
	size_t sz;

	/* Open controlling terminal */
	console = fopen("/dev/tty", "w+");
	if (console == NULL) {
		warn(_("Could not open console for password prompt"));
		return(EXIT_FAILURE);
	}
	console_fd = fileno(console);

	/* Output prompt */
	fprintf(console, _("mcds: password for %s at %s: "),
		options.username, options.url);
	fflush(console);

	/* Do not echo password */
	if (tcgetattr(console_fd, &tios_save) != 0) {
		warn(_("Could not turn off echo for password prompt"));
		fclose(console);
		return(EXIT_FAILURE);
	}
	tios = tios_save;
	tios.c_lflag &= ~ECHO;
	if (tcsetattr(console_fd, TCSAFLUSH, &tios) != 0) {
	  warn(_("Could not turn off echo for password prompt"));
		fclose(console);
		return(EXIT_FAILURE);
	}

	/* Read password */
	len = getline(&password, &sz, console);
	fprintf(console, "\n");
	fflush(console);
	if (len < 1) {
		free(password); /* Yes, getline(3) says to do this! */
		warn(_("Could not read password"));
	} else {
		/* Remove line ending */
		password[len - 1] = '\0';
		options.password = password;
	}

	/* Restore echo state */
	tcsetattr(console_fd, TCSAFLUSH, &tios_save);
	fclose(console);

	return(EXIT_SUCCESS);
}

/**
 * \}
 **/
