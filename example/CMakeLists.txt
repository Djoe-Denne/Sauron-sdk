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