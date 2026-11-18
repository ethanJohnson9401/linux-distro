
#include "forma/system_inspector.hpp"

#include <sys/utsname.h>
#include <unistd.h>

#include <array>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace forma {

    namespace {

        // Read PRETTY_NAME from /etc/os-release.
        std::string read_os_name() {
            std::ifstream file("/etc/os-release");

            if (!file.is_open()) {
                return "Unknown Linux distribution";
            }

            std::string line;

            while (std::getline(file, line)) {
                constexpr std::string_view key = "PRETTY_NAME=";

                if (line.rfind(key, 0) == 0) {
                    std::string value = line.substr(key.size());

                    if (value.size() >= 2 &&
                        value.front() == '"' &&
                        value.back() == '"') {
                        value = value.substr(1, value.size() - 2);
                        }

                    return value;
                }
            }

            return "Unknown Linux distribution";
        }

    } // namespace

    SystemInfo LinuxSystemInspector::inspect() const {
        std::array<char, 256> hostname_buffer{};

        if (gethostname(
                hostname_buffer.data(),
                hostname_buffer.size() - 1
            ) != 0) {
            throw std::runtime_error(
                "Failed to read system hostname."
            );
            }

        struct utsname system_data {};

        if (uname(&system_data) != 0) {
            throw std::runtime_error(
                "Failed to read Linux kernel information."
            );
        }

        return SystemInfo{
            .hostname = hostname_buffer.data(),
            .operating_system = read_os_name(),
            .architecture = system_data.machine,
            .kernel = std::string(system_data.sysname) +
                      " " + system_data.release
        };
    }

} // namespace forma
