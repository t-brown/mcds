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
 * \file prompt.h
 * Internal definitions for password prompting.
 *
 * \ingroup secret
 * \{
 **/

#ifndef MCDS_PROMPT_H
#define MCDS_PROMPT_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Prompt for user's password */
int prompt_password(void);

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* MCDS_PROMPT_H */
/**
 * \}
 **/
