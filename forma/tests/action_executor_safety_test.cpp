
#include "forma/action_executor.hpp"

#include <iostream>
#include <stdexcept>

int main() {
    using namespace forma;

    const SystemState initial{
        .hostname = "pop-os"
    };

    // Test 1: Multiple valid actions execute in order.
    const ActionPlan valid_plan{
        .actions = {
            {
                .type = ActionType::SetHostname,
                .current_value = "pop-os",
                .desired_value = "forma-test"
            },
            {
                .type = ActionType::SetHostname,
                .current_value = "forma-test",
                .desired_value = "forma-desktop"
            }
        }
    };

    const ExecutionResult result =
        simulate_actions(valid_plan, initial);

    if (result.final_state.hostname != "forma-desktop" ||
        result.actions_applied != 2 ||
        initial.hostname != "pop-os") {
        std::cerr << "Multi-action simulation failed.\n";
        return 1;
    }

    // Test 2: Reject a stale second action.
    const ActionPlan stale_plan{
        .actions = {
            {
                .type = ActionType::SetHostname,
                .current_value = "pop-os",
                .desired_value = "forma-test"
            },
            {
                .type = ActionType::SetHostname,
                .current_value = "wrong-host",
                .desired_value = "forma-desktop"
            }
        }
    };

    bool stale_rejected = false;

    try {
        (void)simulate_actions(stale_plan, initial);
    } catch (const std::runtime_error&) {
        stale_rejected = true;
    }

    if (!stale_rejected || initial.hostname != "pop-os") {
        std::cerr << "Stale action sequence was accepted.\n";
        return 1;
    }

    // Test 3: Reject unsupported action types.
    const ActionPlan unsupported_plan{
        .actions = {
            {
                .type = static_cast<ActionType>(999),
                .current_value = "pop-os",
                .desired_value = "forma-desktop"
            }
        }
    };

    bool unsupported_rejected = false;

    try {
        (void)simulate_actions(unsupported_plan, initial);
    } catch (const std::invalid_argument&) {
        unsupported_rejected = true;
    }

    if (!unsupported_rejected) {
        std::cerr << "Unsupported action was accepted.\n";
        return 1;
    }

    // Test 4: Reject an empty desired hostname.
    const ActionPlan empty_hostname_plan{
        .actions = {
            {
                .type = ActionType::SetHostname,
                .current_value = "pop-os",
                .desired_value = ""
            }
        }
    };

    bool empty_rejected = false;

    try {
        (void)simulate_actions(empty_hostname_plan, initial);
    } catch (const std::invalid_argument&) {
        empty_rejected = true;
    }

    if (!empty_rejected) {
        std::cerr << "Empty hostname was accepted.\n";
        return 1;
    }

    std::cout
        << "All executor safety tests passed.\n";

    return 0;
}
