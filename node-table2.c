#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "node-table.h"
#include "compat.h"

static int masterID = 0;

AC_NODE *new_node(uint8_t mode)
{
    AC_NODE *new_node = calloc(1, sizeof(AC_NODE));
    if (!new_node)
        return NULL; //OOM
    new_node->id = masterID++;
    new_node->depth = 0;
    new_node->mode = mode;

    /*** CASE-SENSITIVE ***/
    new_node->table = calloc(256, sizeof(AC_NODE *));
    if (!new_node->table) {
        free(new_node);
        return NULL; //OOM
    }
    new_node->tbl_cnt = 256;
    /*** CASE-SENSITIVE ***/
    /* TODO - other table variants */

    return new_node;
}

/* TODO - inline for speed */
AC_NODE *get_node(AC_NODE *parent, int edge)
{
    if (!parent) return NULL; //INVALID ARG
    if (edge > 0 && edge < parent->tbl_cnt)
        return parent->table[edge]; //INVALID EDGE
    return NULL;
}

AC_NODE *get_or_insert_node(AC_NODE *parent, int edge, int *ret)
{
    AC_NODE *node;

    /* validate */
    if (!parent) {
        if (ret) *ret = CL_ENULLARG;
        return NULL;
    }
    if (edge < 0 && edge >= parent->tbl_cnt) {
        if (ret) *ret = CL_EARG; // invalid edge
        return NULL;
    }

    /* get */
    node = get_node(parent, edge);

    /* insert */
    if (!node) {
        // overloaded to signify tracking
        if (ret) *ret = CL_VIRUS;

        node = new_node(parent->mode);
        if (!node) {
            if (ret) *ret = CL_EMEM;
            return NULL;
        }

        node->depth = parent->depth+1;
        node->value = edge;
        node->fail = parent;

        parent->table[edge] = node;
    } else if (ret)
        if (ret) *ret = CL_SUCCESS;

    return node;
}

int add_patt_node(AC_NODE *node, AC_PATTERN *pattern)
{
    AC_PATTERN **new_patterns;

    if (!node || !pattern)
        return -2; // CL_EARG

    // TODO - check for overlapping patterns

    node->patt_cnt++;
    new_patterns = realloc(node->patterns, node->patt_cnt * sizeof(AC_PATTERN *));
    if (!new_patterns)
        return -1; // CL_EMEM
    new_patterns[node->patt_cnt-1] = pattern;

    node->patterns = new_patterns;
    return 0;
}

int resolve_node(AC_NODE *node)
{
    AC_NODE *fail, *root;
    int i, reloop = 1;

    if (!node) return -2;
    if (node->fail) {
        fail = node->fail->fail; //skips parent node
        while (fail) {
            node->fail = fail;
            if (fail->table[node->value]) {
                node->fail = fail->table[node->value];
                break;
            }

            fail = fail->fail;
        }
    }

    root = node;
    fail = node->fail;
    while (reloop && fail) {
        reloop = 0;
        for (i = 0; i < node->tbl_cnt; ++i) {
            if (!node->table[i] && fail->table[i])
                node->table[i] = fail->table[i];
            else if (!node->table[i])
                reloop = 1;
        }

        if (!fail->fail) root = fail;
        fail = fail->fail;
    }

    if (reloop && !fail) {
        for (i = 0; i < node->tbl_cnt; ++i) {
            if (!node->table[i])
                node->table[i] = root;
        }
    }

    return 0;
}

AC_NODE *advance_node(AC_NODE *node, uint8_t trans)
{
    return node->table[trans];
}

void delete_node(AC_NODE *node)
{
    int i;
    /* patterns */
    if (node->patt_cnt > 0) {
        for (i = 0; i < node->patt_cnt; ++i) {
            free_pattern(node->patterns[i]);
        }
        free(node->patterns);
    }

    /* paths */
    free(node->table);
    free(node);
}

void delete_node_r(AC_NODE *node)
{
    cli_errmsg("cannot delete nodes recursively with current tree settings\n");
}

void print_node(AC_NODE *node, int tab)
{
    int i;
    char tabs[10] = {0};

    if (tab > 9) tab = 9;
    for (i = 0; i < tab; ++i)
        tabs[i] = '\t';

    printf("%s--------------------------\n", tabs);
    if (!node) {
        printf("%sNULL Node!\n", tabs);
        return;
    }

    printf("%sNode ID: %i%s\n", tabs, node->id, (node->id) ? "" : "(root)");
    printf("%s\tdepth: %i\n", tabs, node->depth);
    printf("%s\tmode:  0x%02x\n", tabs, node->mode);
    printf("%s\tvalue: %u[%c]\n", tabs, node->value, node->value);

    /* patterns */
    printf("%sPatterns: %u\n", tabs, node->patt_cnt);
    for (i = 0; i < node->patt_cnt; ++i)
        print_pattern(node->patterns[i], tab+1);

    /* paths */
    if (node->table) {
        printf("%sTable: %u entries\n", tabs, node->tbl_cnt);
        for (i = 0; i < node->tbl_cnt; ++i) {
            if (node->table[i] && node->table[i]->id != 0) {
                printf("%s\t%u[%c], %d\n", tabs, i, isprint(i) ? i : '\0', node->table[i]->id);
            } else if (!node->table[i]) {
                printf("%s\t%u[%c], NULL\n", tabs, i, isprint(i) ? i : '\0');
            }
        }
    }

    if (node->fail) {
        printf("%sFailure Node: %d\n", tabs, node->fail->id);
    }
    printf("%s--------------------------\n", tabs);
}

static void print_node_helper(AC_NODE *node, int tab, uint8_t *seen)
{
    int i;

    print_node(node, tab);
    seen[node->id] = 1;

    if (node->table)
        for (i = 0; i < node->tbl_cnt; ++i)
            if (node->table[i] && !seen[node->table[i]->id])
                print_node_helper(node->table[i], tab+1, seen);
}

void print_node_r(AC_NODE *node, int tab)
{
    uint8_t *seen;

    if (!node) return;

    seen = cli_calloc(masterID, sizeof(uint8_t));
    print_node_helper(node, tab, seen);

    free(seen);
}
