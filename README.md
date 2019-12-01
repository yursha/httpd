# Dependency layers

This code is broken down into dependency layers.
A module in level N can only depend on modules in level N - 1 or below.
Exception is the 'type' module, upon which any module can depend.

## Level 0

- type

## Level 1

- linux

## Level 2

- file
- proc
- str


# Namespacing

All functions in a module should be prefixed with the basename of the containing file.

# Trade-offs

During the course of development of this library, I learned that it is impossible to achieve
best performance without increasing API surface to cover all special cases.
Big API surface leads to code complexity.
There will always be some performance lost at the API boundary.
This library makes the following performance tradeoffs in the name of consistency and smaller
API surface:

- All functions which convert numbers to strings generate well-formed C-style strings (with '\0' at the end).
