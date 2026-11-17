
#include "forma/config_diff.hpp"

namespace forma {

    ConfigDiff calculate_diff(
        const FormaConfig& desired,
        const SystemState& current
    ) {
        ConfigDiff diff;

        diff.hostname.current_value = current.hostname;
        diff.hostname.requested_value = desired.system.hostname;

        if (!desired.system.hostname.has_value()) {
            diff.hostname.status = ChangeStatus::Unspecified;
        } else if (*desired.system.hostname == current.hostname) {
            diff.hostname.status = ChangeStatus::Unchanged;
        } else {
            diff.hostname.status = ChangeStatus::Modified;
        }

        return diff;
    }

} // namespace forma
