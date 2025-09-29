
#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <set>

#define IS_ALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

struct StandaloneFlag {
    const std::string name;
    const std::string description;
};

struct Flag {
    char shortName = '\0';
    const std::string longName;
    const std::string description;
    const u_int8_t minArgs = 0;
    const u_int8_t maxArgs = 0;
};

struct CommandLineParserParams {
    const std::vector<StandaloneFlag> standaloneFlags;
    const std::vector<Flag> flags;
};

class FailedParsingException : public std::exception {
public:
    FailedParsingException(const std::string &message) : _message(message) {}
    ~FailedParsingException() = default;
    const char *what() const noexcept override { return _message.c_str(); }
private:
    std::string _message;
};

class CommandLineParser {
public:
    CommandLineParser(const CommandLineParserParams &flags);
    ~CommandLineParser() = default;

    void parse(int argc, const char *argv[]);
    std::ostream &printHelp(std::ostream &os = std::cout) const;
    const std::vector<std::string> &getStandaloneArgs() const;
    bool containsFlag(const std::string &arg) const;
    bool containsFlag(char arg) const;
    const std::vector<std::string> &getFlagArgs(const std::string &flag) const;
    const std::vector<std::string> &getFlagArgs(char flag) const;

    std::ostream &printAllFlags(std::ostream &os = std::cout) const;

private:

    struct FlagCheckResult {
        bool isFlag = false;
        char shortName = '\0';
        std::string longName;
        std::string arg;
    };

    FlagCheckResult isFlag(const std::string &arg);

    std::vector<std::string> _standaloneFlagArgs;
    std::vector<std::vector<std::string>> _flagArgs;
    std::set<size_t> _flagIndices;

    std::string _programName;
    std::vector<StandaloneFlag> _standaloneFlags;
    std::vector<Flag> _flags;
};

class EnvironmentParser {
public:
    EnvironmentParser(const char *envp[]);
    ~EnvironmentParser() = default;

    const std::string &get(const std::string &key) const;
    std::ostream &printAll(std::ostream &os = std::cout) const;

private:
    std::map<std::string, std::string> _env;
};
