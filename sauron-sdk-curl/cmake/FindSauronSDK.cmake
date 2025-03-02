# FindSauronSDK.cmake
# This module finds the Sauron SDK and sets the following variables:
#  SAURON_SDK_FOUND        - True if Sauron SDK was found
#  SAURON_SDK_INCLUDE_DIRS - Include directories for Sauron SDK
#  SAURON_SDK_LIBRARIES    - Libraries to link against

include(CMakeFindDependencyMacro)

# Try to find the config file
find_package(SauronSDK CONFIG QUIET)

if(SauronSDK_FOUND)
    # Config file was found, variables should be set
    return()
endif()

# Try to find the sauron-sdk-curl package
find_package(sauron-sdk-curl CONFIG QUIET)

if(sauron-sdk-curl_FOUND)
    # Set variables for consumers
    set(SAURON_SDK_FOUND TRUE)
    set(SAURON_SDK_INCLUDE_DIRS ${sauron-sdk-curl_INCLUDE_DIRS})
    set(SAURON_SDK_LIBRARIES sauron_sdk::curl)
    return()
endif()

# If we get here, we need to handle dependencies ourselves

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
    
    # Make nlohmann_json available to consumers
    if(TARGET nlohmann_json::nlohmann_json)
        get_target_property(NLOHMANN_JSON_INCLUDE_DIRS nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
    endif()
else()
    message(STATUS "Found system nlohmann_json")
    if(TARGET nlohmann_json::nlohmann_json)
        get_target_property(NLOHMANN_JSON_INCLUDE_DIRS nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
    endif()
endif()

# Not found
set(SAURON_SDK_FOUND FALSE) 