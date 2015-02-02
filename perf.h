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

#ifndef __PERF_H__
#define __PERF_H__

struct event_t {
    int64_t start;
    int64_t last;
    int64_t sum;
    int64_t match;
    int64_t count;

    int state;
};

void event_init(struct event_t *ev);
void event_start(struct event_t *ev);
void event_stop(struct event_t *ev, int match);
void event_summary(struct event_t *ev, int tab);

#endif /* __PERF_H__ */
