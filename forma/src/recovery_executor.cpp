
#include "forma/recovery_executor.hpp"

#include "forma/action_executor.hpp"

#include <exception>
#include <string>

namespace forma {

    ExecutionReport execute_with_report(
        const ActionPlan& plan,
        SystemBackend& backend
    ) {
        const SystemState initial = backend.read_state();

        try {
            const ExecutionResult result =
                execute_actions_with_rollback(plan, backend);

            return ExecutionReport{
                .status = ExecutionStatus::Success,
                .initial_state = initial,
                .final_state = result.final_state,
                .actions_completed = result.actions_applied,
                .message = "All actions completed successfully."
            };

        } catch (const std::exception& error) {
            const std::string message = error.what();

            SystemState final_state = initial;

            try {
                final_state = backend.read_state();
            } catch (const std::exception&) {
                return ExecutionReport{
                    .status = ExecutionStatus::FailedUnrecovered,
                    .initial_state = initial,
                    .final_state = initial,
                    .actions_completed = 0,
                    .message =
                        "Execution failed and final state could "
                        "not be verified: " + message
                };
            }

            const bool restored =
                final_state.hostname == initial.hostname;

            return ExecutionReport{
                .status = restored
                    ? ExecutionStatus::FailedRecovered
                    : ExecutionStatus::FailedUnrecovered,
                .initial_state = initial,
                .final_state = final_state,
                .actions_completed = 0,
                .message = message
            };
        }
    }

} // namespace forma
