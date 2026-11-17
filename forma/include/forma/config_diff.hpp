
#pragma once

#include "forma/config.hpp"
#include "forma/system_state.hpp"

#include <optional>
#include <string>

namespace forma {

    enum class ChangeStatus {
        Unspecified,
        Unchanged,
        Modified
    };

    struct HostnameChange {
        ChangeStatus status = ChangeStatus::Unspecified;
        std::string current_value;
        std::optional<std::string> requested_value;
    };

    struct ConfigDiff {
        HostnameChange hostname;
    };

    ConfigDiff calculate_diff(
        const FormaConfig& desired,
        const SystemState& current
    );

} // namespace forma
