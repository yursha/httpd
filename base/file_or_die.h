#pragma once

#include "type.h"

/* Open files */

i32 file_open_to_write_or_die(const char* pathname, i32 flags, u32 mode);

i32 file_open_to_read_or_die(const char* pathname, i32 flags);
