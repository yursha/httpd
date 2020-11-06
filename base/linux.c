#include "linux.h"

// This file contains interface with the Linux kernel.
// x86-64 assembly is used for interfacing with the kernel.
// Calling conventions for x86-64 architecture define that
// function parameters are passed consecutively in the
// following registers:
//   rdi,rsi,rdx,rcx,r8,r9
//
// Additional reading:
//   - https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html
//   - https://elixir.bootlin.com/linux/latest/source
//   - arch/x86/syscalls/syscall_64.tbl (kernel src) - for syscall numbers on x86_64
//   - scripts/checksyscalls.sh (kernel src)

//////////////////   FILE OPERATIONS   ////////////////////////

// Open a file descriptor
//
// Creates a new open file description, a new entry in the system-wide table of open
// files. The open file description records the file offset and status flags. A file
// descriptor is a reference to an open file description. This reference is unaffected
// if `path` is subsequently removed or modified to refer to a different file.
//
// Params:
// `dirfd` - file descriptor of a directory, relative to which `path` is interpreted.
// `path`  - file path to open
// `flags` - flags for the operation as described below.
//    Access mode flags: O_WRONLY, ORDONLY, O_RDWR - one and only one must be specified.
//    File creation flag (semantics of opening a file):
//      O_CLOEXEC - enable the close-on-exec.
//      O_CREAT - if `path` doesn't exist create it as a regular file.
//                owner = euid
//                group = egid
//      O_DIRECTORY
//      O_EXCL
//      O_NOCTTY
//      O_NOFOLLOW
//      O_TMPFILE
//      O_TRUNC - truncate file, if exists.
//    File status flags (semantics of subsequent I/O operations):
//      O_APPEND - before each write(2), the file offset is positioned at the end of the
//                 file as if with lseek(2). The modification of the file offset and the
//                 write operation are performed as a single atomic step. Atomicity is not
//                 guaranteed for NFS filesystems.
//    File status flags can be retrieved/modified at any time with fcntl(2).      
// `mode`  -
//
// Returns a file descriptor, a small, nonnegative integer. The file descriptor
// returned by a successful call will be the lowest-numbered file descriptor not
// currently open for the process.
//
// On error returns -1 and sets errno appropriately.
//
// The file offset is set to the beginning of the file.
//
// Subsumed syscalls:
// - open(2)
// - creat(2)
int sys_openat(int dirfd, const char* path, int flags, u32 mode) {
  int res;
  __asm__ volatile (
      "movl    %%ecx, %%r8d\n\t"
      "movl    %%edx, %%ecx\n\t"
      "movq    %%rsi, %%rdx\n\t"
      "movl    %%edi, %%esi\n\t"
      "movl    $257, %%edi\n\t"
      "jmp     syscall"
      : "=a" (res)
  );
  return res;
}

i64 sys_read(int fd, void* buffer, u64 count) {
  i64 res;
  __asm__ volatile (
      "movq    %%rdx, %%rcx\n\t"
      "movq    %%rsi, %%rdx\n\t"
      "movl    %%edi, %%esi\n\t"
      "movl    $0, %%edi\n\t"
      "jmp     syscall"
      : "=a" (res)
  );
  return res;
}

i64 sys_write(int fd, const void* buf, i64 count) {
  i64 res;
  __asm__ volatile (
      "movq    %%rdx, %%rcx\n\t"
      "movq    %%rsi, %%rdx\n\t"
      "movl    %%edi, %%esi\n\t"
      "movl    $1, %%edi\n\t"
      "jmp     syscall"
      : "=a" (res)
  );
  return res;
}

int sys_close(int fd) {
  int res;
  __asm__ volatile (
      "movl    %%edi, %%esi\n\t"
      "movl    $3, %%edi\n\t"
      "jmp     syscall"
      : "=a" (res)
  );
  return res;
}

// ./fs/stat.c
int sys_statx(int dirfd, const char* pathname, int flags, u32 mask, struct file_info* info) {
  int res;
  __asm__ volatile (
      "movq    %%r8, %%r9\n\t"
      "movl    %%ecx, %%r8d\n\t"
      "movl    %%edx, %%ecx\n\t"
      "movq    %%rsi, %%rdx\n\t"
      "movl    %%edi, %%esi\n\t"
      "movl    $332, %%edi\n\t"
      "jmp     syscall"
      : "=a" (res)
  );
  return res;
}

// TODO: sys_lseek
// TODO: sys_fcntl
//       FD_CLOEXEC - off by default
//      O_ASYNC - Enable signal-driven I/O: generate a signal (SIGIO by default, but can
//                be changed with fcntl(2)) when input or output becomes possible on this
//                file. Only applies to terminals, pseudoterminals, sockets, pipes and
//                FIFOs.

//////////////////   PROCESS OPERATIONS   ////////////////////////

i64 sys_exit_group(int status) {
  i64 res;
  __asm__ volatile (
      "movl    %%edi, %%esi\n\t"
      "movl    $231, %%edi\n\t"
      "jmp     syscall"
      : "=a" (res)
  );
  return res;
}

// By default file descriptors remain open across `execve(2)` calls.
// TODO: sys_execve
