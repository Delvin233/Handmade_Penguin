#!/bin/bash
#
mkdir -p ../build
pushd ../build
# c++ ../code/sdl_handmade.cpp -o handmadehero -g "$(pkg-config --cflags --libs sdl3)"
c++ ../code/sdl_handmade_sdl2.cpp -o handmadehero -g $(sdl2-config --cflags --libs)
popd
