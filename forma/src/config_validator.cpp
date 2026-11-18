
#include "forma/config_validator.hpp"

#include "forma/config_diagnostics.hpp"
#include "forma/config_error.hpp"
#include "forma/config_loader.hpp"

#include <exception>
#include <iostream>
#include <string>

namespace forma {

    bool validate_config(
        const std::string& path,
        bool verbose
    ) {
        try {
            // Loading includes full validation.
            // No need to retain the configuration object.
            (void)load_config(path);

            if (verbose) {
                std::cout
                    << "Configuration valid: "
                    << path << '\n';
            }

            return true;

        } catch (const ConfigError& error) {
            print_config_error(error, std::cerr);
            return false;

        } catch (const std::exception& error) {
            std::cerr
                << "Error: "
                << error.what()
                << '\n';

            return false;
        }
    }

} // namespace forma
