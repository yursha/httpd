#!/usr/bin/env bash

set -e

echo "Building in debug mode"
clang -g -o httpd httpd.c

echo "Debugging"
gdb --args ./httpd 7777
