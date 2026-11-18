
#include "forma/config_loader.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

bool expect_error(
    const std::string& path,
    std::string_view expected_message
) {
    try {
        (void)forma::load_config(path);

        std::cerr
            << "Expected an error for: "
            << path << '\n';

        return false;

    } catch (const std::runtime_error& error) {
        const std::string message = error.what();

        if (message.find(expected_message) ==
            std::string::npos) {
            std::cerr
                << "Unexpected error: "
                << message << '\n';

            return false;
        }

        return true;
    }
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr
            << "Usage: config_loader_test "
            << "VALID DUPLICATE HOSTNAME THEME\n";

        return 1;
    }

    try {
        // Test 1: Valid configuration
        const forma::FormaConfig config =
            forma::load_config(argv[1]);

        if (!config.system.hostname.has_value() ||
            *config.system.hostname != "forma-pc") {
            std::cerr << "Hostname mismatch.\n";
            return 1;
        }

        if (!config.desktop.theme.has_value() ||
            *config.desktop.theme != forma::Theme::Dark) {
            std::cerr << "Theme mismatch.\n";
            return 1;
        }

        if (!config.gaming.enabled.has_value() ||
            !*config.gaming.enabled) {
            std::cerr << "Gaming setting mismatch.\n";
            return 1;
        }

        // Test 2: Duplicate setting
        if (!expect_error(
                argv[2],
                "duplicate setting 'system.hostname'")) {
            return 1;
        }

        // Test 3: Invalid hostname
        if (!expect_error(
                argv[3],
                "invalid hostname")) {
            return 1;
        }

        // Test 4: Invalid theme
        if (!expect_error(
                argv[4],
                "theme must be dark or light")) {
            return 1;
        }

        std::cout
            << "All unified loader tests passed.\n";

        return 0;

    } catch (const std::exception& error) {
        std::cerr
            << "Unexpected exception: "
            << error.what() << '\n';

        return 1;
    }
}
