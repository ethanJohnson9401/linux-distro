
#pragma once

#include "forma/config_error.hpp"

#include <ostream>

namespace forma {

    inline void print_config_error(
        const ConfigError& error,
        std::ostream& output
    ) {
        output << "Error: ";

        if (!error.file().empty()) {
            output << error.file();

            if (error.line() > 0) {
                output << ':' << error.line();

                if (error.column() > 0) {
                    output << ':' << error.column();
                }
            }

            output << '\n';
        }

        output << error.what() << '\n';
    }

} // namespace forma
