#pragma once

/* Open files */

int file_open_to_write_or_die(const char* pathname, int flags, u32 mode);

int file_open_to_read_or_die(const char* pathname, int flags);
