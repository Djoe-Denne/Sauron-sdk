cmake_minimum_required(VERSION 3.15)
project(sauron-sdk VERSION 1.0.0 LANGUAGES CXX)

# Global settings for runtime library handling
if(WIN32)
    # Set runtime library directory for all configurations
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
    
    # Enable runtime dependency auto-copy for Visual Studio
    set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
    
    # Set proper MSVC runtime library
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Set the module path to find our CMake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Add the base SDK first
add_subdirectory(api-spec/cpp-sdk)

# Add the client SDK
add_subdirectory(sauron-sdk-curl)

# Add the example
add_subdirectory(example)
