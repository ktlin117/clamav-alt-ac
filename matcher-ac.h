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

#ifndef __MATCHER_AC_H__
#define __MATCHER_AC_H__

#include <stdint.h>
#include "node-table.h"

typedef struct AC_MATCHER_ {
    uint8_t mode, mindepth, maxdepth;

    AC_TABLE_NODE *root;
    /*
    AC_TABLE_NODE **all_nodes;
    uint32_t node_cnt;
    */
} AC_MATCHER;

int ac_init(AC_MATCHER *matcher, uint8_t mindepth, uint8_t maxdepth, uint8_t mode);
int ac_add_pattern(AC_MATCHER *matcher, const char *pattern, uint16_t length);
int ac_resolve_links(AC_MATCHER *matcher);
int ac_scanbuf(AC_MATCHER *matcher, const uint8_t *buffer, unsigned int buflen);
int ac_free(AC_MATCHER *matcher);
void ac_print(AC_MATCHER *matcher);

#endif /* __MATCHER_AC_H__ */
