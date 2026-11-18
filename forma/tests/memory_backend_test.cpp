
#include "forma/memory_backend.hpp"

#include <iostream>
#include <stdexcept>

int main() {
    using namespace forma;

    const SystemState initial{
        .hostname = "pop-os"
    };

    MemoryBackend backend(initial);

    // Test 1: Read the original state.
    if (backend.read_state().hostname != "pop-os") {
        std::cerr << "Initial state mismatch.\n";
        return 1;
    }

    // Test 2: Update the in-memory hostname.
    backend.set_hostname("forma-desktop");

    if (backend.read_state().hostname != "forma-desktop") {
        std::cerr << "Memory backend failed to update.\n";
        return 1;
    }

    // Test 3: The original state remains unchanged.
    if (initial.hostname != "pop-os") {
        std::cerr << "Original state was modified.\n";
        return 1;
    }

    // Test 4: Reject empty hostnames.
    bool rejected_empty = false;

    try {
        backend.set_hostname("");
    } catch (const std::invalid_argument&) {
        rejected_empty = true;
    }

    if (!rejected_empty ||
        backend.read_state().hostname != "forma-desktop") {
        std::cerr << "Invalid hostname was accepted.\n";
        return 1;
        }

    std::cout << "All memory backend tests passed.\n";
    return 0;
}
