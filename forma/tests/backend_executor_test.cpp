
#include "forma/action_executor.hpp"
#include "forma/memory_backend.hpp"

#include <iostream>
#include <stdexcept>

int main() {
    using namespace forma;

    MemoryBackend backend(
        SystemState{.hostname = "pop-os"}
    );

    const ActionPlan plan{
        .actions = {
            Action{
                .type = ActionType::SetHostname,
                .current_value = "pop-os",
                .desired_value = "forma-desktop"
            }
        }
    };

    // Test 1: The executor updates the memory backend.
    const ExecutionResult result =
        execute_actions(plan, backend);

    if (result.actions_applied != 1 ||
        result.final_state.hostname != "forma-desktop" ||
        backend.read_state().hostname != "forma-desktop") {
        std::cerr << "Backend execution failed.\n";
        return 1;
        }

    // Test 2: A stale plan is rejected.
    bool stale_rejected = false;

    try {
        (void)execute_actions(plan, backend);
    } catch (const std::runtime_error&) {
        stale_rejected = true;
    }

    if (!stale_rejected) {
        std::cerr << "Stale backend plan was accepted.\n";
        return 1;
    }

    // Test 3: Incomplete plans are rejected.
    ActionPlan incomplete = plan;
    incomplete.unsupported_settings.push_back("desktop.theme");

    bool incomplete_rejected = false;

    try {
        (void)execute_actions(incomplete, backend);
    } catch (const std::invalid_argument&) {
        incomplete_rejected = true;
    }

    if (!incomplete_rejected) {
        std::cerr << "Incomplete backend plan was accepted.\n";
        return 1;
    }

    std::cout << "All backend executor tests passed.\n";
    return 0;
}
