#!/bin/bash
g++ -std=c++14 -Wall -Werror -Isrc/glengine src/main.cc src/glengine/FileReader/OBJFileReader.cc -o bin/main -lglfw -lGLEW -lGL -lpng
