# SauronSDKConfig.cmake
# This file is used to find the Sauron SDK when using FetchContent

include(CMakeFindDependencyMacro)

# Try to find system CURL first
find_dependency(CURL QUIET)

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
find_dependency(nlohmann_json QUIET)

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

# Set variables for consumers
set(SAURON_SDK_FOUND TRUE)
set(SAURON_SDK_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include")
set(SAURON_SDK_LIBRARIES sauron_sdk::curl)

# Include the targets file if it exists (for installation)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/sauron-sdk-curl-targets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/sauron-sdk-curl-targets.cmake")
endif() 