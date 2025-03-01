# SauronSDKConfig.cmake
# This file is used to find the Sauron SDK when using FetchContent

# Set variables for consumers
set(SAURON_SDK_FOUND TRUE)
set(SAURON_SDK_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include")
set(SAURON_SDK_LIBRARIES sauron_sdk::curl)

# Include the targets file if it exists (for installation)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/sauron-sdk-curl-targets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/sauron-sdk-curl-targets.cmake")
endif() 