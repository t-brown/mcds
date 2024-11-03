/*
 * Copyright (C) 2014 Timothy Brown
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file options.h
 * Internal definitions for program options.
 *
 * \ingroup options
 * \{
 **/

#ifndef MCDS_OPTIONS_H
#define MCDS_OPTIONS_H

#ifdef __cplusplus
extern "C"
{
#endif

#define STERMS_TABLE            \
	X(name,        "FN")    \
	X(email,       "EMAIL") \
	X(address,     "ADR")   \
	X(telephone,   "TEL")

#define X(a, b) a,
enum s_terms {
	STERMS_TABLE
};
#undef X

/** Program command line options **/
struct opts {
	int verbose;
	int verify;
	int netrc;
	int pwprompt;
	int libsecret;
	int save;
	enum s_terms query;
	enum s_terms search;
	char *url;
	char *term;
	char *username;
	char *password;
};

/** Extern declarations **/
extern struct opts options;
extern char *sterm_name[];

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* MCDS_OPTIONS_H */
/**
 * \}
 **/
