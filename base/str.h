#pragma once

#include "type.h"

i32 str_len(const char* x);

_Bool str_eq(const char* x, const char* y);

// `buf` must be big enough to contain converted `number`.
// 21 bytes should be enough for all cases.
#define MAX_64_I_LEN_DECIMAL 21
#define MAX_64_U_LEN_OCTAL 22
void str_from_i64(i64 number, u8 base, char* buf);
void str_from_u64(u64 number, u8 base, char* buf);

