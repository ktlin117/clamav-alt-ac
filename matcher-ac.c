#include "matcher-ac.h"

int ac_init(AC_MATCHER *matcher, uint8_t mindepth, uint8_t maxdepth, uint8_t mode)
{
    matcher->mode = mode;
    matcher->mindepth = mindepth;
    matcher->maxdepth = maxdepth;

    if (mode & AC_LIST) {
    } else {
    }

    return RET_SUCCESS;
}
