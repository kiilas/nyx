#!/bin/bash
compiler="clang"
opts="-std=c99 -Wall -Wextra -pedantic -Werror -Iinclude $@"
mkdir -p build &&
mkdir -p bin/lib &&
$compiler $opts -fPIC -o build/draw.o -c src/draw.c &&
$compiler $opts -fPIC -o build/event.o -c src/event.c &&
$compiler $opts -fPIC -o build/graphics.o -c src/graphics.c &&
$compiler $opts -fPIC -o build/keyboard.o -c src/keyboard.c &&
$compiler $opts -fPIC -o build/layer.o -c src/layer.c &&
$compiler $opts -fPIC -o build/linear.o -c src/linear.c &&
$compiler $opts -fPIC -o build/meta.o -c src/meta.c &&
$compiler $opts -fPIC -o build/pipeline.o -c src/pipeline.c &&
$compiler $opts -fPIC -o build/random.o -c src/random.c &&
$compiler $opts -fPIC -o build/render.o -c src/render.c &&
$compiler $opts -fPIC -o build/texture.o -c src/texture.c &&
$compiler $opts -fPIC -o build/transform.o -c src/transform.c &&
$compiler $opts -fPIC -o build/view.o -c src/view.c &&
$compiler $opts -lSDL2 -lm -o bin/lib/libnyx.so -shared build/draw.o build/event.o build/graphics.o build/keyboard.o build/layer.o build/linear.o build/meta.o build/pipeline.o build/random.o build/render.o build/texture.o build/transform.o build/view.o
