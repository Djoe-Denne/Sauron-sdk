# Sauron SDK

A comprehensive C++ SDK for building applications with HTTP communication capabilities.

## Components

- **sauron-sdk-curl**: HTTP client implementation using libcurl

## Quick Start

The easiest way to use Sauron SDK in your project is via CMake's FetchContent:

```cmake
if(NOT SAURON_SDK_FOUND)
    message(STATUS "System Sauron SDK not found, building from source...")
    # Find or install Sauron SDK
    include(FetchContent)
    FetchContent_Declare(
        SAURON_SDK
        GIT_REPOSITORY https://github.com/Djoe-Denne/Sauron-sdk.git
        GIT_BRANCH master
    )
    FetchContent_MakeAvailable(SAURON_SDK)
    
    # Variables are automatically set by the SDK
    # SAURON_SDK_INCLUDE_DIRS - Include directories
    # SAURON_SDK_LIBRARIES - Libraries to link against (sauron_sdk::curl)
else()
    message(STATUS "Found system Sauron SDK: ${SAURON_SDK_LIBRARIES}")
endif()

# Then link your target
target_link_libraries(your_target PRIVATE ${SAURON_SDK_LIBRARIES})
target_include_directories(your_target PRIVATE ${SAURON_SDK_INCLUDE_DIRS})
```

## Features

- **Automatic Dependency Management**: The SDK handles all dependencies (curl, nlohmann_json) automatically
- **Easy Integration**: Simple CMake integration with FetchContent support
- **Cross-Platform**: Works on Windows, macOS, and Linux

## Requirements

- C++17 compatible compiler
- CMake 3.15 or higher

## Documentation

For detailed documentation on each component, please refer to:

- [sauron-sdk-curl Documentation](sauron-sdk-curl/README.md)

## License

[MIT License](LICENSE)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. 