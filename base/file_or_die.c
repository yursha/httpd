#include "file_or_die.h"

#include "file.h"
#include "proc.h"

int file_open_to_write_or_die(const char* pathname, int flags, u32 mode) {
  int fd = file_open_to_write(pathname, flags, mode);
  if (fd == -1) {
    file_stderr("Failed to open file: ", pathname, "\n", 0);
    proc_exit();
  }
  return fd;
}

int file_open_to_read_or_die(const char* pathname, int flags) {
  int fd = file_open_to_read(pathname, flags);
  if (fd == -1) {
    file_stderr("Failed to open file: ", pathname, "\n", 0);
    proc_exit();
  }
  return fd;
}
