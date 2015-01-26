#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ac-backend.h"

// default verifier
int default_verifier()
{
    return 1; // always a match (ac-tree verified)
}

//TODO - handle intermittent NULL characters
AC_PATTERN *compile_pattern(const char *pattern, char *trigger, unsigned int trig_len)
{
    AC_PATTERN *new_patt;

    if (!pattern) return NULL; // INVALID ARG

    new_patt = calloc(1, sizeof(AC_PATTERN));
    if (!new_patt) return NULL; // OOM

    new_patt->pattern = strdup(pattern);
    if (!new_patt->pattern) {
        free(new_patt);
        return NULL; // OOM
    }

    new_patt->verify = default_verifier;

    if (trigger) { // if they want a trigger
        memset(trigger, 0, trig_len);
        strncpy(trigger, pattern, trig_len-1); // -1 to guarentee NULL-term
    }

    return new_patt;
}

void print_pattern(AC_PATTERN *pattern, int tab)
{
    int i;
    char tabs[10] = {0};

    if (!pattern) return; // INVALID ARG

    if (tab > 9) tab = 9;
    for (i = 0; i < tab; ++i)
        tabs[i] = '\t';

    printf("%s%s\n", tabs, pattern->pattern); // TODO - handle the intermitten NULLs
}

int free_pattern(AC_PATTERN *pattern)
{
    if (!pattern)
        return -2; // INVALID ARG

    free(pattern->pattern);
    free(pattern);
    return 0;
}
