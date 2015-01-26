#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matcher-ac.h"
#include "node-table.h"
#include "ac-backend.h"
#include "queue.h"

#define TRIGLENCAP 256

int ac_init(AC_MATCHER *matcher, uint8_t mindepth, uint8_t maxdepth, uint8_t mode)
{
    matcher->mode = mode;
    matcher->mindepth = mindepth;
    matcher->maxdepth = maxdepth;

    matcher->root = new_node(mode);
    if (!matcher->root)
        return -1; // NODAL ISSUE, most likely OOM

    return 0;
}

// TODO - handle intermitten NULL characters, is this to be hex-encoded?
int ac_add_pattern(AC_MATCHER *matcher, const char *pattern)
{
    int i;
    char trigger[TRIGLENCAP];
    AC_TABLE_NODE *track = matcher->root;
    AC_PATTERN *ac_pattern;

    ac_pattern = compile_pattern(pattern, trigger, TRIGLENCAP);
    if (!ac_pattern) {
        return -4; // critical error
    }

    for (i = 0; i < strlen(trigger); ++i) {
        track = get_or_insert_node(track, trigger[i]);
        if (!track)
            return -2; // NODAL ISSUE, most likely OOM
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

        printf("QUEUE ID: %d\n", current->id);

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

                fail = fail->fail; /* skips first fail node (parent) */
            }
        }
    }

    return 0;
}

int ac_scanbuf(AC_MATCHER *matcher, const uint8_t *buffer, unsigned int buflen)
{
    unsigned int i, j;
    AC_TABLE_NODE *current = matcher->root, *others;

    for (i = 0; i < buflen; ++i) {
        printf("ac_scanbuf: current node: %d\n", current->id);

        /* advance the node or fail */
        if (current->table[buffer[i]]) {
            current = current->table[buffer[i]];
        } else {
            while (current->fail) {
                current = current->fail;
                if (current->table[buffer[i]]) {
                    current = current->table[buffer[i]];
                    break;
                }
            }
            continue;
        }

        printf("ac_scanbuf: move to node: %d\n", current->id);

        /* check the patterns Mason! */
        if (current->patt_cnt) {
            printf("FOUND PATTERNS:\n");
            for (j = 0; j < current->patt_cnt; ++j) {
                print_pattern(current->patterns[j], 1);
            }
        }
        /* check the fail nodes patterns too! */
        others = current->fail;
        while (others) {
            if (others->patt_cnt) {
                printf("FOUND OTHER PATTERNS:\n");
                for (j = 0; j < others->patt_cnt; ++j) {
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
