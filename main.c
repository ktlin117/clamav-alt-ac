#include "matcher-ac.h"
#include "node-table.h"

int main()
{
    AC_MATCHER matcher;

    ac_init(&matcher, 0, 0, 0);
    ac_add_pattern(&matcher, "clamav");
    ac_add_pattern(&matcher, "steve");
    ac_add_pattern(&matcher, "calmav");
    ac_add_pattern(&matcher, "clambv");
    ac_add_pattern(&matcher, "clamAV");
    ac_add_pattern(&matcher, "clamAVBC");

    ac_print(&matcher);
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
