
#pragma once

#include <string>

namespace forma {

    // Snapshot of basic operating-system information.
    struct SystemInfo {
        std::string hostname;
        std::string operating_system;
        std::string architecture;
        std::string kernel;
    };

    // Abstract interface for system inspection.
    // Allows real and mock implementations.
    class SystemInspector {
    public:
        virtual ~SystemInspector() = default;

        [[nodiscard]]
        virtual SystemInfo inspect() const = 0;
    };

    // Linux implementation.
    // Collects information without modifying the system.
    class LinuxSystemInspector final : public SystemInspector {
    public:
        [[nodiscard]]
        SystemInfo inspect() const override;
    };

} // namespace forma
