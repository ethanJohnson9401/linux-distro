
#include "forma/config_validator.hpp"

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <cctype>
#include <string_view>
#include <unordered_set>

namespace forma {

    namespace {

        bool is_valid_hostname(std::string_view hostname) {
            if (hostname.empty() || hostname.size() > 253) {
                return false;
            }

            std::size_t label_length = 0;
            char previous = '\0';

            for (const char ch : hostname) {
                const unsigned char character =
                    static_cast<unsigned char>(ch);

                if (ch == '.') {
                    if (label_length == 0 || previous == '-') {
                        return false;
                    }

                    label_length = 0;
                } else {
                    if (!std::isalnum(character) && ch != '-') {
                        return false;
                    }

                    if (label_length == 0 && ch == '-') {
                        return false;
                    }

                    ++label_length;

                    if (label_length > 63) {
                        return false;
                    }
                }

                previous = ch;
            }

            return label_length > 0 && previous != '-';
        }

    } // namespace

    bool validate_config(const std::string& path, bool verbose) {
        try {
            const YAML::Node config = YAML::LoadFile(path);

            if (!config.IsMap()) {
                std::cerr << "Error: configuration must be a YAML map.\n";
                return false;
            }

            std::unordered_set<std::string> seen_sections;

            for (const auto& section : config) {
                if (!section.first.IsScalar()) {
                    std::cerr << "Error: section names must be strings.\n";
                    return false;
                }

                const std::string name = section.first.as<std::string>();
                const YAML::Node values = section.second;

                if (!seen_sections.insert(name).second) {
                    std::cerr << "Error: duplicate section '" << name << "'.\n";
                    return false;
                }

                std::unordered_set<std::string> seen_settings;

                if (name != "system" && name != "desktop" &&
                    name != "gaming") {
                    std::cerr << "Error: unknown section '" << name << "'.\n";
                    return false;
                    }

                if (!values.IsMap()) {
                    std::cerr << "Error: section '" << name
                              << "' must contain a map.\n";
                    return false;
                }

                for (const auto& entry : values) {
                    if (!entry.first.IsScalar() || !entry.second.IsScalar()) {
                        std::cerr << "Error: invalid setting in '"
                                  << name << "'.\n";
                        return false;
                    }

                    const std::string key = entry.first.as<std::string>();
                    const std::string value = entry.second.as<std::string>();

                    if (!seen_settings.insert(key).second) {
                        std::cerr
                            << "Error: duplicate setting '"
                            << name << "." << key << "'.\n";
                        return false;
                    }

                    if (name == "system" && key == "hostname") {
                        if (!is_valid_hostname(value)) {
                            std::cerr
                                << "Error: invalid hostname '" << value << "'.\n"
                                << "Use letters, digits, hyphens, and optional dots.\n";
                            return false;
                        }
                    } else if (name == "desktop" && key == "theme") {
                        if (value != "dark" && value != "light") {
                            std::cerr << "Error: theme must be dark or light.\n";
                            return false;
                        }
                    } else if (name == "gaming" && key == "enabled") {
                        if (value != "true" && value != "false") {
                            std::cerr << "Error: gaming.enabled must be true or false.\n";
                            return false;
                        }
                    } else {
                        std::cerr << "Error: unknown setting '"
                                  << name << "." << key << "'.\n";
                        return false;
                    }
                }
            }

            if (verbose) {
                std::cout << "Configuration valid: " << path << '\n';
            }

            return true;

        } catch (const YAML::Exception& error) {
            std::cerr << "YAML error: " << error.what() << '\n';
            return false;
        }
    }
}// namespace forma
