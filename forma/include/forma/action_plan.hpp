
#pragma once

#include "forma/config.hpp"
#include "forma/config_diff.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace forma {

    enum class ActionType {
        SetHostname
    };

    struct Action {
        ActionType type;
        std::string current_value;
        std::string desired_value;
    };

    struct ActionPlan {
        std::vector<Action> actions;

        // Settings requested in YAML but not yet supported
        // by the action execution engine.
        std::vector<std::string> unsupported_settings;

        [[nodiscard]]
        bool empty() const noexcept {
            return actions.empty();
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return actions.size();
        }

        [[nodiscard]]
        bool has_unsupported_settings() const noexcept {
            return !unsupported_settings.empty();
        }
    };

    ActionPlan create_action_plan(
        const FormaConfig& config,
        const ConfigDiff& diff
    );

} // namespace forma
