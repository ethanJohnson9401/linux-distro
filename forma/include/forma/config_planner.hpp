
#pragma once

#include "forma/config.hpp"
#include "forma/config_diff.hpp"

#include <iosfwd>

namespace forma {

    void print_plan(
        const FormaConfig& config,
        const ConfigDiff& diff,
        std::ostream& output
    );

} // namespace forma
