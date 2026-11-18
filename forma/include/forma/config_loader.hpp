
#pragma once

#include "forma/config.hpp"

#include <string>

namespace forma {

    // Loads, validates, and parses a configuration.
    // Throws std::runtime_error for invalid configuration.
    FormaConfig load_config(const std::string& path);

} // namespace forma
