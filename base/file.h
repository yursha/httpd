#pragma once

#include "type.h"
#include "linux.h"

/* Open files */

// See /usr/include/x86_64-linux-gnu/bits/fcntl-linux.h for flags
#define FILE_CREATE_IF_NOT_EXISTS /*CREAT=*/0100         // FOR WRONLY & RDWR
#define FILE_FAIL_IF_EXISTS /*CREAT=*/0100|/*EXCL=*/0200 // FOR WRONLY & RDWR
#define FILE_APPEND /*CREAT=*/0100|/*APPEND=*/02000      // FOR WRONLY & RDWR (local files only)
#define FILE_TRUNCATE_IF_EXISTS /*TRUNC=*/01000          // FOR WRONLY & RDWR

i32 file_open_to_read(const char* pathname, i32 flags);

i32 file_open_to_write(const char* pathname, i32 flags, u32 mode);

i32 file_open_to_read_and_write(const char* pathname, i32 flags, u32 mode);

i32 file_open_to_append(const char* pathname, u32 mode);

/* Read files */

i64 file_read(i32 fd, void* buffer, u64 count);

/* Write to files */

void file_stdout(const char* str, ...);

void file_stderr(const char* str, ...);

void file_write_all(i32 fd, const char* str, ...);

void file_write(i32 fd, const char* str, i32 len);

/* Close files */

i32 file_close(i32 fd);

/* inode information */

// man 1 stat
// man 2 stat
// man 7 inode

i32 file_info_read(const char* pathname, struct file_info* info);

u32 file_info_perm(const struct file_info* info);

/* Read directory */

// i32 file_dir_read(u32 fd, struct dir_info* info, u32 count);

/* Walk file system */

// man 3 ftw
// man 3 readdir
// man 2 getdents

/*
void fn(const char* path, j


void file_walk(const char* path, fn);
  // file_info_read(path);
  // for each entry in path
  //   if directory recurse
  //   else fn on it
  //   */
