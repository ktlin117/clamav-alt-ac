#include <string.h>
#include "matcher-ac.h"
#include "node-table.h"

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

int ac_add_pattern(AC_MATCHER *matcher, const char *pattern)
{
    int i;
    AC_TABLE_NODE *track = matcher->root;

    for (i = 0; i < strlen(pattern); ++i) {
        track = get_or_insert_node(track, pattern[i]);
        if (!track)
            return -2; // NODAL ISSUE, most likely OOM
    }

    /* INSERT PATTERN HERE! */

    return 0;
}

int ac_resolve_links(AC_MATCHER *matcher)
{
    
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
