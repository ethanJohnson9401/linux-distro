
#include "forma/action_executor.hpp"
#include "forma/action_plan.hpp"
#include "forma/config_diff.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>

int main() {
    using namespace forma;

    const SystemState initial{
        .hostname = "pop-os"
    };

    // Request a supported hostname change and an
    // unsupported desktop setting.
    FormaConfig config;
    config.system.hostname = "forma-desktop";
    config.desktop.theme = Theme::Dark;

    const ConfigDiff diff =
        calculate_diff(config, initial);

    const ActionPlan plan =
        create_action_plan(config, diff);

    // Verify both the supported action and the
    // unsupported setting are represented.
    if (plan.size() != 1 ||
        !plan.has_unsupported_settings() ||
        plan.unsupported_settings.size() != 1 ||
        plan.unsupported_settings.front() != "desktop.theme") {
        std::cerr << "Incomplete plan was not constructed correctly.\n";
        return 1;
        }

    // The simulator must reject the entire plan.
    bool rejected = false;

    try {
        (void)simulate_actions(plan, initial);

    } catch (const std::invalid_argument& error) {
        rejected = true;

        if (std::string_view(error.what()).find(
                "unsupported requested settings"
            ) == std::string_view::npos) {
            std::cerr << "Unexpected rejection reason.\n";
            return 1;
            }
    }

    if (!rejected) {
        std::cerr << "Incomplete plan was accepted.\n";
        return 1;
    }

    // The original state must remain untouched.
    if (initial.hostname != "pop-os") {
        std::cerr << "Original state was modified.\n";
        return 1;
    }

    std::cout << "Incomplete-plan rejection test passed.\n";
    return 0;
}
