#include "file_or_die.h"

#include "file.h"
#include "proc.h"

i32 file_open_to_write_or_die(const char* pathname, i32 flags, u32 mode) {
  i32 fd = file_open_to_write(pathname, flags, mode);
  if (fd == -1) {
    file_stderr("Failed to open file: ", pathname, "\n", 0);
    proc_exit();
  }
  return fd;
}

i32 file_open_to_read_or_die(const char* pathname, i32 flags) {
  i32 fd = file_open_to_read(pathname, flags);
  if (fd == -1) {
    file_stderr("Failed to open file: ", pathname, "\n", 0);
    proc_exit();
  }
  return fd;
}
