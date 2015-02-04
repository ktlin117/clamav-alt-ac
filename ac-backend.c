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
#include <sys/types.h>
#include "ac-backend.h"

// dummy verifier - used with idiots
int dummy_verifier(AC_PATTERN *pattern, const uint8_t *buffer, size_t buflen, off_t offset, uint32_t depth, uint16_t mode)
{
    return 1; // always a match (ac-tree verified)
}

// CS sequence verifier - used with CS patterns
int cs_sequence_verifier(AC_PATTERN *pattern, const uint8_t *buffer, size_t buflen, off_t offset, uint32_t depth, uint16_t mode)
{
    off_t start;

    if (!(mode & AC_CASE_INSENSITIVE) && (depth > pattern->length))
        return 1;

    start = offset - depth + 1; /* adjust for trigger */

    if (start < 0 || start + pattern->length > buflen)
        return 0;

    if (!memcmp(pattern->pattern, buffer+start, pattern->length))
        return 1;

    return 0;
}

// CI sequence verifier - used with CI (lowercase) patterns (usually with CI AC trees)
int ci_sequence_verifier(AC_PATTERN *pattern, const uint8_t *buffer, size_t buflen, off_t offset, uint32_t depth, uint16_t mode)
{
    off_t start;
    uint16_t i;

    if ((mode & AC_CASE_INSENSITIVE) && depth > pattern->length)
        return 1;

    start = offset - depth + 1; /* adjust for trigger */

    if (start < 0 || start + pattern->length > buflen)
        return 0;

    for (i = 0; i < pattern->length; ++i)
        if ((uint8_t)tolower(buffer[start+i]) != pattern->pattern[i]) // assuming pattern is lowercase
            return 0;

    return 1;
}


AC_PATTERN *compile_pattern(const uint8_t *sig, uint16_t slen, uint8_t *trigger, uint16_t *tlen, const uint32_t *lsigid, uint16_t mode, uint16_t options)
{
    AC_PATTERN *new;
    int i;

    if (!sig || slen <= 0)
        return NULL; // CL_EARG

    new = calloc(1, sizeof(AC_PATTERN));
    if (!new) return NULL; // CL_EMEM

    new->pattern = calloc(slen, sizeof(uint8_t));
    if (!new->pattern) {
        free(new);
        return NULL; // CL_EMEM
    }

    if (options & AC_CASE_INSENSITIVE) {
        for (i = 0; i < slen; ++i)
            new->pattern[i] = (uint8_t)tolower(sig[i]);
        new->verify = ci_sequence_verifier;
    } else {
        memcpy(new->pattern, sig, slen);
        new->verify = cs_sequence_verifier;
    }

    if (lsigid) {
        new->lsigid[0] = 1;
        new->lsigid[1] = lsigid[0];
        new->lsigid[2] = lsigid[1];
    } else {
        /* sigtool */
        new->lsigid[0] = 0;
    }
    new->length = slen;
    event_init(&new->vtime);

    // trigger has no NULL termination, TODO - move trigger generation to matcher?
    if (trigger) {
        if (mode & AC_CASE_INSENSITIVE) {
            if (slen <= *tlen)
                *tlen = slen;

            for (i = 0; i < *tlen; ++i)
                trigger[i] = (uint8_t)tolower(new->pattern[i]);
        } else {
            if (slen <= *tlen)
                *tlen = slen;

            memcpy(trigger, new->pattern, *tlen);
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
    if (pattern->lsigid[0])
        printf("[%u, %u] ", pattern->lsigid[1], pattern->lsigid[2]);

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
        return -2; // CL_EARG

    free(pattern->pattern);
    free(pattern);
    return 0;
}
