#!/bin/bash

# Check if CMAKE_INSTALL_PREFIX is set
if [[ -z "$CMAKE_INSTALL_PREFIX" ]]; then
    export CMAKE_INSTALL_PREFIX="/usr/local"
    echo "CMAKE_INSTALL_PREFIX was not set. Defaulting to: $CMAKE_INSTALL_PREFIX"
else
    echo "CMAKE_INSTALL_PREFIX is already set to: $CMAKE_INSTALL_PREFIX"
fi

# Check if CMAKE_INSTALL_LIBDIR is set
if [[ -z "$CMAKE_INSTALL_LIBDIR" ]]; then
    export CMAKE_INSTALL_LIBDIR="lib"
    echo "CMAKE_INSTALL_LIBDIR was not set. Defaulting to: $CMAKE_INSTALL_LIBDIR"
else
    echo "CMAKE_INSTALL_LIBDIR is already set to: $CMAKE_INSTALL_LIBDIR"
fi
