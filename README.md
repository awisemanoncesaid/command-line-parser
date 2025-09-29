# Command Line Parser

This is a simple command line parser library implemented in C++. It allows you to define and parse command line options and arguments easily.

## Features supported

- Short options (e.g., `-h`) with optional arguments (e.g., `-farg`)
- Long options (e.g., `--help`) with optional arguments (e.g., `--file=arg`)
- Positional arguments
- Env variables retrieval (EnvironmentParser)
- Automatic help message generation

Combined short options (e.g., `-abc` equivalent to `-a -b -c`) which involves different parser modes are not supported at this time.

## Usage

1. Include the header file:
   ```cpp
   #include "CommandLineParser.hpp"
   ```
2. Define command line parser params:
    ```cpp
    static const CommandLineParserParams commandLineParserParams = {
        .standaloneFlags = {},
        .flags = {
            {'h', "help", "Print this help and exit"},
        }
    };
    ```
3. Create a `CommandLineParser` instance and parse the command line arguments:
    ```cpp
    CommandLineParser parser(commandLineParserParams);
    try {
        parser.parse(argc, argv);
    } catch (const FailedParsingException &e) {
        parser.printHelp();
    }
4. Semantic checks on parsed arguments:
    ```cpp
    if (parser.containsFlag('h')) {
        parser.printHelp();
    }
    ```
5. Environment variables parsing:
    ```cpp
    EnvironmentParser envParser(env);
    envParser.printAll();
    ```

## Building

This project uses CMake for building. To build the library, follow these steps:
```bash
mkdir build
cd build
cmake ..
make
```
The compiled library will be located in the `build` directory.

## License

This project is licensed under the MIT License.
