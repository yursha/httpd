#!/usr/bin/env bash
echo "Building httpd"

clang \
  -Werror  `# promote all warnings to errors` \
  -o httpd `# output file name` \
  httpd.c
