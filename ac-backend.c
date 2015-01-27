#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ac-backend.h"

// default verifier
int default_verifier()
{
    return 1; // always a match (ac-tree verified)
}

AC_PATTERN *compile_pattern(const uint8_t *sig, uint16_t slen, uint8_t *trigger, uint16_t *tlen)
{
    AC_PATTERN *new;

    if (!sig || slen <= 0)
        return NULL; // CL_EARG

    new = calloc(1, sizeof(AC_PATTERN));
    if (!new) return NULL; // CL_EMEM

    new->pattern = calloc(slen, sizeof(uint8_t));
    if (!new->pattern) {
        free(new);
        return NULL; // CL_EMEM
    }
    memcpy(new->pattern, sig, slen);
    new->length = slen;

    // trigger has no NULL termination
    if (trigger) {
        if (slen > *tlen) {
            memcpy(trigger, new->pattern, *tlen);
        } else {
            memcpy(trigger, new->pattern, slen);
            *tlen = slen;
        }
    }

    return new;
}

void print_pattern(AC_PATTERN *pattern, int tab)
{
    int i;
    char tabs[10] = {0};

    if (!pattern) return; // CL_EARG

    if (tab > 9) tab = 9;
    for (i = 0; i < tab; ++i)
        tabs[i] = '\t';

    printf("%s", tabs);

    for (i = 0; i < pattern->length; ++i) {
        if (isprint((char)pattern->pattern[i]))
            printf("%c", (char)pattern->pattern[i]);
        else
            printf("[%02x]", pattern->pattern[i]);
    }

    printf("\n");
}

int free_pattern(AC_PATTERN *pattern)
{
    if (!pattern)
        return -2; // INVALID ARG

    free(pattern->pattern);
    free(pattern);
    return 0;
}
