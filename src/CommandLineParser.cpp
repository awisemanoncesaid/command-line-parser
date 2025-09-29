#include "CommandLineParser.hpp"

CommandLineParser::FlagCheckResult CommandLineParser::isFlag(const std::string &arg)
{
    if (arg.size() < 2 || arg[0] != '-') // not a flag
        return {false};
    if (arg.size() == 2 && IS_ALPHA(arg[1])) // short flag
        return {true, arg[1]};
    if (arg[1] == '-') {
        if (arg[2] == '-' || !IS_ALPHA(arg[2])) // invalid long flag
            return {false};
        size_t eqPos = arg.find('=');
        if (eqPos != std::string::npos) // long flag with arg
            return {true, '\0', arg.substr(2, eqPos - 2), arg.substr(eqPos + 1)};
        return {true, '\0', arg.substr(2)}; // long flag
    } else if (IS_ALPHA(arg[1])) // short flag with arg
        return {true, arg[1], "", arg.substr(2)};
    return {false};
}

CommandLineParser::CommandLineParser(const CommandLineParserParams &flags)
    : _standaloneFlags(flags.standaloneFlags), _flags(flags.flags)
{
    _flagArgs.resize(_flags.size());
}

void CommandLineParser::parse(int argc, const char *argv[])
{
    _programName = argv[0];
    for (size_t i = 1; i < argc; i++) {
        const std::string &arg = argv[i];

        FlagCheckResult flagCheckResult = isFlag(arg);
        if (flagCheckResult.isFlag) {
            bool found = false;
            size_t flagIdx = -1;
            for (const auto &flag : _flags) {
                if (flagCheckResult.shortName == flag.shortName || flagCheckResult.longName == flag.longName) {
                    found = true;
                    flagIdx = &flag - &_flags[0];
                    if (_flagIndices.find(flagIdx) != _flagIndices.end()) {
                        throw FailedParsingException("Flag set twice: " + arg);
                    }
                    _flagIndices.insert(flagIdx);
                    if (flag.maxArgs == 0) {
                        _flagArgs[flagIdx] = {};
                    } else {
                        std::vector<std::string> flagArgs;
                        if (!flagCheckResult.arg.empty()) {
                            flagArgs.push_back(flagCheckResult.arg);
                        }
                        for (size_t j = i + 1; j < argc; j++) {
                            if (isFlag(argv[j]).isFlag || flagArgs.size() == flag.maxArgs) {
                                break;
                            }
                            flagArgs.push_back(argv[j]);
                        }
                        if (flagArgs.size() < flag.minArgs) {
                            throw FailedParsingException("Missing arguments for flag: " + flag.longName);
                        }
                        _flagArgs[flagIdx] = flagArgs;
                        i += flagArgs.size() - 1;
                    }
                    break;
                }
            }
            if (!found) {
                throw FailedParsingException("Unknown flag: " + arg);
            }
        } else {
            _standaloneFlagArgs.push_back(arg);
        }
    }
}

std::ostream &CommandLineParser::printHelp(std::ostream &os) const
{
    os << "Usage:\n";
    os << "\t" << _programName;
    if (!_flags.empty()) {
        os << " [options]";
    }
    for (const auto &flag : _standaloneFlags) {
        os << " [" << flag.name << "]";
    }
    os << "\nArgs:\n";
    for (const auto &flag : _standaloneFlags) {
        os << "\t" << flag.name << ":\t" << flag.description << "\n";
    }
    os << "Options:\n";
    for (const auto &flag : _flags) {
        os << "\t";
        if (flag.shortName != '\0' && !flag.longName.empty()) {
            os << "-" << flag.shortName << ", " << "--" << flag.longName;
        } else if (flag.shortName != '\0') {
            os << "-" << flag.shortName;
        } else if (!flag.longName.empty()) {
            os << "--" << flag.longName;
        } else {
            continue; // invalid flag - should not happen
        }
        os << ":\t" << flag.description << "\n";
    }
    return os;
}

std::ostream &CommandLineParser::printAllFlags(std::ostream &os) const
{
    if (!_standaloneFlagArgs.empty()) {
        os << "Standalone flags:\n";
        for (const auto &flag : _standaloneFlagArgs) {
            os << flag << " ";
        }
        os << "\n";
    }
    if (!_flagIndices.empty()) {
        os << "Flags:\n";
        for (const auto &flag : _flagIndices) {
            if (_flags[flag].shortName != '\0' && !_flags[flag].longName.empty()) {
                os << "-" << _flags[flag].shortName << ", " << "--" << _flags[flag].longName;
            } else if (_flags[flag].shortName != '\0') {
                os << "-" << _flags[flag].shortName;
            } else if (!_flags[flag].longName.empty()) {
                os << "--" << _flags[flag].longName;
            }
            os << ": ";
            for (const auto &arg : _flagArgs[flag]) {
                os << arg << " ";
            }
            os << "\n";
        }
    }
    return os;
}

const std::vector<std::string> &CommandLineParser::getStandaloneArgs() const
{
    return _standaloneFlagArgs;
}

bool CommandLineParser::containsFlag(const std::string &arg) const
{
    size_t flagIdx = -1;
    for (const auto &flag : _flags) {
        if (arg[1] == flag.shortName || arg.substr(2) == flag.longName) {
            flagIdx = &flag - &_flags[0];
            break;
        }
    }
    if (flagIdx == -1) {
        return false;
    }
    return _flagIndices.find(flagIdx) != _flagIndices.end();
}

bool CommandLineParser::containsFlag(char arg) const
{
    size_t flagIdx = -1;
    for (const auto &flag : _flags) {
        if (arg == flag.shortName) {
            flagIdx = &flag - &_flags[0];
            break;
        }
    }
    if (flagIdx == -1) {
        return false;
    }
    return _flagIndices.find(flagIdx) != _flagIndices.end();
}

const std::vector<std::string> &CommandLineParser::getFlagArgs(const std::string &flag) const
{
    for (const auto &f : _flags) {
        if (f.longName == flag) {
            return _flagArgs[&f - &_flags[0]];
        }
    }
    throw std::invalid_argument("Unknown flag: " + flag);
}

const std::vector<std::string> &CommandLineParser::getFlagArgs(char flag) const
{
    for (const auto &f : _flags) {
        if (f.shortName == flag) {
            return _flagArgs[&f - &_flags[0]];
        }
    }
    throw std::invalid_argument("Unknown flag: " + flag);
}

EnvironmentParser::EnvironmentParser(const char *envp[])
{
    for (size_t i = 0; envp[i] != nullptr; i++) {
        std::string env(envp[i]);
        size_t eqPos = env.find('=');
        if (eqPos == std::string::npos) {
            continue;
        }
        _env[env.substr(0, eqPos)] = env.substr(eqPos + 1);
    }
}

const std::string &EnvironmentParser::get(const std::string &key) const
{
    auto it = _env.find(key);
    if (it == _env.end()) {
        throw std::invalid_argument("Unknown environment variable: " + key);
    }
    return it->second;
}

std::ostream &EnvironmentParser::printAll(std::ostream &os) const
{
    for (const auto &env : _env) {
        os << env.first << ": " << env.second << "\n";
    }
    return os;
}
