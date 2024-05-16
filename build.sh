#!/bin/bash
gcc -fno-inline -O0 -g test.c patcher.c patcher.h -o test 