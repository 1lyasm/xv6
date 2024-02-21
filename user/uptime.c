#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int t = uptime();
  if (t > 0) {
    printf("%d\n", t);
  } else {
    fprintf(2, "uptime: uptime syscall failed\n");
    exit(1);
  }
  exit(0);
}

