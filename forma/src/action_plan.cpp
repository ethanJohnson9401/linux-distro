
#include "forma/action_plan.hpp"

#include <stdexcept>

namespace forma {

    ActionPlan create_action_plan(
        const FormaConfig& config,
        const ConfigDiff& diff
    ) {
        ActionPlan plan;

        // Create supported hostname actions.
        if (diff.hostname.status == ChangeStatus::Modified) {
            if (!diff.hostname.requested_value.has_value()) {
                throw std::logic_error(
                    "Modified hostname is missing a desired value."
                );
            }

            plan.actions.push_back(Action{
                .type = ActionType::SetHostname,
                .current_value = diff.hostname.current_value,
                .desired_value = *diff.hostname.requested_value
            });
        }

        // Track requested settings that we cannot execute yet.
        if (config.desktop.theme.has_value()) {
            plan.unsupported_settings.push_back(
                "desktop.theme"
            );
        }

        if (config.gaming.enabled.has_value()) {
            plan.unsupported_settings.push_back(
                "gaming.enabled"
            );
        }

        return plan;
    }

} // namespace forma
