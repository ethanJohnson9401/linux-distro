
#include "forma/action_executor.hpp"
#include "forma/system_backend.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

// Test backend that deliberately fails on a chosen write.
class FailingBackend final : public forma::SystemBackend {
public:
    explicit FailingBackend(
        forma::SystemState initial_state,
        int fail_on_write
    )
        : state_(std::move(initial_state)),
          fail_on_write_(fail_on_write) {}

    [[nodiscard]]
    forma::SystemState read_state() const override {
        return state_;
    }

    void set_hostname(
        const std::string& hostname
    ) override {
        ++write_count_;

        if (write_count_ == fail_on_write_) {
            throw std::runtime_error(
                "Simulated backend write failure."
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
    int fail_on_write_;
    int write_count_ = 0;
};

} // namespace

int main() {
    using namespace forma;

    FailingBackend backend(
        SystemState{.hostname = "pop-os"},
        2
    );

    // Both actions are valid during preflight.
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

    bool failure_detected = false;

    try {
        (void)execute_actions(plan, backend);

    } catch (const std::runtime_error& error) {
        if (std::string(error.what()) !=
            "Simulated backend write failure.") {
            std::cerr
                << "Unexpected exception: "
                << error.what() << '\n';
            return 1;
        }

        failure_detected = true;
    }

    if (!failure_detected) {
        std::cerr << "Backend failure was not detected.\n";
        return 1;
    }

    if (backend.write_count() != 2) {
        std::cerr << "Unexpected number of write attempts.\n";
        return 1;
    }

    // Document the current partial-application behavior.
    if (backend.read_state().hostname != "forma-test") {
        std::cerr
            << "Unexpected state after backend failure.\n";
        return 1;
    }

    std::cout
        << "Backend failure correctly detected.\n"
        << "Partial application confirmed.\n"
        << "Recovery support is still required.\n";

    return 0;
}
