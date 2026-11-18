
#pragma once

#include <stdexcept>
#include <string>
#include <utility>

namespace forma {

    enum class ConfigErrorCode {
        YamlError,
        InvalidStructure,
        DuplicateSection,
        DuplicateSetting,
        UnknownSection,
        UnknownSetting,
        InvalidValue
    };


    class ConfigError : public std::runtime_error {
    public:
        ConfigError(
            ConfigErrorCode code,
            std::string message,
            std::string file = {},
            int line = -1,
            int column = -1
        )
            : std::runtime_error(std::move(message)),
              code_(code),
              file_(std::move(file)),
              line_(line),
              column_(column) {}

        [[nodiscard]]
        ConfigErrorCode code() const noexcept {
            return code_;
        }

        [[nodiscard]]
        const std::string& file() const noexcept {
            return file_;
        }

        [[nodiscard]]
        int line() const noexcept {
            return line_;
        }

        [[nodiscard]]
        int column() const noexcept {
            return column_;
        }

    private:
        ConfigErrorCode code_;
        std::string file_;
        int line_;
        int column_;
    };


} // namespace forma
