cmake_minimum_required(VERSION 3.15)
project(Sauron-SDK VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_EXAMPLES "Build examples" OFF)

# Try to find system CURL first
find_package(CURL QUIET)

if(NOT CURL_FOUND)
    message(STATUS "System CURL not found, building from source...")
    # Find or install CURL
    include(FetchContent)
    FetchContent_Declare(
        CURL
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-8_5_0
    )
    FetchContent_MakeAvailable(CURL)
    
    # Set CURL variables for config file
    set(CURL_INCLUDE_DIRS ${curl_SOURCE_DIR}/include)
    set(CURL_LIBRARIES libcurl)
else()
    message(STATUS "Found system CURL: ${CURL_LIBRARIES}")
endif()

# Try to find nlohmann_json
find_package(nlohmann_json QUIET)

if(NOT nlohmann_json_FOUND)
    message(STATUS "System nlohmann_json not found, building from source...")
    # Find or install nlohmann_json
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
else()
    message(STATUS "Found system nlohmann_json")
endif()

# Add subdirectories for each component
add_subdirectory(api-spec/cpp-sdk)
add_subdirectory(sauron-sdk-curl)
if(BUILD_EXAMPLES)  
    add_subdirectory(example)
endif()


# Set variables for consumers
set(SAURON_SDK_FOUND TRUE CACHE INTERNAL "Sauron SDK found")
set(SAURON_SDK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/sauron-sdk-curl/include CACHE INTERNAL "Sauron SDK include directories")
set(SAURON_SDK_LIBRARIES sauron_sdk::curl CACHE INTERNAL "Sauron SDK libraries")

# Install SauronSDKConfig.cmake for FetchContent usage
install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/sauron-sdk-curl/cmake/SauronSDKConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SauronSDK
) 

