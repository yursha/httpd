#include "../base/type.h"
#include "../base/file.h"
#include "../base/proc.h"
#include "../base/str.h"

i32 main(i32 argc, char** argv, char** envp) {
  if (argc < 3) {
    file_stderr(argv[0], ": please, provide a command and a file path\n", 0);
    proc_exit();
  }

  struct file_info info;

  i32 result = file_info_read(argv[2], &info);
  if (result != 0) {
    file_stderr(argv[0], ": error during stat syscall\n", 0);
    proc_exit();
  }
  if (str_eq("dump_bin", argv[1])) {
    file_write(1, (const char*) &info, sizeof(struct file_info));
  } else if (str_eq("size", argv[1])) {
    char buffer[MAX_64_I_LEN_DECIMAL];
    str_from_i64(info.size, 10, buffer);
    file_stdout(buffer, "\n", 0);
  } else if (str_eq("perms", argv[1])) {
    char buffer[MAX_64_U_LEN_OCTAL];
    str_from_u64(file_info_perm(&info), 8, buffer);
    file_stdout(buffer, "\n", 0);
  } else {
    file_stderr(argv[0], ": unknown command:", argv[1], "\n", 0);
    proc_exit();
  }
}
