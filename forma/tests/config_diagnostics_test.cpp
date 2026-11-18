
#include "forma/config_diagnostics.hpp"
#include "forma/config_error.hpp"
#include "forma/config_loader.hpp"

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {

bool test_error(
    const std::string& file,
    forma::ConfigErrorCode expected_code,
    int expected_line,
    int expected_column,
    std::string_view expected_message
) {
    try {
        (void)forma::load_config(file);

        std::cerr << "Expected error: " << file << '\n';
        return false;

    } catch (const forma::ConfigError& error) {
        if (error.code() != expected_code ||
            error.file() != file ||
            error.line() != expected_line ||
            error.column() != expected_column) {
            std::cerr
                << "Incorrect error metadata for: "
                << file << '\n';
            return false;
        }

        if (std::string_view(error.what()).find(
                expected_message
            ) == std::string_view::npos) {
            std::cerr << "Incorrect error message.\n";
            return false;
        }

        std::ostringstream output;
        forma::print_config_error(error, output);

        const std::string expected_output =
            "Error: " + file + ":" +
            std::to_string(expected_line) + ":" +
            std::to_string(expected_column) + "\n" +
            error.what() + "\n";

        if (output.str() != expected_output) {
            std::cerr
                << "Diagnostic formatting mismatch.\n"
                << "Expected:\n" << expected_output
                << "Actual:\n" << output.str();
            return false;
        }

        std::cout
            << "Verified diagnostic: "
            << file << '\n';

        return true;

    } catch (const std::exception& error) {
        std::cerr
            << "Unexpected exception: "
            << error.what() << '\n';
        return false;
    }
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr
            << "Usage: config_diagnostics_test "
            << "THEME HOSTNAME DUPLICATE\n";
        return 1;
    }

    bool passed = true;

    passed = test_error(
        argv[1],
        forma::ConfigErrorCode::InvalidValue,
        2,
        10,
        "theme must be dark or light."
    ) && passed;

    passed = test_error(
        argv[2],
        forma::ConfigErrorCode::InvalidValue,
        2,
        13,
        "invalid hostname"
    ) && passed;

    passed = test_error(
        argv[3],
        forma::ConfigErrorCode::DuplicateSetting,
        3,
        3,
        "duplicate setting 'system.hostname'"
    ) && passed;

    if (!passed) {
        return 1;
    }

    std::cout
        << "All diagnostic tests passed.\n";
    return 0;
}
