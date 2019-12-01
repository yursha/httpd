#include "base/file.h"
#include "base/str.h"
#include "base/type.h"

// https://en.cppreference.com/w/c/language/arithmetic_types
i32 main() {
  char size[22]; // max length of any integer primitive type

  str_from_u64(sizeof(_Bool), size);
  file_write_all(1, "_Bool: ", size, "\n", 0);

  str_from_u64(sizeof(char), size);
  file_write_all(1, "char: ", size, "\n", 0);

  str_from_u64(sizeof(signed char), size);
  file_write_all(1, "signed char: ", size, "\n", 0);

  str_from_u64(sizeof(unsigned char), size);
  file_write_all(1, "unsigned char: ", size, "\n", 0);
  
  str_from_u64(sizeof(short), size);
  file_write_all(1, "short: ", size, "\n", 0);
  
  str_from_u64(sizeof(unsigned short), size);
  file_write_all(1, "unsigned short: ", size, "\n", 0);
  
  str_from_u64(sizeof(int), size);
  file_write_all(1, "int: ", size, "\n", 0);
  
  str_from_u64(sizeof(unsigned int), size);
  file_write_all(1, "unsigned int: ", size, "\n", 0);
  
  str_from_u64(sizeof(long), size);
  file_write_all(1, "long: ", size, "\n", 0);
  
  str_from_u64(sizeof(unsigned long), size);
  file_write_all(1, "unsigned long: ", size, "\n", 0);
  
  str_from_u64(sizeof(long long), size);
  file_write_all(1, "long long: ", size, "\n", 0);
  
  str_from_u64(sizeof(unsigned long long), size);
  file_write_all(1, "unsigned long long: ", size, "\n", 0);
}
