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

#include "node-table.h"
#include "node-list.h"

#define AC_TABLE 0x00
#define AC_LIST  0x01
#define AC_CASE_SENSITIVE 0x02

typedef struct AC_MATCHER_ {
    uint8_t mode, mindepth, maxdepth;

    union root {
        AC_TABLE_NODE *tbl;
        AC_LIST_NODE *list;
    };
    union all {
        AC_TABLE_NODE *tbl;
        AC_LIST_NODE *list;
    };
} AC_MATCHER;

int ac_init(AC_MATCHER *matcher, uint8_t mindepth, uint8_t maxdepth, uint8_t mode);

#endif /* __MATCHER_AC_H__ */
