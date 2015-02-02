#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matcher-ac.h"
#include "node-table.h"
#include "ac-backend.h"
#include "queue.h"

#define TRIGLENCAP 3

int ac_init(AC_MATCHER *matcher, uint8_t mindepth, uint8_t maxdepth, uint16_t mode)
{
    matcher->mode = mode;
    matcher->mindepth = mindepth;
    matcher->maxdepth = maxdepth;

    matcher->root = new_node(mode);
    if (!matcher->root)
        return -1; // NODAL ISSUE, most likely OOM

    return 0;
}

int ac_add_pattern(AC_MATCHER *matcher, const char *pattern, uint16_t length, uint16_t options)
{
    int i;
    uint16_t tlen = TRIGLENCAP;
    uint8_t trigger[TRIGLENCAP];
    AC_TABLE_NODE *track = matcher->root;
    AC_PATTERN *ac_pattern;

    ac_pattern = compile_pattern((uint8_t *)pattern, length, trigger, &tlen, matcher->mode, options);
    if (!ac_pattern)
        return -1; // Error: CL_EMEM

    for (i = 0; i < tlen; ++i) {
        track = get_or_insert_node(track, trigger[i]);
        if (!track)
            return -1; // NODAL ISSUE, most likely OOM
    }

    // add pattern to final node
    return add_patt_node(track, ac_pattern); // TODO - check the error codes here?
}

int ac_resolve_links(AC_MATCHER *matcher)
{
    STAILQ_HEAD(stail_head, entry) head =
        STAILQ_HEAD_INITIALIZER(head);
    struct entry {
        AC_TABLE_NODE *node;
        STAILQ_ENTRY(entry) entries;
    } *track;
    AC_TABLE_NODE *current, *fail;
    int i;

    STAILQ_INIT(&head);
    track = malloc(sizeof(struct entry));
    if (!track) return -1; //OOM
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
                track = malloc(sizeof(struct entry));
                if (!track) return -1; //OOM, TODO - memory freeing on failure
                track->node = current->table[i];
                STAILQ_INSERT_TAIL(&head, track, entries);
            }
        }

        /* resolve failure */
        if (current->fail) {
            fail = current->fail->fail; //skips parent node
            while (fail) {
                current->fail = fail;
                if (fail->table[current->value]) {
                    current->fail = fail->table[current->value];
                    break;
                }

                fail = fail->fail;
            }
        }
    }

    return 0;
}

int ac_scanbuf(AC_MATCHER *matcher, const uint8_t *buffer, unsigned int buflen)
{
    unsigned int i, j;
    uint8_t trans;
    AC_TABLE_NODE *current = matcher->root, *others;

    for (i = 0; i < buflen; ++i) {
        ac_dense_debug("ac_scanbuf: current node: %d\n", current->id);

        // TODO: separate into different loops to reduce perfroming check on every cycle
        if (matcher->mode & AC_CASE_INSENSITIVE)
            trans = (uint8_t)tolower(buffer[i]);
        else
            trans = buffer[i];

        /* advance the node or fail */
        if (current->table[trans]) {
            current = current->table[trans];
        } else {
            while (current->fail) {
                current = current->fail;
                if (current->table[trans]) {
                    current = current->table[trans];
                    break;
                }
            }
            continue;
        }

        ac_dense_debug("ac_scanbuf: move to node: %d\n", current->id);

        /* check the patterns Mason! */
        if (current->patt_cnt) {
            printf("FOUND PATTERNS:\n");
            for (j = 0; j < current->patt_cnt; ++j) {
                if (current->patterns[j]->verify(current->patterns[j], buffer, buflen, i, matcher->mode) == 1)
                    print_pattern(current->patterns[j], 1);
            }
        }
        /* check the fail nodes patterns too! */
        others = current->fail;
        while (others) {
            if (others->patt_cnt) {
                printf("FOUND OTHER PATTERNS:\n");
                for (j = 0; j < others->patt_cnt; ++j) {
                    if (others->patterns[j]->verify(current->patterns[j], buffer, buflen, i, matcher->mode) == 1)
                        print_pattern(others->patterns[j], 1);
                }
            }
            others = others->fail;
        }
    }

    return 0;
}

int ac_free(AC_MATCHER *matcher)
{
    if (!matcher) return -2; //INVALID ARG
    delete_node_r(matcher->root);
    return 0;
}

void ac_print(AC_MATCHER *matcher)
{
    if (!matcher) return; //INVALID ARG
    print_node_r(matcher->root, 0);
}
