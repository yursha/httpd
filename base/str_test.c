#include "str.h"

#include "test.h"

i32 main(i32 argc, char** argv, char** envp) {
  test_bool("str_eq: x empty, y non-empty", str_eq("", "abc"), 0);
  test_bool("str_eq: x non-empty, y empty", str_eq("abc", ""), 0);
  test_bool("str_eq: both empty", str_eq("", ""), 1);
  test_bool("str_eq: both equal", str_eq("abc", "abc"), 1);
  test_bool("str_eq: x shorter", str_eq("a", "ab"), 0);
  test_bool("str_eq: x longer", str_eq("ab", "a"), 0);
  test_bool("str_eq: same length, but different", str_eq("ac", "ab"), 0);


  char buf[MAX_64_I_LEN];
  str_from_u64(0U, buf);
  test_str("str_from_u64: 0", buf, "0");
  str_from_u64(1234U, buf);
  test_str("str_from_u64: 1234", buf, "1234");
  str_from_u64(18446744073709551615U, buf);
  test_str("str_from_u64: max", buf, "18446744073709551615");

  str_from_i64(-9223372036854775807, buf);
  test_str("str_from_i64: min", buf, "-9223372036854775807");
  str_from_i64(-1234, buf);
  test_str("str_from_i64: -1234", buf, "-1234");
  str_from_i64(0, buf);
  test_str("str_from_i64: 0", buf, "0");
  str_from_i64(1234, buf);
  test_str("str_from_i64: 1234", buf, "1234");
  str_from_i64(9223372036854775807, buf);
  test_str("str_from_i64: max", buf, "9223372036854775807");
  
  return 0;
}
