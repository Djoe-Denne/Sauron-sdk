# Sauron SDK - CURL Implementation

A C++ SDK for HTTP communication using libcurl.

## Features

- HTTP client implementation using libcurl
- Automatic dependency management (curl, nlohmann_json)
- CMake integration with FetchContent support
- Cross-platform compatibility

## Requirements

- C++17 compatible compiler
- CMake 3.15 or higher

## Integration

### Using FetchContent (Recommended)

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
```

Then, in your target configuration:

```cmake
target_link_libraries(your_target PRIVATE ${SAURON_SDK_LIBRARIES})
target_include_directories(your_target PRIVATE ${SAURON_SDK_INCLUDE_DIRS})
```

### Using find_package

If you have installed the SDK on your system, you can use `find_package`:

```cmake
find_package(SauronSDK REQUIRED)
target_link_libraries(your_target PRIVATE ${SAURON_SDK_LIBRARIES})
target_include_directories(your_target PRIVATE ${SAURON_SDK_INCLUDE_DIRS})
```

## Dependencies

The SDK automatically handles the following dependencies:

- **libcurl**: Used for HTTP communication
- **nlohmann_json**: Used for JSON parsing

You don't need to manually install or configure these dependencies. The SDK will:

1. First try to find them on your system
2. If not found, automatically download and build them from source

**Note**: The nlohmann_json headers are automatically made available to your project when you use Sauron SDK. You can directly include and use them in your code without any additional configuration:

```cpp
#include <nlohmann/json.hpp>

// Use nlohmann::json in your code
nlohmann::json j = {{"name", "value"}};
```

## Building from Source

If you want to build and install the SDK manually:

```bash
mkdir build && cd build
cmake ..
cmake --build .
cmake --install .
```

## Usage Example
```cpp

int main() {
    try {
        // Print SDK version
        std::cout << "Sauron SDK Version: " << sauron::Version::toString() << std::endl;

        // Create HTTP client
        auto httpClient = std::make_unique<sauron::client::HttpClientCurl>("localhost:3000");
        
        // Create Sauron client
        sauron::client::SauronClient client(std::move(httpClient));

        client.login(sauron::dto::LoginRequest("sk-proj-******", sauron::dto::AIProvider::OPENAI));

       auto response = client.query(sauron::dto::AIQueryRequest("Hello, how are you?", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini"));

        std::cout << "Response: " << response.getResponse() << std::endl;

        std::atomic_bool is_done = false;
        // use stream to get the response
        if(!client.queryStream(sauron::dto::AIQueryRequest("I'm testing your stream api, could you provide me a long enough anwser to test the stream?", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini"), [&is_done](const std::string& chunk, bool is_final) {
            std::cout << "Chunk: " << chunk << std::endl;
            if(is_final) {
                is_done = true;
            }
            return true;
        })) {
            std::cerr << "Stream failed" << std::endl;
        }

        while(!is_done) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        auto responseAlgorithm = client.queryAlgorithm(sauron::dto::AIQueryRequest("solve the k-neirest element problem in java", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini"));

        std::cout << "Response: " << responseAlgorithm.toJson().dump(4) << std::endl;

        // load image in base64 format
        std::ifstream file("../../data-tests/image.png", std::ios::binary);
        std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
        std::string image_base64 = "data:image/png;base64," + base64_encode(buffer);
        
        auto aiAlgorithmWithImageQuery = sauron::dto::AIQueryRequest("solve it in java", sauron::dto::AIProvider::OPENAI, "gpt-4o-mini");
        aiAlgorithmWithImageQuery.addImage(image_base64);

        auto responseAlgorithmWithImage = client.queryAlgorithm(aiAlgorithmWithImageQuery);

        std::cout << "Response: " << responseAlgorithmWithImage.toJson().dump(4) << std::endl;

        // Example: Print client information
        std::cout << "Sauron client initialized successfully!" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}  
```

## Advanced Configuration

### Using with an Existing Curl Installation

If you have a specific curl installation you want to use:

```cmake
# Set curl variables before including Sauron SDK
set(CURL_ROOT "/path/to/curl")
find_package(CURL REQUIRED)

# Then include Sauron SDK
include(FetchContent)
FetchContent_Declare(
    SAURON_SDK
    GIT_REPOSITORY https://github.com/Djoe-Denne/Sauron-sdk.git
    GIT_BRANCH master
)
FetchContent_MakeAvailable(SAURON_SDK)
```

## License

[MIT License](LICENSE)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. 