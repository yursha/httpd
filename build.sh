#!/usr/bin/env bash

DIR=$(dirname $0)

MAIN=$1

echo "Building $MAIN"

rm -rf $DIR/build
mkdir $DIR/build

clang -std=c17 $FLAGS -O3 $DIR/src/base/linux.c $DIR/src/base/proc.c $DIR/src/base/file.c \
  $DIR/src/base/str.c $DIR/src/base/test.c $DIR/src/base/file_or_die.c $DIR/$MAIN -o $DIR/build/a.out
