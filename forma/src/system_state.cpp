
#include "forma/system_state.hpp"

#include <array>
#include <cerrno>
#include <system_error>
#include <unistd.h>

namespace forma {

    SystemState read_system_state() {
        std::array<char, 256> buffer{};

        if (gethostname(buffer.data(), buffer.size()) != 0) {
            throw std::system_error(
                errno,
                std::generic_category(),
                "Failed to read system hostname"
            );
        }

        buffer.back() = '\0';

        return SystemState{
            .hostname = buffer.data()
        };
    }

} // namespace forma
