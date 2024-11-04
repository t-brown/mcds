/*
 * Copyright (C) 2024 Andrew Bower
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
 * \file secret.h
 * Internal definitions for accessing credential store.
 *
 * \ingroup secret
 * \{
 **/

#ifndef MCDS_SECRET_H
#define MCDS_SECRET_H

#if HAVE_LIBSECRET
#include <libsecret/secret.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/** Save password in user's credential store */
int store_password(void);

/** Lookup password in user's credential store */
int lookup_password(void);

/** Clear password in user's credential store */
int clear_password(void);

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* MCDS_SECRET_H */
/**
 * \}
 **/
