
#include "forma/config_planner.hpp"

#include <ostream>

namespace forma {

       void print_plan(
           const FormaConfig& config,
           const ConfigDiff& diff,
           std::ostream& output
       ) {
              output << "Forma Configuration Plan\n"
                     << "========================\n\n";

              output << "System\n";

              switch (diff.hostname.status) {
                     case ChangeStatus::Unspecified:
                            output << "  Hostname: (unchanged)\n";
                            break;

                     case ChangeStatus::Unchanged:
                            output << "  Hostname: "
                                   << diff.hostname.current_value << '\n'
                                   << "  Status: Already matching\n";
                            break;

                     case ChangeStatus::Modified:
                            output << "  Hostname: "
                                   << diff.hostname.current_value
                                   << " -> "
                                   << *diff.hostname.requested_value << '\n'
                                   << "  Status: Change required\n";
                            break;
              }

              output << "\nDesktop\n";

              if (config.desktop.theme.has_value()) {
                     output << "  Theme requested: "
                            << (*config.desktop.theme == Theme::Dark
                                    ? "dark" : "light")
                            << " (current state not checked)\n";
              } else {
                     output << "  Theme: (unchanged)\n";
              }

              output << "\nGaming\n";

              if (config.gaming.enabled.has_value()) {
                     output << "  Enabled requested: "
                            << (*config.gaming.enabled ? "yes" : "no")
                            << " (current state not checked)\n";
              } else {
                     output << "  Enabled: (unchanged)\n";
              }

              output << "\nPreview only. No system changes were made.\n";
       }

} // namespace forma
