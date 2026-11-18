
cmake_minimum_required(VERSION 3.20)

project(FormaCore
        VERSION 0.1.0
        DESCRIPTION "Declarative configuration manager for Forma Linux"
        LANGUAGES CXX
)

# ============================================================
# C++ configuration
# ============================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Dependencies
# ============================================================

find_package(yaml-cpp CONFIG REQUIRED)

# ============================================================
# Forma Core library
# ============================================================

add_library(forma_core STATIC
        src/config_loader.cpp
        src/config_validator.cpp
        src/config_parser.cpp
        src/config_planner.cpp
        src/system_state.cpp
        src/config_diff.cpp
)

target_include_directories(forma_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(forma_core PUBLIC
        yaml-cpp::yaml-cpp
)

target_compile_options(forma_core PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# ============================================================
# Forma CLI
# ============================================================

add_executable(forma
        src/main.cpp
)

target_link_libraries(forma PRIVATE
        forma_core
)

target_compile_options(forma PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

# ============================================================
# Automated testing
# ============================================================

enable_testing()

# ------------------------------------------------------------
# CLI tests
# ------------------------------------------------------------

add_test(
        NAME forma_help
        COMMAND forma --help
)

add_test(
        NAME forma_version
        COMMAND forma --version
)

add_test(
        NAME forma_invalid_argument
        COMMAND forma --unknown
)

set_tests_properties(forma_invalid_argument PROPERTIES
        WILL_FAIL TRUE
)

# ------------------------------------------------------------
# Configuration validation tests
# ------------------------------------------------------------

add_test(
        NAME forma_valid_config
        COMMAND forma validate
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/desktop.yaml
)

add_test(
        NAME forma_invalid_theme
        COMMAND forma validate
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-theme.yaml
)

set_tests_properties(forma_invalid_theme PROPERTIES
        WILL_FAIL TRUE
)

add_test(
        NAME forma_invalid_hostname
        COMMAND forma validate
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-hostname.yaml
)

set_tests_properties(forma_invalid_hostname PROPERTIES
        WILL_FAIL TRUE
)

add_test(
        NAME forma_duplicate_keys
        COMMAND forma validate
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-duplicate.yaml
)

set_tests_properties(forma_duplicate_keys PROPERTIES
        WILL_FAIL TRUE
)

# ------------------------------------------------------------
# Typed configuration parser tests
# ------------------------------------------------------------

add_executable(forma_config_parser_test
        tests/config_parser_test.cpp
)

target_link_libraries(forma_config_parser_test PRIVATE
        forma_core
)

add_test(
        NAME forma_config_parser
        COMMAND forma_config_parser_test
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/desktop.yaml
)

# ------------------------------------------------------------
# Configuration planning tests
# ------------------------------------------------------------

add_test(
        NAME forma_plan
        COMMAND forma plan
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/desktop.yaml
)

set_tests_properties(forma_plan PROPERTIES
        PASS_REGULAR_EXPRESSION "Preview only"
)

add_test(
        NAME forma_partial_config
        COMMAND forma plan
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/partial.yaml
)

set_tests_properties(forma_partial_config PROPERTIES
        PASS_REGULAR_EXPRESSION "Theme: \\(unchanged\\)"
)

# ------------------------------------------------------------
# Configuration difference engine tests
# ------------------------------------------------------------

add_executable(forma_config_diff_test
        tests/config_diff_test.cpp
)

target_link_libraries(forma_config_diff_test PRIVATE
        forma_core
)

add_test(
        NAME forma_config_diff
        COMMAND forma_config_diff_test
)

# ------------------------------------------------------------
# Unified configuration loader tests
# ------------------------------------------------------------

add_executable(forma_config_loader_test
        tests/config_loader_test.cpp
)

target_link_libraries(forma_config_loader_test PRIVATE
        forma_core
)

add_test(
        NAME forma_config_loader
        COMMAND forma_config_loader_test
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/desktop.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-duplicate.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-hostname.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-theme.yaml
)

# ------------------------------------------------------------
# Typed configuration error tests
# ------------------------------------------------------------

add_executable(forma_config_error_test
        tests/config_error_test.cpp
)

target_link_libraries(forma_config_error_test PRIVATE
        forma_core
)

add_test(
        NAME forma_config_error
        COMMAND forma_config_error_test
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-duplicate.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-hostname.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-theme.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-syntax.yaml
)

# Diagnostic formatting and source-location tests
add_executable(forma_config_diagnostics_test
        tests/config_diagnostics_test.cpp
)

target_link_libraries(forma_config_diagnostics_test PRIVATE
        forma_core
)

add_test(
        NAME forma_config_diagnostics
        COMMAND forma_config_diagnostics_test
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-theme.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-hostname.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/invalid-duplicate.yaml
)