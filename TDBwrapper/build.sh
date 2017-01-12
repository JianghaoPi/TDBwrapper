#!/usr/bin/env bash
g++ util.cpp -fPIC -L. -lTDBAPI -o libutil.so

g++ -shared -fpic -lm -ldl -o libutil.so util.cpp