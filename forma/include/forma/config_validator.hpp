
#pragma once

#include <string>

namespace forma {

    // Validate a Forma configuration file.
    // When verbose is true, print a success message.
    bool validate_config(
        const std::string& path,
        bool verbose = true
    );

} // namespace forma
