#!/bin/bash
#
mkdir -p ../build
pushd ../build
c++ ../code/sdl_handmade.cpp -o handmadehero -g "$(pkg-config --cflags --libs sdl3)"
popd
