#include "file.h"
#include "str.h"

void test_bool(char* name, _Bool result, _Bool expected) {
  if(result != expected) {
    file_write_all(/*fd=*/2, "Test [", name, "] failed\n", 0);
  } else {
    file_write_all(/*fd=*/1, "Test [", name, "] passed\n", 0);
  }
}

void test_str(char* name, const char* result, const char* expected) {
  if(str_eq(result, expected)) {
    file_write_all(/*fd=*/1, "Test [", name, "] passed\n", 0);
  } else {
    file_write_all(/*fd=*/2, "Test [", name, "] failed. Expected [", expected,
          "], actual [", result, "]\n", 0);
  }
}
