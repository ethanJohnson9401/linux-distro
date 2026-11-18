
#pragma once

#include "forma/action_plan.hpp"

#include <iosfwd>

namespace forma {

    // Display a read-only plan of proposed system actions.
    void print_action_plan(
        const ActionPlan& plan,
        std::ostream& output
    );

} // namespace forma
