
#include "forma/config_parser.hpp"
#include "forma/config_validator.hpp"

#include <yaml-cpp/yaml.h>

#include <stdexcept>
#include <string>

namespace forma {

    FormaConfig parse_config(const std::string& path) {
        // Keep the existing validator as the source of truth
        // until parsing and validation are consolidated.
        if (!validate_config(path, false)) {
            throw std::runtime_error("Invalid Forma configuration: " + path);
        }

        const YAML::Node root = YAML::LoadFile(path);

        FormaConfig config;

        if (const YAML::Node system = root["system"]) {
            if (const YAML::Node hostname = system["hostname"]) {
                config.system.hostname = hostname.as<std::string>();
            }
        }

        if (const YAML::Node desktop = root["desktop"]) {
            if (const YAML::Node theme = desktop["theme"]) {
                const std::string value = theme.as<std::string>();
                config.desktop.theme =
                    value == "light" ? Theme::Light : Theme::Dark;
            }
        }

        if (const YAML::Node gaming = root["gaming"]) {
            if (const YAML::Node enabled = gaming["enabled"]) {
                config.gaming.enabled = enabled.as<bool>();
            }
        }

        return config;
    }

} // namespace forma
