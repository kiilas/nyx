#!/bin/bash
compiler="clang"
opts="-Wall -Wextra -pedantic -Wno-logical-op-parentheses -Werror -Iinclude $@"
mkdir -p bin/examples &&
$compiler $opts -lnyx -o bin/examples/3dsquares examples/3dsquares.c &&
$compiler $opts -lnyx -o bin/examples/fonts examples/fonts.c &&
$compiler $opts -lnyx -o bin/examples/rampval examples/rampval.c &&
$compiler $opts -lnyx -o bin/examples/rects examples/rects.c &&
$compiler $opts -lnyx -o bin/examples/textures examples/textures.c
$compiler $opts -lnyx -o bin/examples/view examples/view.c
