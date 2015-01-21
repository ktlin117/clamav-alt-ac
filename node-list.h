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

#ifndef __NODE_LIST_H__
#define __NODE_LIST_H__

#include <stdint.h>
#include "ac-backend.h"

typedef struct AC_LIST_NODE_ {
    int id, type, depth;

    /* patterns */
    AC_PATTERN *patterns;
    uint32_t patt_cnt;

    /* paths */
    struct AC_LIST_NODE_ *next;
    struct AC_LIST_NODE_ *down;

    struct AC_LIST_NODE_ *fail;
} AC_LIST_NODE;


#endif /* __NODE_LIST_H__ */
