#!/bin/bash

DIRECTORY=${1:-.}

find $DIRECTORY -name "*.cpp" -o -name "*.h" -o -name "*.c" -o -name "*.hpp" -exec clang-format -i {} \;