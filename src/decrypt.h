/*
 * Copyright (C) 2019 Timothy Brown
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
 * \file decrypt.h
 * Internal definitions for decrypting the password file.
 *
 * \ingroup rc
 * \{
 **/

#ifndef MCDS_DECRYPT_H
#define MCDS_DECRYPT_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Decrypt the password file */
int decrypt(char *);

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* MCDS_DECRYPT_H */
/**
 * \}
 **/
