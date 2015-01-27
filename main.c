#include <stdint.h>
#include "matcher-ac.h"
#include "node-table.h"

int main()
{
    AC_MATCHER matcher;

    ac_init(&matcher, 0, 0, 0);
    ac_add_pattern(&matcher, "take", 4);
    ac_add_pattern(&matcher, "fast", 4);
    ac_add_pattern(&matcher, "soft", 4);
    ac_add_pattern(&matcher, "ake", 3);
    ac_add_pattern(&matcher, "\0\0\0\0a", 5);

    ac_resolve_links(&matcher);
    ac_print(&matcher);

    ac_scanbuf(&matcher, (const uint8_t *)"take\0\0mimisoft", 14);

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
