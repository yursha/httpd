#include "zlib_decompress.h"

#include "file.h"
#include "str.h"

i32 run_verify(i32 argc, char** argv, char** envp) {
  return 0;
}

i32 main(i32 argc, char** argv, char** envp) {
  if (argc < 2) {
    file_write_all(2, "Please, specify command to run\n", 0);
    return 1;
  }

  char* commmand = argv[2];
  if (str_eq(command, "verify")) {
    return run_verify(argc, argv, envp);
  }
  println("Unknown command: ", command);
  return 1;
}
