# FindSauronSDK.cmake
# This module finds the Sauron SDK and sets the following variables:
#  SAURON_SDK_FOUND        - True if Sauron SDK was found
#  SAURON_SDK_INCLUDE_DIRS - Include directories for Sauron SDK
#  SAURON_SDK_LIBRARIES    - Libraries to link against

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

# Not found
set(SAURON_SDK_FOUND FALSE) 