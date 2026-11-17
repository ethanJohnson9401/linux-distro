
#include "forma/config_validator.hpp"
#include "forma/config_parser.hpp"
#include "forma/config_planner.hpp"
#include "forma/config_diff.hpp"
#include "forma/system_state.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>
#include <exception>

namespace {

    constexpr std::string_view kVersion = "0.1.0";

    void print_help() {
        std::cout
            << "Forma Core - Declarative configuration for Forma Linux\n\n"
            << "Usage: forma [options] [command] [file]\n\n"
            << "Options:\n"
            << "  -h, --help       Show help\n"
            << "  -v, --version    Show version\n\n"
            << "Commands:\n"
            << "  validate FILE    Validate a YAML configuration\n"
            << "  plan FILE        Preview requested configuration\n";
    }

} // namespace

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
        std::cout << "Forma Core " << kVersion << '\n';
        return EXIT_SUCCESS;
    }

    if (argument == "validate") {
        if (argc != 3) {
            std::cerr << "Usage: forma validate FILE\n";
            return EXIT_FAILURE;
        }

        return forma::validate_config(argv[2])
                   ? EXIT_SUCCESS
                   : EXIT_FAILURE;
    }


    if (argument == "plan") {
        if (argc != 3) {
            std::cerr << "Usage: forma plan FILE\n";
            return EXIT_FAILURE;
        }

        try {

            const forma::FormaConfig config =
                forma::parse_config(argv[2]);

            const forma::SystemState current =
                forma::read_system_state();

            const forma::ConfigDiff diff =
                forma::calculate_diff(config, current);

            forma::print_plan(config, diff, std::cout);

            return EXIT_SUCCESS;


        } catch (const std::exception& error) {
            std::cerr << "Error: " << error.what() << '\n';
            return EXIT_FAILURE;
        }
    }


    std::cerr << "Unknown argument: " << argument << '\n';
    std::cerr << "Run 'forma --help' for usage information.\n";
    return EXIT_FAILURE;
}
