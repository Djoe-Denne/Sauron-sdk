cmake_minimum_required(VERSION 3.15)
project(sauron-sdk-curl VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add parent directory to module path to find sauron-sdk
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/..)

# First try to find system CURL
find_package(CURL QUIET)

if(NOT CURL_FOUND)
    message(STATUS "System CURL not found, building from source...")
    # Find or install CURL
    include(FetchContent)
    
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

# Add library
add_library(${PROJECT_NAME}
    include/sauron/client/http_client_curl.hpp
    include/sauron/client/http_client_curl.cpp
)

# Create an alias target that matches the expected namespace in other projects
add_library(sauron_sdk::curl ALIAS ${PROJECT_NAME})

# Generate export header
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
    BASE_NAME SAURON_SDK_CURL
    EXPORT_MACRO_NAME SAURON_SDK_CURL_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/sauron_sdk_curl_export.h
)


# Get the path where the content was fetched
FetchContent_GetProperties(api_spec SOURCE_DIR SAURON_API_SOURCE_DIR)
# Add the include directory to your project
include_directories(${SAURON_API_SOURCE_DIR}/cpp-sdk/include)
# Link against the sauron-sdk target
# This assumes your target is already defined above
target_link_libraries(${PROJECT_NAME} PRIVATE sauron_sdk::sauron-sdk)

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../api-spec/cpp-sdk/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Make nlohmann_json headers available to consumers
if(TARGET nlohmann_json::nlohmann_json)
    get_target_property(NLOHMANN_JSON_INCLUDE_DIRS nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${NLOHMANN_JSON_INCLUDE_DIRS}>)
endif()

# Make sauron-api headers available to consumers
if(TARGET sauron_sdk::sauron-sdk)
    get_target_property(SAURON_API_INCLUDE_DIRS sauron_sdk::sauron-sdk INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${SAURON_API_INCLUDE_DIRS}>)
    # Also add the include directory to the exported targets
    target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/sauron-api>)
endif()

# Link dependencies
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        CURL::libcurl
        nlohmann_json::nlohmann_json
)

# Set Windows DLL export/import
if(WIN32)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE SAURON_SDK_CURL_EXPORTS
        INTERFACE SAURON_SDK_CURL_IMPORTS
    )
endif()

# Set SAURON_SDK_FOUND variable for consumers
set(SAURON_SDK_FOUND TRUE CACHE INTERNAL "Sauron SDK found")
set(SAURON_SDK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "Sauron SDK include directories")
set(SAURON_SDK_LIBRARIES sauron_sdk::curl CACHE INTERNAL "Sauron SDK libraries")

# Installation
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} nlohmann_json
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/include/sauron_sdk_curl_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE sauron_sdk::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Set variables for config file
set(PACKAGE_CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
set(PACKAGE_CURL_LIBRARIES ${CURL_LIBRARIES})

# Create and install package configuration files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_NAME}-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Install SauronSDKConfig.cmake for FetchContent usage
install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SauronSDKConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SauronSDK
)

# Install FindSauronSDK.cmake for find_package usage
install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindSauronSDK.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/
)

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}") 