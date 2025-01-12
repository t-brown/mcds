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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * \file defs.h
 * Internal definitions.
 *
 * \ingroup defs
 * \{
 **/

#ifndef MCDS_DEFS_H
#define MCDS_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define _(x)              gettext(x)


/**
 * Compiler __attribute__ extensions
 **/
#ifdef HAVE___ATTRIBUTE__
#define ATT_CONSTR       __attribute__((__constructor__))
#define ATT_DESTR        __attribute__((__destructor__))
#define ATT_PUBLIC       __attribute__((__visibility__("default")))
#define ATT_LOCAL        __attribute__((__visibility__("hidden")))
#define ATT_DEPRECATED   __attribute__((__deprecated__))
#define ATT_MSIZE(x)     __attribute__((__alloc_size__(x)))
#define ATT_MALLOC       __attribute__((__malloc__))
#define ATT_FMT(x,y)     __attribute__((__format__(printf, x, y)))
#define ATT_NORETURN     __attribute__((__noreturn__))
#define ATT_INLINE       __attribute__((__always_inline__))
#define ATT_ALIAS(x)     __attribute__((__weak__, __alias__(x)))
#else
#define ATT_CONSTR
#define ATT_DESTR
#define ATT_PUBLIC
#define ATT_LOCAL
#define ATT_DEPRECATED(msg)
#define ATT_MSIZE(x)
#define ATT_MALLOC
#define ATT_FMT(x,y)
#define ATT_NORETURN
#define ATT_INLINE
#define ATT_ALIAS(x)
#endif


#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* MCDS_DEFS_H */
/**
 * \}
 **/
