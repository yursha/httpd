set -e

mkdir -p build
clang -std=gnu17 -O3 -Wall httpd.c -o build/httpd
