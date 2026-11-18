
#include "forma/config_error.hpp"
#include "forma/config_loader.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <string_view>

namespace {

bool expect_error(
    const std::string& path,
    forma::ConfigErrorCode expected_code,
    std::string_view expected_message
) {
    try {
        (void)forma::load_config(path);

        std::cerr
            << "Expected an error for: "
            << path << '\n';

        return false;

    } catch (const forma::ConfigError& error) {
        if (error.code() != expected_code) {
            std::cerr
                << "Incorrect error code for: "
                << path << '\n';

            return false;
        }

        if (std::string_view(error.what()).find(
                expected_message
            ) == std::string_view::npos) {
            std::cerr
                << "Incorrect error message: "
                << error.what() << '\n';

            return false;
        }

        std::cout
            << "Verified: "
            << error.what() << '\n';

        return true;

    } catch (const std::exception& error) {
        std::cerr
            << "Unexpected exception type: "
            << error.what() << '\n';

        return false;
    }
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr
            << "Usage: config_error_test "
            << "DUPLICATE HOSTNAME THEME INVALID_YAML\n";

        return 1;
    }

    bool passed = true;

    passed = expect_error(
        argv[1],
        forma::ConfigErrorCode::DuplicateSetting,
        "duplicate setting 'system.hostname'"
    ) && passed;

    passed = expect_error(
        argv[2],
        forma::ConfigErrorCode::InvalidValue,
        "invalid hostname"
    ) && passed;

    passed = expect_error(
        argv[3],
        forma::ConfigErrorCode::InvalidValue,
        "theme must be dark or light"
    ) && passed;

    passed = expect_error(
        argv[4],
        forma::ConfigErrorCode::YamlError,
        "YAML error:"
    ) && passed;

    if (!passed) {
        return 1;
    }

    std::cout
        << "All typed configuration error tests passed.\n";

    return 0;
}
