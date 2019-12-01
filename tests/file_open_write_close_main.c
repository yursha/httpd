#include "../base/type.h"

#include "../base/proc.h"
#include "../base/file.h"
#include "../base/str.h"

#include "../base/file_or_die.h"

i32 main(i32 argc, char** argv, char** envp) {
  if (argc < 2) {
    file_stderr("Please, provide file path\n", 0);
    proc_exit();
  }

  i32 fd = file_open_to_write_or_die(argv[1],
      FILE_CREATE_IF_NOT_EXISTS | FILE_TRUNCATE_IF_EXISTS, 00644);

  const char* message = "Hello world!\n";
  file_write(fd, message, str_len(message));
  if (file_close(fd) != 0) {
    file_stderr("Failed to close file\n", 0);
    proc_exit();
  }
}
