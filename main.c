#include "node-table.h"

int main()
{
    AC_TABLE_NODE *root, *side;

    root = new_node(17);

    get_or_insert_node(root, 'a');
    get_or_insert_node(root, 'a');
    side = get_or_insert_node(root, 'C');
    get_or_insert_node(side, 'a');
    get_or_insert_node(root, 'A');

    print_node_r(root, 0);
    delete_node_r(root);

    return 0;
}
