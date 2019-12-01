#pragma once

#include "type.h"

i32 sys_openat(i32 dirfd, const char* pathname, i32 flags, u32 mode);

i64 sys_read(i32 fd, void* buffer, u64 count);

i64 sys_write(i32 fd, const void* buf, i64 count);

i32 sys_close(i32 fd);

struct file_info {
  u64 hosting_device_id; // on which this file resides
  u64 inode;
  u64 hardlink_count;
  u32 mode; // file type and mode
  u32 user_id;
  u32 group_id;
  i32 __pad0;
  u64 represented_device_id; // that this file represents if a special file
  i64 size; // in bytes
  i64 block_size; // optimal block size for file system IO
  i64 block_count; // number of 512-byte blocks allocated
  i64 acess_ts_sec;
  u64 acess_ts_nsec;
  i64 modifed_ts_sec; // file contents change
  u64 modifed_ts_nsec;
  i64 changed_ts_sec; // inode metadata change
  u64 changed_ts_nsec;
  i64 __reserved[3];
};

i32 sys_statx(i32 dirfd, const char* pathname, i32 flags, u32 mask, struct file_info* info);

i64 sys_exit_group(i32 status);

