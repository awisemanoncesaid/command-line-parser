#include <catch2/catch_test_macros.hpp>
#include "../include/CommandLineParser.hpp"

TEST_CASE("Basic flag parsing", "[flags]") {
    static const CommandLineParserParams commandLineParserParams = {
        .standaloneFlags = {},
        .flags = {
            {'h', "help", "Print this help and exit"},
        }
    };

    const int argc = 2;
    const char* argv[] = {"./program", "--help"};

    CommandLineParser parser(commandLineParserParams);
    try {
        parser.parse(argc, argv);
    } catch (const FailedParsingException &e) {
        FAIL("Parsing failed with exception: " << e.what());
    }
    REQUIRE(parser.containsFlag('h'));
}
