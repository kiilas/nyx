#!/bin/bash
compiler="clang"
opts="-Wall -Wextra -pedantic -Wno-logical-op-parentheses -Werror -Iinclude $@"
mkdir -p bin/tools &&
$compiler $opts -lnyx -o bin/tools/font_import tools/font_import.c
$compiler $opts -lnyx -o bin/tools/fontedit tools/fontedit.c
