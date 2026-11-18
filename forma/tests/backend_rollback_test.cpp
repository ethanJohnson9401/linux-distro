
#include "forma/action_executor.hpp"
#include "forma/system_backend.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

// ============================================================
// Failure-injection backend
// ============================================================

class FailingBackend final : public forma::SystemBackend {
public:
    explicit FailingBackend(
        forma::SystemState initial
    )
        : state_(std::move(initial)) {}

    [[nodiscard]]
    forma::SystemState read_state() const override {
        return state_;
    }

    void set_hostname(
        const std::string& hostname
    ) override {
        ++write_count_;

        // Simulate a failure on the second write.
        if (write_count_ == 2) {
            throw std::runtime_error(
                "Injected write failure."
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

// ============================================================
// Rollback regression tests
// ============================================================

int main() {
    using namespace forma;

    FailingBackend backend(
        SystemState{
            .hostname = "pop-os"
        }
    );

    // The first action succeeds.
    // The second action fails during backend execution.
    const ActionPlan plan{
        .actions = {
            Action{
                .type = ActionType::SetHostname,
                .current_value = "pop-os",
                .desired_value = "forma-test"
            },
            Action{
                .type = ActionType::SetHostname,
                .current_value = "forma-test",
                .desired_value = "forma-desktop"
            }
        }
    };

    // --------------------------------------------------------
    // Test 1: Original execution failure is propagated
    // --------------------------------------------------------

    bool failure_detected = false;

    try {
        (void)execute_actions_with_rollback(
            plan,
            backend
        );

    } catch (const std::runtime_error& error) {
        failure_detected =
            std::string(error.what()) ==
            "Injected write failure.";
    }

    if (!failure_detected) {
        std::cerr
            << "Original execution failure "
            << "was not propagated.\n";

        return 1;
    }

    // --------------------------------------------------------
    // Test 2: Verify execution and rollback writes
    // --------------------------------------------------------

    // Write 1: pop-os -> forma-test
    // Write 2: injected failure
    // Write 3: forma-test -> pop-os
    if (backend.write_count() != 3) {
        std::cerr
            << "Expected three backend write attempts.\n"
            << "Actual: "
            << backend.write_count()
            << '\n';

        return 1;
    }

    // --------------------------------------------------------
    // Test 3: Verify original state restoration
    // --------------------------------------------------------

    const SystemState restored =
        backend.read_state();

    if (restored.hostname != "pop-os") {
        std::cerr
            << "Rollback failed to restore "
            << "the initial hostname.\n"
            << "Actual hostname: "
            << restored.hostname
            << '\n';

        return 1;
    }

    std::cout
        << "Backend failure detected.\n"
        << "Completed action rolled back successfully.\n"
        << "Original system state restored.\n"
        << "All rollback tests passed.\n";

    return 0;
}
