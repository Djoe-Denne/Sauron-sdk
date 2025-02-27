cmake_minimum_required(VERSION 3.20)
project(sauron-sdk-root VERSION 1.0.0 LANGUAGES CXX)

# Set the module path to find our CMake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Add the base SDK first
add_subdirectory(api-spec/cpp-sdk)

# Add the client SDK
add_subdirectory(sauron-sdk-curl)

# Add the example
add_subdirectory(example)
