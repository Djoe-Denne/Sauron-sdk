cmake_minimum_required(VERSION 3.15)
project(Sauron-SDK VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_EXAMPLES "Build examples" ON)

# Include FetchContent module
include(FetchContent)

# Try to find system CURL first
find_package(CURL QUIET)

if(NOT CURL_FOUND)
    message(STATUS "System CURL not found, building from source...")
    # Find or install CURL
    
    # Set CURL build options to minimize what gets built
    set(BUILD_CURL_EXE OFF CACHE BOOL "Don't build curl executable")
    set(BUILD_TESTING OFF CACHE BOOL "Don't build tests")
    set(CURL_DISABLE_DICT ON CACHE BOOL "Disable DICT protocol")
    set(CURL_DISABLE_FILE ON CACHE BOOL "Disable FILE protocol")
    set(CURL_DISABLE_FTP ON CACHE BOOL "Disable FTP protocol")
    set(CURL_DISABLE_GOPHER ON CACHE BOOL "Disable Gopher protocol")
    set(CURL_DISABLE_IMAP ON CACHE BOOL "Disable IMAP protocol")
    set(CURL_DISABLE_LDAP ON CACHE BOOL "Disable LDAP protocol")
    set(CURL_DISABLE_LDAPS ON CACHE BOOL "Disable LDAPS protocol")
    set(CURL_DISABLE_POP3 ON CACHE BOOL "Disable POP3 protocol")
    set(CURL_DISABLE_RTSP ON CACHE BOOL "Disable RTSP protocol")
    set(CURL_DISABLE_SMB ON CACHE BOOL "Disable SMB protocol")
    set(CURL_DISABLE_SMTP ON CACHE BOOL "Disable SMTP protocol")
    set(CURL_DISABLE_TELNET ON CACHE BOOL "Disable TELNET protocol")
    set(CURL_DISABLE_TFTP ON CACHE BOOL "Disable TFTP protocol")
    
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
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)

else()
    message(STATUS "Found system nlohmann_json")
endif()
# Make nlohmann_json available to consumers
if(TARGET nlohmann_json::nlohmann_json)
    get_target_property(NLOHMANN_JSON_INCLUDE_DIRS nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
    # Store for later use
    set(NLOHMANN_JSON_INCLUDE_DIRS ${NLOHMANN_JSON_INCLUDE_DIRS} CACHE INTERNAL "nlohmann_json include directories")
endif()


if(GITHUB_TOKEN)
    set(GIT_REPOSITORY https://${GITHUB_TOKEN}@github.com/Djoe-Denne/Sauron-api.git)
else()
    set(GIT_REPOSITORY https://github.com/Djoe-Denne/Sauron-api.git)
endif()
# Fetch API spec from Git repository instead of using submodule
message(STATUS "Fetching Sauron API specification...")
# Set a variable to indicate that we're building as part of the main project
set(SAURON_SDK_MAIN_PROJECT ON CACHE INTERNAL "Building as part of the main Sauron SDK project")
FetchContent_Declare(
    api_spec
    GIT_REPOSITORY ${GIT_REPOSITORY}
    GIT_TAG main
)
FetchContent_MakeAvailable(api_spec)

add_subdirectory(sauron-sdk-curl)
if(BUILD_EXAMPLES)  
    add_subdirectory(example)
endif()


# Set variables for consumers
set(SAURON_SDK_FOUND TRUE CACHE INTERNAL "Sauron SDK found")
set(SAURON_SDK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/sauron-sdk-curl/include CACHE INTERNAL "Sauron SDK include directories")
if(DEFINED NLOHMANN_JSON_INCLUDE_DIRS)
    set(SAURON_SDK_INCLUDE_DIRS ${SAURON_SDK_INCLUDE_DIRS} ${NLOHMANN_JSON_INCLUDE_DIRS} CACHE INTERNAL "Sauron SDK include directories")
endif()

# Add sauron-api include directories
FetchContent_GetProperties(api_spec SOURCE_DIR SAURON_API_SOURCE_DIR)
set(SAURON_SDK_INCLUDE_DIRS ${SAURON_SDK_INCLUDE_DIRS} ${SAURON_API_SOURCE_DIR}/cpp-sdk/include CACHE INTERNAL "Sauron SDK include directories")

set(SAURON_SDK_LIBRARIES sauron_sdk::curl CACHE INTERNAL "Sauron SDK libraries")

# Install SauronSDKConfig.cmake for FetchContent usage
install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/sauron-sdk-curl/cmake/SauronSDKConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SauronSDK
) 

