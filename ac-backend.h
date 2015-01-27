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

typedef int (*verifier_cb)(); //trival verifer, add addtitional junk later
// verifiers return 1 (CL_VIRUS) on match, 0 on no match, anything else is an error

int default_verifier();

typedef struct AC_PATTERN_ {
    uint8_t *pattern;
    uint16_t length;

    verifier_cb verify;
} AC_PATTERN;

AC_PATTERN *compile_pattern(const uint8_t *sig, uint16_t slen, uint8_t *trigger, uint16_t *tlen);
// sig = to be parsed string, slen is lengh of sig, trigger is submission to ac tree, tlen is max storage, overwrite to used storage
void print_pattern(AC_PATTERN *pattern, int tab);
int free_pattern(AC_PATTERN *pattern);

#endif /* __AC_BACKEND_H__ */
