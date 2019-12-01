#!/usr/bin/env bash

DIR=$(dirname $0)/..

rm -rf $DIR/tmp
mkdir $DIR/tmp

clang -std=c17 -O3 $DIR/base/linux.c $DIR/base/proc.c $DIR/base/file.c $DIR/base/str.c \
  $DIR/base/test.c $DIR/base/file_or_die.c $DIR/tests/stat_main.c -o $DIR/tmp/a.out

echo "Hello world!" > $DIR/tmp/file.txt

expected=$(stat --printf="%s" $DIR/tmp/file.txt)
actual=$($DIR/tmp/a.out size $DIR/tmp/file.txt)

if [ "$actual" = "$expected" ]; then
  echo "Stat size test passed";
else
  echo "Stat size test failed, expected [$expected], actual [$actual]";
  exit 1
fi

expected=$(stat --printf="%a" $DIR/tmp/file.txt)
actual=$($DIR/tmp/a.out perms $DIR/tmp/file.txt)

if [ "$actual" = "$expected" ]; then
  echo "Stat perms test passed";
else
  echo "Stat perms test failed, expected [$expected], actual [$actual]";
  exit 1
fi

rm -rf $DIR/tmp
