#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
  int retcode = 0;
  if (argc < 2) {
    char *emsg = "argument is required\n";
    write(2, emsg, strlen(emsg));
    retcode = 1;
  } else {
    int nticks = atoi(argv[1]);
    int retsleep = sleep(nticks);
    if (retsleep < 0) {
      char *emsg = "could not sleep\n";
      write(2, emsg, strlen(emsg));
      retcode = 1;
    }
  }
  exit(retcode);
}

