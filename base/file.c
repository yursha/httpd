#include "file.h"

#include "linux.h"
#include "str.h"
#include "proc.h"

static void file_write_all_va_list(int fd, __builtin_va_list args);

/* Open files */

// ASYNC - enable async IO for a file (1,2,3)
//
// TMPFILE (makes 'mode' param useful) (2,3)
//
// DIRECT - best effort to disable caches on read/write
// DSYNC - enable read/write-through behavior (synchronized data integrity)
// SYNC - enable read/write-through behavior (synchronized file integrity)
//
// DIRECTORY - fail is file is not a directory
//
// LARGEFILE - use 64-bit offsets in file descriptions
//
// NOATIME - useful for indexing and backup programs

// NOCTTY
// CLOEXEC
// NONBLOCK
// PATH
//
// NOFOLLOW - don't follow symbolic links
//
#define CWD -100

int file_open_to_read(const char* path, int flags) {
  return sys_openat(CWD, path, /*RDONLY=*/00 | flags, /*unused=*/0);
}

int file_open_to_write(const char* pathname, int flags, u32 mode) {
  return sys_openat(CWD, pathname, /*WRONLY=*/01 | flags, mode);
}

int file_open_to_read_and_write(const char* pathname, int flags, u32 mode) {
  return sys_openat(CWD, pathname, /*RDWR=*/01 | flags, mode);
}

int file_open_to_append(const char* pathname, u32 mode) {
  return sys_openat(CWD, pathname, /*WRONLY=*/01 | FILE_APPEND, mode);
}

/* Read files */

i64 file_read(int fd, void* buffer, u64 count) {
  i64 bytes_read = sys_read(fd, buffer, count);
  if (bytes_read == -1) {
    file_stderr("Error: Can't read from file\n", 0);
    proc_exit();
  }
  return bytes_read;
}

/* Write to files */

void file_stdout(const char* str, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, str);
  int len = str_len(str);
  file_write(1, str, str_len(str));
  file_write_all_va_list(1, args);
  __builtin_va_end(args);
}

void file_stderr(const char* str, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, str);
  int len = str_len(str);
  file_write(2, str, str_len(str));
  file_write_all_va_list(2, args);
  __builtin_va_end(args);
}

// TODO: syscall is performed on every argument.
//       It's better to combine that in one buffer before making a system call.
void file_write_all(int fd, const char* str, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, str);
  int len = str_len(str);
  file_write(fd, str, str_len(str));
  while (str = __builtin_va_arg(args, const char*), str != 0) {
    file_write(fd, str, str_len(str));
  }
  __builtin_va_end(args);
}

void file_write(int fd, const char* str, int len) {
  if (sys_write(fd, str, len) != len) {
    file_stderr("Error: failed to write a string\n", 0);
    proc_exit();
  }
}

/* Close files */

int file_close(int fd) {
  return sys_close(fd);
}

/* inode information */

int file_info_read(const char* pathname, struct file_info* info) {             
  return sys_statx(dirfd, pathname, flags, mask, info);
}

u32 file_info_perm(const struct file_info* info) {
  return info->mode & 0777;
}

/* Read directory */

int file_dir_read(u32 fd, struct dir_entry* buffer, u32 buffer_size) {
}

/******* PRIVATE *******/

static void file_write_all_va_list(int fd, __builtin_va_list args) {
  const char* str;
  while (str = __builtin_va_arg(args, const char*), str != 0) {
    file_write(fd, str, str_len(str));
  }
}
