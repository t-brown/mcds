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
 * \file mem.h
 * Internal definitions for memory allocation and deallocation.
 *
 * \ingroup memory
 * \{
 **/

#ifndef MCDS_MEM_H
#define MCDS_MEM_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Allocate a block of memory */
void * xmalloc(size_t);

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* MCDS_MEM_H */
/**
 * \}
 **/
