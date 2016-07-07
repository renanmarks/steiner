#!/bin/sh

cd ..
mkdir steiner-build-release
cd steiner-build-release
cmake ../steiner -DCMAKE_BUILD_TYPE=Release
make
