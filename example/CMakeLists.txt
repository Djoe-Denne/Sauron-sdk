cmake_minimum_required(VERSION 3.15)
project(sauron-example VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find or install nlohmann_json
find_package(nlohmann_json QUIET)
if (NOT nlohmann_json_FOUND)
    include(FetchContent)
    message(STATUS "Fetching nlohmann_json")
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    message(STATUS "Fetching nlohmann_json done")
    FetchContent_MakeAvailable(nlohmann_json)
    message(STATUS "nlohmann_json found")
endif()

# Add executable
add_executable(${PROJECT_NAME}
    main.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../api-spec/cpp-sdk/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../sauron-sdk-curl/include
)

# Link dependencies
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        sauron-sdk
        sauron-sdk-curl
        nlohmann_json::nlohmann_json
)

if(WIN32)
    # Set runtime output directory to be the same for all targets
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
    )

    # Add custom command to copy DLLs post-build
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        # Copy sauron-sdk-curl
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sauron-sdk-curl>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            
        # Copy libcurl from the build directory
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CURL_BINARY_DIR}/lib/$<IF:$<CONFIG:Debug>,libcurl-d.dll,libcurl.dll>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            
        COMMENT "Copying required DLLs to output directory"
    )
endif() 
# TODO(@OopsOverflow): Add a way to handle runtime dependencies for other platforms