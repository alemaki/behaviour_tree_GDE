#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest.h>

#include "behaviour_tree/ui/bt_graph_view.hpp"
#include "behaviour_tree/ui/bt_graph_node.hpp"
#include "behaviour_tree/ui/bt_graph_node_subtree.hpp"
#include "tests/test_utils/signal_watcher.hpp"

struct BTGraphViewFixture
{
    BTGraphView* graph_view = memnew(BTGraphView);
    BTGraphViewFixture(){}
    ~BTGraphViewFixture()
    {
        memdelete(graph_view);
    }
};


TEST_SUITE("[editor]" "BTGraphView")
{
    TEST_CASE_FIXTURE(BTGraphViewFixture, "Setters and getters")
    {
        REQUIRE_EQ(graph_view->get_undo_redo_manager(), nullptr);
        REQUIRE_FALSE(graph_view->is_undo_redo_enabled());
    }
}