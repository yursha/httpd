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

  i32 fd = file_open_to_read_or_die(argv[1], 0);

  char buffer[1024];
  int bytes_read = file_read(fd, (void *) buffer, sizeof(buffer));
  *(buffer + bytes_read) = '\0';
  file_write(1, buffer, bytes_read);
  if (file_close(fd) != 0) {
    file_stderr("Failed to close file\n", 0);
    proc_exit();
  }
}
