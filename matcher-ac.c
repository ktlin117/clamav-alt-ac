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

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matcher-ac.h"
#include "node-table.h"
#include "ac-backend.h"
#include "perf.h"
#include "queue.h"
#include "compat.h"

#define TRIGLENCAP 3

int ac_init(AC_MATCHER *matcher, uint8_t mindepth, uint8_t maxdepth, uint16_t mode)
{
    matcher->mode = mode;
    matcher->mindepth = mindepth;
    matcher->maxdepth = maxdepth;

    matcher->root = new_node(mode);
    if (!matcher->root)
        return CL_EMEM;

    matcher->all_nodes = cli_calloc(1, sizeof(AC_NODE *));
    if (!matcher->all_nodes)
        return CL_EMEM;
    matcher->all_nodes[0] = matcher->root;
    matcher->node_cnt = 1;

    matcher->all_patts = NULL;
    matcher->patt_cnt = 0;

    return 0;
}

int ac_add_pattern(AC_MATCHER *matcher, const char *pattern, uint16_t length, const uint32_t *lsigid, uint16_t options)
{
    int i, ret;
    uint16_t tlen = TRIGLENCAP;
    uint8_t trigger[TRIGLENCAP];
    AC_NODE *track = matcher->root, **new_nodes;
    AC_PATTERN *ac_pattern, **new_patts;

    ac_pattern = compile_pattern((uint8_t *)pattern, length, trigger, &tlen, lsigid, matcher->mode, options);
    if (!ac_pattern)
        return CL_EMEM; // Error: CL_EMEM

    for (i = 0; i < tlen; ++i) {
        track = get_or_insert_node(track, trigger[i], &ret);
        if (ret != CL_SUCCESS && ret != CL_VIRUS)
            return ret; //nodal issue

        /* overloaded to insert to allnodes list */
        if (ret == CL_VIRUS) {
            matcher->node_cnt++;
            new_nodes = cli_realloc(matcher->all_nodes, matcher->node_cnt * sizeof(AC_NODE *));
            if (!new_nodes)
                return CL_EMEM;
            new_nodes[matcher->node_cnt-1] = track;
            matcher->all_nodes = new_nodes;
        }
    }

    // add pattern to the full list
    matcher->patt_cnt++;
    new_patts = cli_realloc(matcher->all_patts, matcher->patt_cnt * sizeof(AC_PATTERN *));
    if (!new_patts)
        return CL_EMEM;
    new_patts[matcher->patt_cnt-1] = ac_pattern;
    matcher->all_patts = new_patts;

    // add pattern to final node
    return add_patt_node(track, ac_pattern); // TODO - check the error codes here (same pattern?)?
}

int ac_resolve_links(AC_MATCHER *matcher)
{
    int i, ret = CL_SUCCESS;
    AC_NODE *current;
    STAILQ_HEAD(stail_head, entry) head =
        STAILQ_HEAD_INITIALIZER(head);
    struct entry {
        AC_NODE *node;
        STAILQ_ENTRY(entry) entries;
    } *track;

    STAILQ_INIT(&head);
    track = cli_malloc(sizeof(struct entry));
    if (!track) return CL_EMEM;
    track->node = matcher->root;
    STAILQ_INSERT_HEAD(&head, track, entries);

    while (!STAILQ_EMPTY(&head)) {
        track = STAILQ_FIRST(&head);
        current = track->node;
        STAILQ_REMOVE_HEAD(&head, entries);
        free(track);

        ac_dense_debug("QUEUE ID: %d\n", current->id);

        /* breadth-first */
        for (i = 0; i < current->tbl_cnt; ++i) {
            if (current->table[i]) {
                track = cli_malloc(sizeof(struct entry));
                if (!track) {
                    ret = CL_EMEM;
                    goto queue_cleanup;
                }
                track->node = current->table[i];
                STAILQ_INSERT_TAIL(&head, track, entries);
            }
        }

        /* resolve failure */
        if ((ret = resolve_node(current)) != CL_SUCCESS)
            goto queue_cleanup;
    }

 queue_cleanup:
    while (!STAILQ_EMPTY(&head)) {
        track = STAILQ_FIRST(&head);
        STAILQ_REMOVE_HEAD(&head, entries);
        free(track);
    }

    return ret;
}

int ac_scanbuf(AC_MATCHER *matcher, const uint8_t *buffer, unsigned int buflen)
{
    unsigned int i, j;
    uint8_t trans;
    AC_NODE *current = matcher->root, *others;

    for (i = 0; i < buflen; ++i) {
        ac_dense_debug("ac_scanbuf: current node: %d\n", current->id);

        // TODO: separate into different loops to reduce perfroming check on every cycle
        if (matcher->mode & AC_CASE_INSENSITIVE)
            trans = (uint8_t)tolower(buffer[i]);
        else
            trans = buffer[i];

        /* advance the node or fail - TODO: inline */
        current = advance_node(current, trans);

        ac_dense_debug("ac_scanbuf: move to node: %d\n", current->id);

        /* check the patterns Mason! */
        if (current->patt_cnt) {
#ifdef DENSE_DEBUG
            printf("FOUND PATTERNS:\n");
#endif
            for (j = 0; j < current->patt_cnt; ++j) {
                event_start(&current->patterns[j]->vtime);
                if (current->patterns[j]->verify(current->patterns[j], buffer, buflen, i, current->depth, matcher->mode) == 1) {
                    event_stop(&current->patterns[j]->vtime, 1);
#ifdef DENSE_DEBUG
                    print_pattern(current->patterns[j], 1);
#endif
                }
                event_stop(&current->patterns[j]->vtime, 0);
            }
        }
        /* check the fail nodes patterns too! */
        others = current->fail;
        while (others) {
            if (others->patt_cnt) {
#ifdef DENSE_DEBUG
                printf("FOUND OTHER PATTERNS:\n");
#endif
                for (j = 0; j < others->patt_cnt; ++j) {
                    event_start(&others->patterns[j]->vtime);
                    if (others->patterns[j]->verify(others->patterns[j], buffer, buflen, i, current->depth, matcher->mode) == 1) {
                        event_stop(&others->patterns[j]->vtime, 1);
#ifdef DENSE_DEBUG
                        print_pattern(others->patterns[j], 1);
#endif
                    }
                    event_stop(&others->patterns[j]->vtime, 0);
                }
            }
            others = others->fail;
        }
    }

    return 0;
}

int ac_free(AC_MATCHER *matcher)
{
    int i;

    if (!matcher) return -2; //INVALID ARG
    //delete_node_r(matcher->root);
    for (i = 0; i < matcher->node_cnt; ++i)
        delete_node(matcher->all_nodes[i]);
    free(matcher->all_nodes);
    free(matcher->all_patts);
    return CL_SUCCESS;
}

void ac_print(AC_MATCHER *matcher)
{
    if (!matcher) return; //INVALID ARG
    print_node_r(matcher->root, 0);
}
