
#include "forma/config_diff.hpp"

#include <iostream>

int main() {
    forma::SystemState current{
        .hostname = "current-pc"
    };

    // Case 1: Hostname not specified
    forma::FormaConfig unspecified;
    auto diff = forma::calculate_diff(unspecified, current);

    if (diff.hostname.status != forma::ChangeStatus::Unspecified) {
        std::cerr << "Failed: unspecified hostname\n";
        return 1;
    }

    // Case 2: Hostname already matches
    forma::FormaConfig matching;
    matching.system.hostname = "current-pc";
    diff = forma::calculate_diff(matching, current);

    if (diff.hostname.status != forma::ChangeStatus::Unchanged) {
        std::cerr << "Failed: matching hostname\n";
        return 1;
    }

    // Case 3: Hostname change needed
    forma::FormaConfig modified;
    modified.system.hostname = "forma-pc";
    diff = forma::calculate_diff(modified, current);

    if (diff.hostname.status != forma::ChangeStatus::Modified ||
        diff.hostname.current_value != "current-pc" ||
        diff.hostname.requested_value != "forma-pc") {
        std::cerr << "Failed: modified hostname\n";
        return 1;
        }

    std::cout << "All configuration diff tests passed.\n";
    return 0;
}
