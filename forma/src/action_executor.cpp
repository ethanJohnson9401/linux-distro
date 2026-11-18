
#include "forma/action_executor.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

namespace forma {

// ============================================================
// Simulated execution
// ============================================================

ExecutionResult simulate_actions(
    const ActionPlan& plan,
    const SystemState& initial_state
) {
    if (plan.has_unsupported_settings()) {
        throw std::invalid_argument(
            "Cannot simulate a plan containing "
            "unsupported requested settings."
        );
    }

    // Operate exclusively on a copy of the initial state.
    ExecutionResult result{
        .final_state = initial_state,
        .actions_applied = 0
    };

    for (const Action& action : plan.actions) {
        switch (action.type) {
            case ActionType::SetHostname: {
                if (result.final_state.hostname !=
                    action.current_value) {
                    throw std::runtime_error(
                        "Simulated state does not match "
                        "the action's expected hostname."
                    );
                }

                if (action.desired_value.empty()) {
                    throw std::invalid_argument(
                        "Desired hostname cannot be empty."
                    );
                }

                result.final_state.hostname =
                    action.desired_value;

                ++result.actions_applied;
                break;
            }

            default:
                throw std::invalid_argument(
                    "Unsupported action type."
                );
        }
    }

    return result;
}

// ============================================================
// Backend-based execution
// ============================================================

ExecutionResult execute_actions(
    const ActionPlan& plan,
    SystemBackend& backend
) {
    if (plan.has_unsupported_settings()) {
        throw std::invalid_argument(
            "Cannot execute a plan containing "
            "unsupported requested settings."
        );
    }

    // Validate the entire action sequence before
    // performing any backend writes.
    const SystemState initial_state =
        backend.read_state();

    (void)simulate_actions(plan, initial_state);

    std::size_t actions_applied = 0;

    for (const Action& action : plan.actions) {
        switch (action.type) {
            case ActionType::SetHostname: {
                const SystemState current =
                    backend.read_state();

                if (current.hostname !=
                    action.current_value) {
                    throw std::runtime_error(
                        "Backend state does not match "
                        "the action's expected hostname."
                    );
                }

                if (action.desired_value.empty()) {
                    throw std::invalid_argument(
                        "Desired hostname cannot be empty."
                    );
                }

                backend.set_hostname(
                    action.desired_value
                );

                ++actions_applied;
                break;
            }

            default:
                throw std::invalid_argument(
                    "Unsupported action type."
                );
        }
    }

    return ExecutionResult{
        .final_state = backend.read_state(),
        .actions_applied = actions_applied
    };
}

// ============================================================
// Backend execution with best-effort rollback
// ============================================================

ExecutionResult execute_actions_with_rollback(
    const ActionPlan& plan,
    SystemBackend& backend
) {
    if (plan.has_unsupported_settings()) {
        throw std::invalid_argument(
            "Cannot execute a plan containing "
            "unsupported requested settings."
        );
    }

    // --------------------------------------------------------
    // Phase 1: Preflight validation
    // --------------------------------------------------------

    const SystemState initial_state =
        backend.read_state();

    // Validate the complete action sequence before
    // performing any writes.
    (void)simulate_actions(plan, initial_state);

    // Track successfully completed actions.
    std::vector<Action> completed;
    completed.reserve(plan.actions.size());

    try {
        // ----------------------------------------------------
        // Phase 2: Execute actions
        // ----------------------------------------------------

        for (const Action& action : plan.actions) {
            switch (action.type) {
                case ActionType::SetHostname: {
                    const SystemState current =
                        backend.read_state();

                    if (current.hostname !=
                        action.current_value) {
                        throw std::runtime_error(
                            "Backend state changed "
                            "during execution."
                        );
                    }

                    backend.set_hostname(
                        action.desired_value
                    );

                    // Record the action only after its
                    // backend operation returns successfully.
                    completed.push_back(action);

                    break;
                }

                default:
                    throw std::invalid_argument(
                        "Unsupported action type."
                    );
            }
        }

        // ----------------------------------------------------
        // Phase 3: Return the successful result
        // ----------------------------------------------------

        return ExecutionResult{
            .final_state = backend.read_state(),
            .actions_applied = completed.size()
        };

    } catch (...) {
        // ----------------------------------------------------
        // Phase 4: Recover from execution failure
        // ----------------------------------------------------

        // Preserve the original execution exception.
        const std::exception_ptr original_error =
            std::current_exception();

        try {
            // Reverse completed actions in reverse order.
            for (auto it = completed.rbegin();
                 it != completed.rend();
                 ++it) {

                const Action& action = *it;

                // Do not knowingly overwrite state that
                // differs from the expected post-action value.
                const SystemState current =
                    backend.read_state();

                if (current.hostname !=
                    action.desired_value) {
                    throw std::runtime_error(
                        "Cannot safely roll back: "
                        "backend state has changed."
                    );
                }

                // Restore the hostname that existed before
                // this particular action.
                backend.set_hostname(
                    action.current_value
                );
            }

            // Verify that the original hostname was restored.
            const SystemState restored =
                backend.read_state();

            if (restored.hostname !=
                initial_state.hostname) {
                throw std::runtime_error(
                    "Rollback verification failed: "
                    "initial hostname was not restored."
                );
            }

        } catch (const std::exception& rollback_error) {
            throw std::runtime_error(
                std::string(
                    "Execution failed and rollback also failed: "
                ) + rollback_error.what()
            );
        }

        // Rollback succeeded. Propagate the original error.
        std::rethrow_exception(original_error);
    }
}

} // namespace forma
