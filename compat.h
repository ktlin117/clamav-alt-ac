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

#ifndef __RETCODE_H__
#define __RETCODE_H__

#define CL_VIRUS 1
#define CL_SUCCESS 0
#define CL_EMEM -1
#define CL_EARG -2
#define CL_ENULLARG -3

#define cli_dbgmsg(...) fprintf(stderr, __VA_ARGS__)
#define cli_errmsg(...) fprintf(stderr, __VA_ARGS__)

#define cli_malloc(size) malloc(size)
#define cli_calloc(count, size) calloc(count, size)
#define cli_realloc(ptr, size) realloc(ptr, size)

#endif
