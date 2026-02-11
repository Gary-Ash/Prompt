#!/usr/bin/env bash
#*****************************************************************************************
# build-libgit2.sh
#
# Build libgit2 as a static library
#
# Author   :  Gary Ash <gary.ash@icloud.com>
# Created  :   2-Feb-2026  4:30pm
# Modified :
#
# Copyright © 2026 By Gary Ash All rights reserved.
#*****************************************************************************************
set -e
cd "$(dirname "$0")"
cd ..
git clone --recurse-submodules --single-branch https://github.com/libgit2/libgit2.git
export PKG_CONFIG_PATH=/usr/local/opt/openssl/lib/pkgconfig
cd libgit2
mkdir build
cd build

OSXARCHITECTURE="x86_64;arm64"

cmake -DBUILD_SHARED_LIBS:BOOL=OFF -DCMAKE_OSX_ARCHITECTURES=$OSXARCHITECTURE ..
cmake --build .
