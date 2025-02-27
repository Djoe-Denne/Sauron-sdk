# Sauron SDK CURL

A C++ SDK implementation for the Sauron API using libcurl as the HTTP client.

## Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Git (for fetching dependencies)
- libcurl (optional - will be built from source if not found)

## Building the SDK

1. Clone the repository with submodules:
```bash
git clone --recursive https://github.com/yourusername/Sauron-sdk.git
cd Sauron-sdk
```

2. Create and navigate to the build directory:
```bash
mkdir build && cd build
```

3. Configure the project with CMake:
```bash
cmake ..
```

4. Build the project:
```bash
cmake --build .
```

### Build Options

- `BUILD_EXAMPLES` (ON by default): Build example applications
- You can modify these options using `-D` flag with CMake:
```bash
cmake .. -DBUILD_EXAMPLES=OFF
```

## Installation

To install the SDK to your system:

```bash
cmake --install .
```

This will install:
- Header files
- Library files
- CMake configuration files

## Usage

### CMake Integration

To use the SDK in your CMake project:

```cmake
find_package(sauron-sdk-curl REQUIRED)
target_link_libraries(your_target PRIVATE sauron-sdk-curl::sauron-sdk-curl)
```

### Code Example

```cpp
#include <iostream>
#include <sauron/Sauron.hpp>
#include <client/http_client_curl.hpp>

int main() {
    try {
        // Print SDK version
        std::cout << "Sauron SDK Version: " << sauron::Version::toString() << std::endl;

        // Create HTTP client
        auto httpClient = std::make_unique<sauron::client::HttpClientCurl>("localhost:3000");
        
        // Create Sauron client
        sauron::client::SauronClient client(std::move(httpClient));

        client.login(sauron::dto::LoginRequest("sk-proj-123456789123456789123456789", sauron::dto::AIProvider::OPENAI));

        auto response = client.query(sauron::dto::AIQueryRequest("Hello, how are you?", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini"));

        std::cout << "Response: " << response.getResponse() << std::endl;

        // Example: Print client information
        std::cout << "Sauron client initialized successfully!" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 
```

## Dependencies

The SDK automatically manages the following dependencies:
- libcurl (version 8.5.0 if built from source)
- nlohmann_json (for JSON handling)

## Building on Windows

When building on Windows:
- The SDK uses dynamic runtime libraries (MD/MDd)
- DLL files will be placed in the `bin` directory
- All runtime dependencies will be copied to the output directory automatically

## License

[Add your license information here]
