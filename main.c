#include <stdint.h>
#include <stdio.h>
#include "perf.h"
#include "matcher-ac.h"
#include "node-table.h"
#include "ac-backend.h"

int main()
{
    AC_MATCHER matcher;
    struct event_t build, scan;

    event_init(&build);
    event_init(&scan);

    event_start(&build);
    ac_init(&matcher, 0, 0, AC_CASE_INSENSITIVE);
    ac_add_pattern(&matcher, "TAKE", 4, 0);
    ac_add_pattern(&matcher, "fast", 4, AC_CASE_INSENSITIVE);
    ac_add_pattern(&matcher, "soft", 4, AC_CASE_INSENSITIVE);
    ac_add_pattern(&matcher, "ake", 3, AC_CASE_INSENSITIVE);
    ac_add_pattern(&matcher, "\0\0\0\0a", 5, AC_CASE_INSENSITIVE);
    ac_resolve_links(&matcher);
    event_stop(&build, 1);

#ifdef DENSE_DEBUG
    ac_print(&matcher);
#endif

    event_start(&scan);
    ac_scanbuf(&matcher, (const uint8_t *)"TAKE\0\0mimisoft", 14);
    event_stop(&scan, 1);

    printf("Build Time Summary:\n");
    event_summary(&build, 1);

    printf("Scan Time Summary:\n");
    event_summary(&scan, 1);

    ac_free(&matcher);

    /** Node Testing **/
    /*
    AC_TABLE_NODE *root, *side;

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
