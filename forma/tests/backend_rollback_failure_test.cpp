
#include "forma/action_executor.hpp"
#include "forma/system_backend.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

class RollbackFailingBackend final
    : public forma::SystemBackend {
public:
    explicit RollbackFailingBackend(
        forma::SystemState initial
    ) : state_(std::move(initial)) {}

    [[nodiscard]]
    forma::SystemState read_state() const override {
        return state_;
    }

    void set_hostname(
        const std::string& hostname
    ) override {
        ++write_count_;

        // Fail during the second action AND during rollback.
        if (write_count_ == 2 || write_count_ == 3) {
            throw std::runtime_error(
                "Injected backend failure."
            );
        }

        state_.hostname = hostname;
    }

    [[nodiscard]]
    int write_count() const noexcept {
        return write_count_;
    }

private:
    forma::SystemState state_;
    int write_count_ = 0;
};

} // namespace

int main() {
    using namespace forma;

    RollbackFailingBackend backend(
        SystemState{.hostname = "pop-os"}
    );

    const ActionPlan plan{
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

    bool rollback_failure_detected = false;

    try {
        (void)execute_actions_with_rollback(plan, backend);

    } catch (const std::runtime_error& error) {
        rollback_failure_detected =
            std::string(error.what()).find(
                "Execution failed and rollback also failed"
            ) != std::string::npos;
    }

    if (!rollback_failure_detected) {
        std::cerr
            << "Rollback failure was not reported.\n";
        return 1;
    }

    if (backend.write_count() != 3) {
        std::cerr
            << "Unexpected backend write count.\n";
        return 1;
    }

    // The system remains partially modified.
    if (backend.read_state().hostname != "forma-test") {
        std::cerr
            << "Unexpected state after rollback failure.\n";
        return 1;
    }

    std::cout
        << "Execution failure detected.\n"
        << "Rollback failure detected.\n"
        << "Partial state correctly identified.\n";

    return 0;
}
