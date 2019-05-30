#!/usr/bin/env bash

# build the cmake project
cmake CMakeLists.txt

# build the project files
make

# Execute the ray tracer
./RayTracer.out