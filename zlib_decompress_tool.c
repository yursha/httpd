#include "zlib_decompress.h"

#include "file.h"
#include "str.h"

i32 run_verify(i32 argc, char** argv, char** envp) {
  if (argc < 3) {
    file_write_all(2, "Please, specify file or directory path to verify\n", 0);
    return 1;
  }
  file_write_all(2, "Verifying\n", 0);
  const char* filepath = argv[2];
  // if file - verify
  // if directory - recursively walk and if file verify

  return 0;
}

i32 main(i32 argc, char** argv, char** envp) {
  if (argc < 2) {
    file_write_all(2, "Please, specify command to run\n", 0);
    return 1;
  }

  char* command = argv[1];
  if (str_eq(command, "verify")) {
    return run_verify(argc, argv, envp);
  }
  file_write_all(2, "Unknown command: ", command, "\n", 0);
  return 1;
}
