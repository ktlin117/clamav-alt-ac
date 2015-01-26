#include <stdint.h>
#include "matcher-ac.h"
#include "node-table.h"

int main()
{
    AC_MATCHER matcher;

    ac_init(&matcher, 0, 0, 0);
    ac_add_pattern(&matcher, "take");
    //ac_add_pattern(&matcher, "fast");
    //ac_add_pattern(&matcher, "soft");
    ac_add_pattern(&matcher, "ake");

    ac_resolve_links(&matcher);
    ac_print(&matcher);

    ac_scanbuf(&matcher, (const uint8_t *)"take", 4);

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
