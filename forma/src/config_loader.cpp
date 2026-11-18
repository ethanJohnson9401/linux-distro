
#include "forma/config_loader.hpp"
#include "forma/config_error.hpp"

#include <yaml-cpp/yaml.h>

#include <string>
#include <string_view>
#include <unordered_set>

namespace forma {
namespace {

// ============================================================
// Hostname validation
// ============================================================

bool is_valid_hostname(std::string_view hostname) {
    if (hostname.empty() || hostname.size() > 253) {
        return false;
    }

    std::size_t label_length = 0;
    char previous = '\0';

    for (const char ch : hostname) {
        const bool valid_character =
            (ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') ||
            ch == '-';

        if (ch == '.') {
            if (label_length == 0 || previous == '-') {
                return false;
            }

            label_length = 0;
        } else {
            if (!valid_character) {
                return false;
            }

            if (label_length == 0 && ch == '-') {
                return false;
            }

            if (++label_length > 63) {
                return false;
            }
        }

        previous = ch;
    }

    return label_length > 0 && previous != '-';
}

// ============================================================
// Structured error helper
// ============================================================

[[noreturn]]
void throw_config_error(
    ConfigErrorCode code,
    const std::string& message,
    const std::string& path,
    const YAML::Mark& mark
) {
    throw ConfigError(
        code,
        message,
        path,
        mark.line >= 0 ? mark.line + 1 : -1,
        mark.column >= 0 ? mark.column + 1 : -1
    );
}

// ============================================================
// YAML validation and parsing
// ============================================================

FormaConfig parse_node(
    const YAML::Node& root,
    const std::string& path
) {
    if (!root.IsMap()) {
        throw_config_error(
            ConfigErrorCode::InvalidStructure,
            "configuration must be a YAML map.",
            path,
            root.Mark()
        );
    }

    FormaConfig config;
    std::unordered_set<std::string> seen_sections;

    for (const auto& section : root) {
        const YAML::Node section_key = section.first;
        const YAML::Node values = section.second;

        if (!section_key.IsScalar()) {
            throw_config_error(
                ConfigErrorCode::InvalidStructure,
                "section names must be strings.",
                path,
                section_key.Mark()
            );
        }

        const std::string name =
            section_key.as<std::string>();

        // Detect duplicate sections.
        if (!seen_sections.insert(name).second) {
            throw_config_error(
                ConfigErrorCode::DuplicateSection,
                "duplicate section '" + name + "'.",
                path,
                section_key.Mark()
            );
        }

        // Reject unsupported sections.
        if (name != "system" &&
            name != "desktop" &&
            name != "gaming") {
            throw_config_error(
                ConfigErrorCode::UnknownSection,
                "unknown section '" + name + "'.",
                path,
                section_key.Mark()
            );
        }

        if (!values.IsMap()) {
            throw_config_error(
                ConfigErrorCode::InvalidStructure,
                "section '" + name +
                    "' must contain a map.",
                path,
                values.Mark()
            );
        }

        std::unordered_set<std::string> seen_settings;

        for (const auto& entry : values) {
            const YAML::Node setting_key = entry.first;
            const YAML::Node setting_value = entry.second;

            if (!setting_key.IsScalar() ||
                !setting_value.IsScalar()) {
                throw_config_error(
                    ConfigErrorCode::InvalidStructure,
                    "invalid setting in '" + name + "'.",
                    path,
                    !setting_key.IsScalar()
                        ? setting_key.Mark()
                        : setting_value.Mark()
                );
            }

            const std::string key =
                setting_key.as<std::string>();

            const std::string value =
                setting_value.as<std::string>();

            // Reject duplicate settings.
            if (!seen_settings.insert(key).second) {
                throw_config_error(
                    ConfigErrorCode::DuplicateSetting,
                    "duplicate setting '" +
                        name + "." + key + "'.",
                    path,
                    setting_key.Mark()
                );
            }

            // ------------------------------------------------
            // System settings
            // ------------------------------------------------

            if (name == "system" && key == "hostname") {
                if (!is_valid_hostname(value)) {
                    throw_config_error(
                        ConfigErrorCode::InvalidValue,
                        "invalid hostname '" + value +
                            "'. Use letters, digits, hyphens, "
                            "and optional dots.",
                        path,
                        setting_value.Mark()
                    );
                }

                config.system.hostname = value;

            // ------------------------------------------------
            // Desktop settings
            // ------------------------------------------------

            } else if (
                name == "desktop" && key == "theme"
            ) {
                if (value != "dark" && value != "light") {
                    throw_config_error(
                        ConfigErrorCode::InvalidValue,
                        "theme must be dark or light.",
                        path,
                        setting_value.Mark()
                    );
                }

                config.desktop.theme =
                    value == "dark"
                        ? Theme::Dark
                        : Theme::Light;

            // ------------------------------------------------
            // Gaming settings
            // ------------------------------------------------

            } else if (
                name == "gaming" && key == "enabled"
            ) {
                if (value != "true" && value != "false") {
                    throw_config_error(
                        ConfigErrorCode::InvalidValue,
                        "gaming.enabled must be true or false.",
                        path,
                        setting_value.Mark()
                    );
                }

                config.gaming.enabled = (value == "true");

            // ------------------------------------------------
            // Unknown settings
            // ------------------------------------------------

            } else {
                throw_config_error(
                    ConfigErrorCode::UnknownSetting,
                    "unknown setting '" +
                        name + "." + key + "'.",
                    path,
                    setting_key.Mark()
                );
            }
        }
    }

    return config;
}

} // namespace

// ============================================================
// Public configuration loader
// ============================================================

FormaConfig load_config(const std::string& path) {
    try {
        const YAML::Node root = YAML::LoadFile(path);

        return parse_node(root, path);

    } catch (const YAML::Exception& error) {
        throw_config_error(
            ConfigErrorCode::YamlError,
            std::string("YAML error: ") + error.what(),
            path,
            error.mark
        );
    }
}

} // namespace forma
