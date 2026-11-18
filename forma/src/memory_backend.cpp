
#include "forma/memory_backend.hpp"

#include <stdexcept>
#include <utility>

namespace forma {

    MemoryBackend::MemoryBackend(SystemState initial_state)
        : state_(std::move(initial_state)) {}

    SystemState MemoryBackend::read_state() const {
        return state_;
    }

    void MemoryBackend::set_hostname(
        const std::string& hostname
    ) {
        if (hostname.empty()) {
            throw std::invalid_argument(
                "Hostname cannot be empty."
            );
        }

        state_.hostname = hostname;
    }

} // namespace forma
