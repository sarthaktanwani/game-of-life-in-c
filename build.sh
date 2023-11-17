#!/bin/sh

set -xe

clang -O3 -Wall -Wextra -o gol gol.c -lm
# clang -O3 -Wall -Wextra `pkg-config --cflags raylib` -o smoothlife_gpu smoothlife_gpu.c -lm `pkg-config --libs raylib` -ldl -lglfw -lpthread
