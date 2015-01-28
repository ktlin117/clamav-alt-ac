#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
#include "perf.h"

static int64_t get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((int64_t)tv.tv_sec * 1000000) + tv.tv_usec;
}

void event_init(struct event_t *ev)
{
    ev->start = 0;
    ev->last = 0;
    ev->sum = 0;
    ev->count = 0;
    ev->state = 0;
}

/* event tracking overhead ~0.30us */
void event_start(struct event_t *ev)
{
    if (ev->state != 0)
        return;

    ev->start = get_time();
    ev->state = 1;
}

void event_stop(struct event_t *ev)
{
    int64_t end;
    if (ev->state != 1)
        return;

    end = get_time();

    ev->last = end - ev->start;
    ev->sum += ev->last;
    ev->count++;
    ev->state = 0;
}

/* event summary overhead ~60us */
void event_summary(struct event_t *ev, int tab)
{
    int i;
    char tabs[10] = {0};

    if (tab > 9) tab = 9;
    for (i = 0; i < tab; ++i)
        tabs[i] = '\t';

    printf("%slast:   %llu us\n", tabs, ev->last);
    printf("%sevents: %llu events\n", tabs, ev->count);
    printf("%stotal:  %llu us\n", tabs, ev->sum);
    printf("%savg:    %02.2f us\n", tabs, ev->count ? ((float)ev->sum / (float)ev->count) : 0);
}
