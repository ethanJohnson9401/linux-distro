
#include "forma/action_plan_formatter.hpp"

#include <cstddef>
#include <ostream>

namespace forma {

    void print_action_plan(
        const ActionPlan& plan,
        std::ostream& output
    ) {
        output << "Forma Action Plan\n"
               << "=================\n\n";

        if (plan.empty()) {
            output
                << "No supported configuration changes are required.\n\n";
        }

        for (std::size_t i = 0; i < plan.actions.size(); ++i) {
            const Action& action = plan.actions[i];

            output << '[' << i + 1 << "] ";

            switch (action.type) {
                case ActionType::SetHostname:
                    output << "Set hostname\n";
                    break;
            }

            output
                << "    Current: "
                << action.current_value << '\n'
                << "    Desired: "
                << action.desired_value << "\n\n";
        }

        output
            << "Total actions: "
            << plan.size() << "\n\n"
            << "Only supported settings are included in this plan.\n"
            << "No changes have been applied.\n";


        if (plan.has_unsupported_settings()) {
            output << "Unsupported requested settings:\n";

            for (const std::string& setting :
                 plan.unsupported_settings) {
                output << "  - " << setting << '\n';
                 }

            output << '\n'
                   << "This plan is incomplete.\n\n";
        }

    }

} // namespace forma
