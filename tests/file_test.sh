#!/usr/bin/env bash

DIR=$(dirname $0)/..

rm -rf $DIR/tmp
mkdir $DIR/tmp

clang -std=c17 -O3 $DIR/base/linux.c $DIR/base/proc.c $DIR/base/file.c $DIR/base/str.c \
  $DIR/base/test.c $DIR/base/file_or_die.c $DIR/tests/file_open_write_close_main.c -o $DIR/tmp/a.out

$DIR/tmp/a.out $DIR/tmp/file.txt

expected="Hello world!"
actual=$(cat $DIR/tmp/file.txt)
if [ "$actual" = "$expected" ]; then
  echo "Write test passed";
else
  echo "Write test failed, expected [$expected], actual [$actual]";
  exit 1
fi

clang -std=c17 -O3 $DIR/base/linux.c $DIR/base/proc.c $DIR/base/file.c $DIR/base/str.c \
  $DIR/base/test.c $DIR/base/file_or_die.c $DIR/tests/file_open_read_close_main.c -o $DIR/tmp/a.out

actual=$($DIR/tmp/a.out $DIR/tmp/file.txt)
if [ "$actual" = "$expected" ]; then
  echo "Read test passed";
else
  echo "Read test failed, expected [$expected], actual [$actual]";
  exit 1
fi

rm -rf $DIR/tmp
