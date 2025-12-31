#!/bin/bash

rm sand
gcc -o sand main.c -lraylib -lGL -lm -lpthread -ldl -lX11 -lXrandr -lXi
./sand
