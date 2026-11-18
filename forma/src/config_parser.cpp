
#include "forma/config_parser.hpp"
#include "forma/config_loader.hpp"

namespace forma {

    FormaConfig parse_config(const std::string& path) {
        return load_config(path);
    }

} // namespace forma
