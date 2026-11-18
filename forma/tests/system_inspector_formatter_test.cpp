
#include "forma/system_inspector_formatter.hpp"

#include <iostream>
#include <sstream>
#include <string>

int main() {
    using namespace forma;

    // Use controlled data instead of relying on the
    // machine running the tests.
    const SystemInfo info{
        .hostname = "forma-test",
        .operating_system = "Forma Linux",
        .architecture = "x86_64",
        .kernel = "Linux 6.0.0-test"
    };

    std::ostringstream output;

    print_system_info(info, output);

    const std::string result = output.str();

    const std::string expected_values[] = {
        "Forma Linux - System Inspection",
        "System",
        "Hostname:",
        "forma-test",
        "Operating system:",
        "Forma Linux",
        "Architecture:",
        "x86_64",
        "Kernel:",
        "Linux 6.0.0-test",
        "Forma Core",
        "0.1.0",
        "Read-only inspection"
    };

    for (const std::string& value : expected_values) {
        if (result.find(value) == std::string::npos) {
            std::cerr
                << "Missing expected output: "
                << value
                << '\n';

            return 1;
        }
    }

    std::cout
        << "System inspection formatter verified.\n"
        << "All expected fields displayed.\n"
        << "Formatter test passed.\n";

    return 0;
}
