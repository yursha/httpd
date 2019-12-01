#!/usr/bin/env bash

clang -std=c11 file.c file_or_die.c linux.c proc.c str.c test.c str_test.c -o str_test
