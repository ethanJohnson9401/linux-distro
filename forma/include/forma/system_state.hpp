
#pragma once

#include <string>

namespace forma {

    struct SystemState {
        std::string hostname;
    };

    // Reads the current hostname without modifying the system.
    // Throws std::system_error if hostname detection fails.
    SystemState read_system_state();

} // namespace forma
