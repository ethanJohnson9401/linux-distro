
#include "forma/action_executor.hpp"
#include "forma/action_plan.hpp"
#include "forma/action_plan_formatter.hpp"
#include "forma/config_diagnostics.hpp"
#include "forma/config_diff.hpp"
#include "forma/config_error.hpp"
#include "forma/config_parser.hpp"
#include "forma/config_validator.hpp"
#include "forma/system_state.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string_view>

namespace {

constexpr std::string_view kVersion = "0.1.0";

// ============================================================
// CLI help
// ============================================================

void print_help() {
    std::cout
        << "Forma Core - Declarative configuration for Forma Linux\n\n"
        << "Usage: forma [options] [command] [file]\n\n"
        << "Options:\n"
        << "  -h, --help       Show help\n"
        << "  -v, --version    Show version\n\n"
        << "Commands:\n"
        << "  validate FILE    Validate a YAML configuration\n"
        << "  plan FILE        Preview proposed system actions\n"
        << "  simulate FILE    Simulate proposed system actions\n";
}

// ============================================================
// Validate command
// ============================================================

int run_validate(const char* path) {
    return forma::validate_config(path)
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}

// ============================================================
// Shared configuration planning
// ============================================================

struct PlanningContext {
    forma::SystemState current;
    forma::ActionPlan plan;
};

PlanningContext build_plan(const char* path) {
    // Load and validate the configuration.
    const forma::FormaConfig config =
        forma::parse_config(path);

    // Read the current hostname.
    const forma::SystemState current =
        forma::read_system_state();

    // Calculate requested changes.
    const forma::ConfigDiff diff =
        forma::calculate_diff(config, current);

    // Convert supported differences into actions.
    const forma::ActionPlan plan =
        forma::create_action_plan(config, diff);

    return PlanningContext{
        .current = current,
        .plan = plan
    };
}

// ============================================================
// Plan command
// ============================================================

int run_plan(const char* path) {
    try {
        const PlanningContext context =
            build_plan(path);

        forma::print_action_plan(
            context.plan,
            std::cout
        );

        return EXIT_SUCCESS;

    } catch (const forma::ConfigError& error) {
        forma::print_config_error(error, std::cerr);
        return EXIT_FAILURE;

    } catch (const std::exception& error) {
        std::cerr
            << "Error: "
            << error.what()
            << '\n';

        return EXIT_FAILURE;
    }
}

// ============================================================
// Simulate command
// ============================================================

int run_simulate(const char* path) {
    try {
        // Build a plan without changing the system.
        const PlanningContext context =
            build_plan(path);

        // Execute actions against an in-memory copy.
        const forma::ExecutionResult result =
            forma::simulate_actions(
                context.plan,
                context.current
            );

        // Display the action plan.
        forma::print_action_plan(
            context.plan,
            std::cout
        );

        // Display the simulated outcome.
        std::cout
            << "\nSimulation Result\n"
            << "=================\n"
            << "Initial hostname: "
            << context.current.hostname << '\n'
            << "Final hostname:   "
            << result.final_state.hostname << '\n'
            << "Actions simulated: "
            << result.actions_applied << '\n'
            << "\n"
            << "Simulation only. "
            << "No system changes were made.\n";

        return EXIT_SUCCESS;

    } catch (const forma::ConfigError& error) {
        forma::print_config_error(error, std::cerr);
        return EXIT_FAILURE;

    } catch (const std::exception& error) {
        std::cerr
            << "Error: "
            << error.what()
            << '\n';

        return EXIT_FAILURE;
    }
}

} // namespace

// ============================================================
// Program entry point
// ============================================================

int main(int argc, char* argv[]) {
    if (argc == 1) {
        print_help();
        return EXIT_SUCCESS;
    }

    const std::string_view argument{argv[1]};

    if (argument == "--help" || argument == "-h") {
        print_help();
        return EXIT_SUCCESS;
    }

    if (argument == "--version" || argument == "-v") {
        std::cout
            << "Forma Core "
            << kVersion
            << '\n';

        return EXIT_SUCCESS;
    }

    if (argument == "validate") {
        if (argc != 3) {
            std::cerr
                << "Usage: forma validate FILE\n";

            return EXIT_FAILURE;
        }

        return run_validate(argv[2]);
    }

    if (argument == "plan") {
        if (argc != 3) {
            std::cerr
                << "Usage: forma plan FILE\n";

            return EXIT_FAILURE;
        }

        return run_plan(argv[2]);
    }

    if (argument == "simulate") {
        if (argc != 3) {
            std::cerr
                << "Usage: forma simulate FILE\n";

            return EXIT_FAILURE;
        }

        return run_simulate(argv[2]);
    }

    std::cerr
        << "Unknown argument: "
        << argument << '\n'
        << "Run 'forma --help' "
        << "for usage information.\n";

    return EXIT_FAILURE;
}
