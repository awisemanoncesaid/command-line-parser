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
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
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
