
#pragma once

#include "forma/system_state.hpp"

#include <cstddef>
#include <string>

namespace forma {

    enum class ExecutionStatus {
        Success,
        FailedRecovered,
        FailedUnrecovered
    };

    struct ExecutionReport {
        ExecutionStatus status;

        SystemState initial_state;
        SystemState final_state;

        std::size_t actions_completed = 0;

        std::string message;

        [[nodiscard]]
        bool succeeded() const noexcept {
            return status == ExecutionStatus::Success;
        }

        [[nodiscard]]
        bool requires_attention() const noexcept {
            return status == ExecutionStatus::FailedUnrecovered;
        }
    };

} // namespace forma
