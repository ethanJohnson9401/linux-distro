
#pragma once

#include "forma/action_plan.hpp"
#include "forma/execution_report.hpp"
#include "forma/system_backend.hpp"

namespace forma {

    ExecutionReport execute_with_report(
        const ActionPlan& plan,
        SystemBackend& backend
    );

} // namespace forma
