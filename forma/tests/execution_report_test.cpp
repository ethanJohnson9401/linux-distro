
#include "forma/recovery_executor.hpp"
#include "forma/memory_backend.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

// Backend that can fail during execution and rollback.
class FailingBackend final : public forma::SystemBackend {
public:
    FailingBackend(
        forma::SystemState initial,
        bool fail_rollback
    )
        : state_(std::move(initial)),
          fail_rollback_(fail_rollback) {}

    [[nodiscard]]
    forma::SystemState read_state() const override {
        return state_;
    }

    void set_hostname(
        const std::string& hostname
    ) override {
        ++writes_;

        // Second write fails during execution.
        if (writes_ == 2) {
            throw std::runtime_error(
                "Injected execution failure."
            );
        }

        // Third write represents rollback.
        if (writes_ == 3 && fail_rollback_) {
            throw std::runtime_error(
                "Injected rollback failure."
            );
        }

        state_.hostname = hostname;
    }

private:
    forma::SystemState state_;
    bool fail_rollback_;
    int writes_ = 0;
};

forma::ActionPlan make_plan() {
    return forma::ActionPlan{
        .actions = {
            {
                .type = forma::ActionType::SetHostname,
                .current_value = "pop-os",
                .desired_value = "forma-test"
            },
            {
                .type = forma::ActionType::SetHostname,
                .current_value = "forma-test",
                .desired_value = "forma-desktop"
            }
        }
    };
}

} // namespace

int main() {
    using namespace forma;

    const ActionPlan plan = make_plan();

    // Test 1: Successful execution.
    {
        MemoryBackend backend(
            SystemState{.hostname = "pop-os"}
        );

        const ExecutionReport report =
            execute_with_report(plan, backend);

        if (report.status != ExecutionStatus::Success ||
            !report.succeeded() ||
            report.requires_attention() ||
            report.actions_completed != 2 ||
            report.final_state.hostname != "forma-desktop") {
            std::cerr << "Success report incorrect.\n";
            return 1;
        }
    }

    // Test 2: Execution failure with successful rollback.
    {
        FailingBackend backend(
            SystemState{.hostname = "pop-os"},
            false
        );

        const ExecutionReport report =
            execute_with_report(plan, backend);

        if (report.status !=
                ExecutionStatus::FailedRecovered ||
            report.succeeded() ||
            report.requires_attention() ||
            report.final_state.hostname != "pop-os") {
            std::cerr << "Recovery report incorrect.\n";
            return 1;
        }
    }

    // Test 3: Execution failure and rollback failure.
    {
        FailingBackend backend(
            SystemState{.hostname = "pop-os"},
            true
        );

        const ExecutionReport report =
            execute_with_report(plan, backend);

        if (report.status !=
                ExecutionStatus::FailedUnrecovered ||
            report.succeeded() ||
            !report.requires_attention() ||
            report.final_state.hostname != "forma-test") {
            std::cerr << "Unrecovered report incorrect.\n";
            return 1;
        }
    }

    std::cout
        << "Success report verified.\n"
        << "Recovered failure report verified.\n"
        << "Unrecovered failure report verified.\n"
        << "All execution report tests passed.\n";

    return 0;
}
