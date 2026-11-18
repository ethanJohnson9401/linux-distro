
#pragma once

#include "forma/action_plan.hpp"
#include "forma/system_backend.hpp"
#include "forma/system_state.hpp"

#include <cstddef>

namespace forma {

    // Result of executing or simulating an action plan.
    struct ExecutionResult {
        SystemState final_state;
        std::size_t actions_applied = 0;
    };

    // Execute actions against an in-memory copy of state.
    // Never modifies the host operating system.
    ExecutionResult simulate_actions(
        const ActionPlan& plan,
        const SystemState& initial_state
    );

    // Execute an action plan against a supplied backend.
    // This function does not provide rollback.
    ExecutionResult execute_actions(
        const ActionPlan& plan,
        SystemBackend& backend
    );

    // Execute actions with best-effort rollback.
    //
    // If an action fails, previously completed actions
    // are reversed in reverse order.
    //
    // This does NOT guarantee atomicity or recovery
    // if a backend modifies state before throwing,
    // rollback fails, or another process changes state.
    ExecutionResult execute_actions_with_rollback(
        const ActionPlan& plan,
        SystemBackend& backend
    );

} // namespace forma
