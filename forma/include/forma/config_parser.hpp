
#pragma once

#include "forma/config.hpp"

#include <string>

namespace forma {

    // Loads a YAML configuration into a typed C++ object.
    // Throws an exception if the file cannot be parsed or validated.
    FormaConfig parse_config(const std::string& path);

} // namespace forma
