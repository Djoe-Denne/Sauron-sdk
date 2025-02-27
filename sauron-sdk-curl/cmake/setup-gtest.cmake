include(FetchContent)

# Option to control automatic GTest installation
option(AUTO_INSTALL_GTEST "Automatically install Google Test if not found" ON)

# First try to find GTest in the system
find_package(GTest QUIET)

if(NOT GTEST_FOUND AND AUTO_INSTALL_GTEST)
    message(STATUS "GTest not found. Setting up GTest via FetchContent...")
    
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
    )
    
    # Windows-specific settings
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    
    FetchContent_MakeAvailable(googletest)
    
    # Set variables to match find_package behavior
    set(GTEST_FOUND TRUE)
    set(GTEST_INCLUDE_DIRS ${gtest_SOURCE_DIR}/include)
    set(GMOCK_INCLUDE_DIRS ${gmock_SOURCE_DIR}/include)
    set(GTEST_LIBRARIES gtest)
    set(GTEST_MAIN_LIBRARIES gtest_main)
    set(GMOCK_LIBRARIES gmock)
    set(GMOCK_MAIN_LIBRARIES gmock_main)
elseif(NOT GTEST_FOUND AND NOT AUTO_INSTALL_GTEST)
    message(WARNING "GTest not found and AUTO_INSTALL_GTEST is OFF. Tests will be disabled.")
    return()
endif()

# Helper function to setup testing for a target
function(setup_target_testing TARGET_NAME)
    if(NOT GTEST_FOUND)
        return()
    endif()
    
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            ${GTEST_LIBRARIES}
            ${GTEST_MAIN_LIBRARIES}
            ${GMOCK_LIBRARIES}
            ${GMOCK_MAIN_LIBRARIES}
    )
    
    target_include_directories(${TARGET_NAME}
        PRIVATE
            ${GTEST_INCLUDE_DIRS}
            ${GMOCK_INCLUDE_DIRS}
    )
endfunction() 