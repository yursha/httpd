#include "proc.h"

#include "linux.h"

void proc_exit() {
  sys_exit_group(1);
}
