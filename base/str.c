#include "str.h"

#include "file.h"

static i32 count_digits(u64 number, u8 base);

_Bool str_eq(const char* x, const char* y) {
  while (1) {
    if (*x != *y) return 0;
    if (*x == '\0') break;
    x++; y++;
  }
  return 1;
}

i32 str_len(const char* x) {
  const char* y = x;
  while (*y != '\0') y++;
  return y - x;
}

void str_from_i64(i64 number, u8 base, char* buf) {
  if (number < 0) {
    *buf++ = '-';
    number = -number;
  }
  str_from_u64(number, base, buf);
}

void str_from_u64(u64 number, u8 base, char* buf) {
  i32 digit_count = count_digits(number, base);
  *(buf + digit_count) = '\0';

  while (digit_count > 0) {
    i32 digit = number % base;
    *(buf + digit_count - 1) = digit + '0';
    number = number / base;
    digit_count--;
  }
}

/*** PRIVATE ***/

static i32 count_digits(u64 number, u8 base) {
  if (number == 0) return 1;
  i32 count = 0;
  while (number != 0) {
    number = number / base;
    count++;
  }
  return count;
}

