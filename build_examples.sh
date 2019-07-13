#!/bin/bash
compiler="clang"
opts="-std=c99 -Wall -Wextra -pedantic -Werror -Iinclude $@"
mkdir -p bin/examples &&
$compiler $opts -Lbin/lib -lnyx -o bin/examples/3dsquares examples/3dsquares.c &&
$compiler $opts -Lbin/lib -lnyx -o bin/examples/rampval examples/rampval.c &&
$compiler $opts -Lbin/lib -lnyx -o bin/examples/rects examples/rects.c &&
$compiler $opts -Lbin/lib -lnyx -o bin/examples/textures examples/textures.c
