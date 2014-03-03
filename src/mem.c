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
 * \file mem.c
 * Memory allocation and deallocation routines.
 *
 * \ingroup memory
 * \{
 **/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sysexits.h>
#include <string.h>

#include "gettext.h"
#include "defs.h"
#include "mem.h"

/**
 * Allocate a block of memory and set all entries to zero.
 * If there is an error in obtaining the memory err()
 * is called, terminating the program.
 *
 * \param[in] n The amount of memory in bytes.
 *
 * \return A pointer to the newly allocated memory.
 **/
ATT_MSIZE(1)
ATT_MALLOC
void *
xmalloc(size_t n)
{
  void *ptr = NULL;		/* New pointer to memory location */

  ptr = (void *) malloc(n);
  if (ptr) {
    memset(ptr, 0, n);
    return ptr;
  } else {
    errx(EX_SOFTWARE,_("out of memory (unable to allocate %ld bytes)"), n);
  }
  /* should never get here */
  return NULL;
}

/**
 * \}
 **/
