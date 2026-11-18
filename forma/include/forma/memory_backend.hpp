
#pragma once

#include "forma/system_backend.hpp"

namespace forma {

    // A safe backend that stores all changes in memory.
    class MemoryBackend final : public SystemBackend {
    public:
        explicit MemoryBackend(SystemState initial_state);

        [[nodiscard]]
        SystemState read_state() const override;

        void set_hostname(
            const std::string& hostname
        ) override;

    private:
        SystemState state_;
    };

} // namespace forma
