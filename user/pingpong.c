#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int retcode = 0;

  int fds[2];
  pipe(fds);

  char *msg = "a";
  char buf[64];

  int chpid = fork();
  if (chpid == 0) {
    int pid = getpid();
    read(fds[1], buf, 1);
    printf("%d: received ping\n", pid);
    write(fds[1], msg, strlen(msg));
    close(fds[1]);
  } else if (chpid > 0) {
    int pid = getpid();
    write(fds[0], msg, strlen(msg));
    read(fds[0], buf, 1);
    printf("%d: received pong\n", pid);
    close(fds[0]);
    wait(0);
  } else {
    printf("fork failed\n");
    retcode = 1;
  }

  exit(retcode);
}
