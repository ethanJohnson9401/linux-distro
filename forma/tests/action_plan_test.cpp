
#include "forma/action_plan.hpp"

#include <iostream>

int main() {
    using namespace forma;

    const SystemState current{
        .hostname = "pop-os"
    };

    // Test 1: Modified hostname produces one action.
    FormaConfig modified;
    modified.system.hostname = "forma-laptop";

    const ConfigDiff modified_diff =
        calculate_diff(modified, current);

    const ActionPlan modified_plan =
        create_action_plan(modified ,modified_diff);

    if (modified_plan.size() != 1) {
        std::cerr << "Expected exactly one action.\n";
        return 1;
    }

    const Action& action = modified_plan.actions.front();

    if (action.type != ActionType::SetHostname ||
        action.current_value != "pop-os" ||
        action.desired_value != "forma-laptop") {
        std::cerr << "Incorrect hostname action.\n";
        return 1;
        }

    // Test 2: Matching hostname produces no actions.
    FormaConfig matching;
    matching.system.hostname = "pop-os";

    const ActionPlan matching_plan =
        create_action_plan(
            matching,
            calculate_diff(matching, current)
        );

    if (!matching_plan.empty()) {
        std::cerr << "Matching hostname created an action.\n";
        return 1;
    }

    // Test 3: Unspecified hostname produces no actions.
    FormaConfig unspecified;

    const ActionPlan unspecified_plan =
        create_action_plan(
            unspecified,
            calculate_diff(unspecified, current)
        );

    if (!unspecified_plan.empty()) {
        std::cerr << "Unspecified hostname created an action.\n";
        return 1;
    }


    // Test 4: Unsupported requested settings are tracked.
    FormaConfig unsupported;
    unsupported.desktop.theme = Theme::Dark;
    unsupported.gaming.enabled = true;

    const ActionPlan unsupported_plan =
        create_action_plan(
            unsupported,
            calculate_diff(unsupported, current)
        );

    if (!unsupported_plan.empty() ||
        unsupported_plan.unsupported_settings.size() != 2 ||
        unsupported_plan.unsupported_settings[0] != "desktop.theme" ||
        unsupported_plan.unsupported_settings[1] != "gaming.enabled") {
        std::cerr
            << "Unsupported settings were not tracked correctly.\n";
        return 1;
        }


    std::cout << "All action plan tests passed.\n";
    return 0;
}
