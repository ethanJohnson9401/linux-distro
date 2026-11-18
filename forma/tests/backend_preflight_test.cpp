
#include "forma/action_executor.hpp"
#include "forma/memory_backend.hpp"

#include <iostream>
#include <stdexcept>

int main() {
    using namespace forma;

    MemoryBackend backend(
        SystemState{.hostname = "pop-os"}
    );

    // First action is valid.
    // Second action is stale.
    const ActionPlan invalid_plan{
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

    bool rejected = false;

    try {
        (void)execute_actions(invalid_plan, backend);
    } catch (const std::runtime_error&) {
        rejected = true;
    }

    if (!rejected) {
        std::cerr << "Invalid plan was accepted.\n";
        return 1;
    }

    // Preflight must prevent the first action
    // from modifying the backend.
    if (backend.read_state().hostname != "pop-os") {
        std::cerr
            << "Backend was modified before "
            << "preflight completed.\n";
        return 1;
    }

    std::cout
        << "Backend preflight validation passed.\n";

    return 0;
}
