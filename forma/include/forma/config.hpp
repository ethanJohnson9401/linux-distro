
#pragma once

#include <optional>
#include <string>

namespace forma {

    enum class Theme {
        Light,
        Dark
    };

    struct SystemConfig {
        std::optional<std::string> hostname;
    };

    struct DesktopConfig {
        std::optional<Theme> theme;
    };

    struct GamingConfig {
        std::optional<bool> enabled;
    };

    struct FormaConfig {
        SystemConfig system;
        DesktopConfig desktop;
        GamingConfig gaming;
    };

} // namespace forma
