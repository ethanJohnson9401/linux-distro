
#pragma once

#include "forma/system_inspector.hpp"

#include <iosfwd>

namespace forma {

    // Formats a system information snapshot for terminal output.
    void print_system_info(
        const SystemInfo& info,
        std::ostream& output
    );

} // namespace forma
