#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include "perf.h"
#include "matcher-ac.h"
#include "node-table.h"
#include "ac-backend.h"

const uint8_t buffer[] = "TAKE\0\0mimisoftTAkE\0\0mimisOftTAKE\0\0mimisoftTAKE\0\0mimisofttake\0\0mimisoft";
const unsigned int buflen = (sizeof(buffer)) / (sizeof(buffer[0]));
void print_statistics(AC_MATCHER *matcher, struct event_t build, struct event_t scan);

int main()
{
    AC_MATCHER matcher;
    struct event_t build, scan;
    int ret;

    event_init(&build);
    event_init(&scan);

    event_start(&build);
    ret = ac_init(&matcher, 0, 0, AC_CASE_INSENSITIVE);
    printf("ac_init ret: %d\n", ret);
    ret = ac_add_pattern(&matcher, "TAKE", 4, 0);
    printf("ac_add_pattern ret: %d\n", ret);
    ret = ac_add_pattern(&matcher, "fast", 4, AC_CASE_INSENSITIVE);
    printf("ac_add_pattern ret: %d\n", ret);
    ret = ac_add_pattern(&matcher, "soft", 4, AC_CASE_INSENSITIVE);
    printf("ac_add_pattern ret: %d\n", ret);
    ret = ac_add_pattern(&matcher, "ak", 2, AC_CASE_INSENSITIVE);
    printf("ac_add_pattern ret: %d\n", ret);
    ret = ac_add_pattern(&matcher, "\0\0\0\0a", 5, AC_CASE_INSENSITIVE);
    printf("ac_add_pattern ret: %d\n", ret);
    ret = ac_resolve_links(&matcher);
    printf("ac_resolve_links ret: %d\n", ret);
    event_stop(&build, 1);

#ifdef DENSE_DEBUG
    ac_print(&matcher);
#endif

    event_start(&scan);
    ret = ac_scanbuf(&matcher, (const uint8_t *)buffer, buflen);
    event_stop(&scan, 1);
    printf("ac_scanbuf ret: %d\n", ret);

    //printf("Build Time Summary:\n");
    //event_summary(&build, 1);

    //printf("Scan Time Summary:\n");
    //event_summary(&scan, 1);

    print_statistics(&matcher, build, scan);

    ac_free(&matcher);

    /** Node Testing **/
    /*
    AC_NODE *root, *side;

    root = new_node(17);

    get_or_insert_node(root, 'a');
    get_or_insert_node(root, 'a');
    side = get_or_insert_node(root, 'C');
    get_or_insert_node(side, 'a');
    get_or_insert_node(root, 'A');

    print_node_r(root, 0);
    delete_node_r(root);
    */
    return 0;
}

void print_statistics(AC_MATCHER *matcher, struct event_t build, struct event_t scan)
{
    int i, j;

    printf("Runtime Statistics:\n");
    printf("%-40s%-20s%-20s%-20s%-20s%-20s\n", "event", "last [us]", "sum [us]", "match", "count", "average [us]");
    printf("%-40s%-20s%-20s%-20s%-20s%-20s\n", "----------------------------", "--------------", "--------------",
           "--------------", "--------------", "--------------"); 
    printf("%-40s%-20llu%-20llu%-20llu%-20llu%-20.2f\n", "build", build.last, build.sum, build.match, 
           build.count, build.count ? ((float)build.sum / (float)build.count) : 0);
    printf("%-40s%-20llu%-20llu%-20llu%-20llu%-20.2f\n", "scan", scan.last, scan.sum, scan.match, 
           scan.count, scan.count ? ((float)scan.sum / (float)scan.count) : 0);
    for (i = 0; i < matcher->patt_cnt ; ++i) {
        AC_PATTERN *patt = matcher->all_patts[i];
        int sum = 0;

        for (j = 0; j < patt->length; ++j) {
            if (isprint((char)patt->pattern[j])) {
                printf("%c", (char)patt->pattern[j]);
                sum++;
            } else {
                printf("[%02x]", patt->pattern[j]);
                sum+=4;
            }
        }
        for (; sum < 40; sum++)
            printf(" ");
        printf("%-20llu%-20llu%-20llu%-20llu%-20.2f\n", patt->vtime.last, patt->vtime.sum, patt->vtime.match, 
               patt->vtime.count, patt->vtime.count ? ((float)patt->vtime.sum / (float)patt->vtime.count) : 0);
    }
}
