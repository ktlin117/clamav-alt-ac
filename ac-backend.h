/*
 *  Authors: Kevin Lin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */

#ifndef __AC_BACKEND_H__
#define __AC_BACKEND_H__

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include "perf.h"

/* dense debugging */
//#define DENSE_DEBUG
#ifdef DENSE_DEBUG
#define ac_dense_debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define ac_dense_debug(...)
#endif

/* AC-MODES */
#define AC_CASE_INSENSITIVE 0x1 /* case-sensitive is assumed */

typedef struct AC_PATTERN_ AC_PATTERN;
typedef int (*verifier_cb)(AC_PATTERN *, const uint8_t *, size_t, off_t, uint16_t);
// verifiers return 1 (CL_VIRUS) on match, 0 on no match, anything else is an error

struct AC_PATTERN_ {
    uint8_t *pattern;
    uint16_t length;
    uint16_t maxdist; /* maximum path length for ac */

    verifier_cb verify;
    struct event_t vtime;
};

AC_PATTERN *compile_pattern(const uint8_t *sig, uint16_t slen, uint8_t *trigger, uint16_t *tlen, uint16_t mode, uint16_t options);
// sig = to be parsed string, slen is lengh of sig, trigger is submission to ac tree, tlen is max storage, overwrite to used storage
void print_pattern(AC_PATTERN *pattern, int tab);
int free_pattern(AC_PATTERN *pattern);

#endif /* __AC_BACKEND_H__ */
