
#include "forma/system_inspector_formatter.hpp"

#include <iomanip>
#include <ostream>
#include <string>

namespace forma {

    namespace {

        void print_field(
            std::ostream& output,
            const std::string& label,
            const std::string& value
        ) {
            output << "  "
                   << std::left
                   << std::setw(20)
                   << label
                   << value
                   << '\n';
        }

    } // namespace

    void print_system_info(
        const SystemInfo& info,
        std::ostream& output
    ) {
        output << "Forma Linux - System Inspection\n\n";

        output << "System\n";

        print_field(
            output,
            "Hostname:",
            info.hostname
        );

        print_field(
            output,
            "Operating system:",
            info.operating_system
        );

        print_field(
            output,
            "Architecture:",
            info.architecture
        );

        print_field(
            output,
            "Kernel:",
            info.kernel
        );

        output << "\nForma Core\n";

        print_field(
            output,
            "Version:",
            "0.1.0"
        );

        print_field(
            output,
            "Mode:",
            "Read-only inspection"
        );
    }

} // namespace forma
