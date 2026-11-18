
#include "forma/action_executor.hpp"

#include <iostream>
#include <stdexcept>

int main() {
    using namespace forma;

    const SystemState initial{
        .hostname = "pop-os"
    };

    const ActionPlan plan{
        .actions = {
            Action{
                .type = ActionType::SetHostname,
                .current_value = "pop-os",
                .desired_value = "forma-laptop"
            }
        }
    };

    // Test 1: Apply one action to simulated state.
    const ExecutionResult result =
        simulate_actions(plan, initial);

    if (result.final_state.hostname != "forma-laptop" ||
        result.actions_applied != 1) {
        std::cerr << "Simulated action failed.\n";
        return 1;
        }

    // Test 2: The original state must remain unchanged.
    if (initial.hostname != "pop-os") {
        std::cerr << "Original state was modified.\n";
        return 1;
    }

    // Test 3: Empty plans must apply no actions.
    const ExecutionResult empty_result =
        simulate_actions(ActionPlan{}, initial);

    if (empty_result.actions_applied != 0 ||
        empty_result.final_state.hostname != "pop-os") {
        std::cerr << "Empty plan handled incorrectly.\n";
        return 1;
        }

    // Test 4: Reject stale plans.
    bool rejected_stale_plan = false;

    try {
        const SystemState changed{
            .hostname = "another-host"
        };

        (void)simulate_actions(plan, changed);

    } catch (const std::runtime_error&) {
        rejected_stale_plan = true;
    }

    if (!rejected_stale_plan) {
        std::cerr << "Stale action plan was accepted.\n";
        return 1;
    }

    std::cout << "All simulated executor tests passed.\n";
    return 0;
}
