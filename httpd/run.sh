#!/usr/bin/env bash
set -e

./build.sh

echo "Starting server on $(hostname -I):7777"
./httpd 7777
