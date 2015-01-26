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

typedef int (*verifier_cb)(); //trival verifer, add addtitional junk later
// verifiers return 1 (CL_VIRUS) on match, 0 on no match, anything else is an error

int default_verifier();

typedef struct AC_PATTERN_ {
    char *pattern;
    verifier_cb verify;
} AC_PATTERN;

AC_PATTERN *compile_pattern(const char *string, char *trigger, unsigned int trig_len);
// string = to be parsed string, trigger is submission to ac tree, trig_len is max storage
void print_pattern(AC_PATTERN *pattern, int tab);
int free_pattern(AC_PATTERN *pattern);

#endif /* __AC_BACKEND_H__ */
