
#pragma once

#include "forma/system_state.hpp"

#include <string>

namespace forma {

    // Abstract interface for reading and modifying system state.
    class SystemBackend {
    public:
        virtual ~SystemBackend() = default;

        [[nodiscard]]
        virtual SystemState read_state() const = 0;

        virtual void set_hostname(
            const std::string& hostname
        ) = 0;
    };

} // namespace forma
