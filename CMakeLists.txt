cmake_minimum_required(VERSION 3.10)
project(CommandLineParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Library ---

set(LIBRARY_SOURCES
    src/CommandLineParser.cpp
)

add_library(CommandLineParser STATIC ${LIBRARY_SOURCES})

target_include_directories(CommandLineParser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# --- Installation ---

include(GNUInstallDirs)

install(TARGETS CommandLineParser
    EXPORT CommandLineParserTargets
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT CommandLineParserTargets
    FILE CommandLineParserTargets.cmake
    NAMESPACE CommandLineParser::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CommandLineParser
)

# Create a ConfigVersion.cmake file

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CommandLineParserConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CommandLineParserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CommandLineParserConfig.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CommandLineParserConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CommandLineParserConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CommandLineParser
)

# --- Testing ---

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
)

FetchContent_MakeAvailable(Catch2)

enable_testing()

add_executable(test_cmdline_parser
    tests/test_parser.cpp
)

target_link_libraries(test_cmdline_parser
    PRIVATE CommandLineParser Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(test_cmdline_parser)
