
#include "forma/config_parser.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: config_parser_test FILE\n";
        return 1;
    }

    try {
        const forma::FormaConfig config =
            forma::parse_config(argv[1]);

        if (!config.system.hostname.has_value() ||
            *config.system.hostname != "forma-pc") {
            std::cerr << "Hostname parsing failed.\n";
            return 1;
            }

        if (!config.desktop.theme.has_value() ||
            *config.desktop.theme != forma::Theme::Dark) {
            std::cerr << "Theme parsing failed.\n";
            return 1;
            }

        if (!config.gaming.enabled.has_value() ||
            *config.gaming.enabled != true) {
            std::cerr << "Gaming setting parsing failed.\n";
            return 1;
            }

        std::cout << "All configuration parser checks passed.\n";
        return 0;

    } catch (const std::exception& error) {
        std::cerr << "Parser error: " << error.what() << '\n';
        return 1;
    }
}
