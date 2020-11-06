#!/bin/bash
g++ -O3 main.cpp Shader.cpp GLApp.cpp Matrix4.cpp BufferAllocator.cpp -lglfw -lGL -lGLEW
