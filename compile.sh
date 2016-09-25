#!/bin/bash
g++ -std=c++11 -g -Wall -Werror -Isrc/glengine src/main.cc src/glengine/FileReader/MTLFileReader.cc src/glengine/FileReader/OBJFileReader.cc -o bin/main -lglfw -lGLEW -lGL -lpng
