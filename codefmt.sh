#!/bin/sh
# Fix coding style, from Mesa-3D

for f in src/*.c src/*.h examples/*.c examples/*.h;
do
    indent -br -i3 -npcs --no-tabs $f
done
