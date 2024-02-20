#include "kernel/types.h"
#include "user/user.h"

int child(int *parfds, int num) {
  if (num >= 35) {
    /* printf("reached 35"); */
    close(parfds[0]);
    close(parfds[1]);
    return 0;
  }
  /* printf("child: num: %d\n", num); */
  int *chfds = malloc(2 * sizeof(int));
  pipe(chfds);
  int chpid = fork();
  int retcode = 0;
  int buf[1];

  if (chpid == 0) {
    close(parfds[0]);
    close(parfds[1]);
    ++num;
    if (num >= 35) {
      close(chfds[0]);
      exit(0);
    }
    retcode = child(chfds, num);
    close(chfds[0]);
    close(chfds[1]);
    exit(retcode);
  } else if (chpid > 0) {
    close(parfds[1]);
    close(chfds[0]);
    int retread = read(parfds[0], buf, sizeof(int));
    if (retread <= 0) {
      /* printf("read failed early, retread: %d\n", retread); */
      /* printf("hey\n"); */
      close(chfds[1]);
      close(parfds[0]);
      free(chfds);
      wait(0);
      exit(0);
    }
    int p = *buf;
    printf("prime %d\n", p);
    retread = read(parfds[0], buf, sizeof(int));
    /* printf("retread: %d\n", retread); */
    while (retread > 0) {
      /* printf("hey\n"); */
      /* printf("buf: %d\n", *buf); */
      if (*buf % p != 0) {
        /* printf("hey\n"); */
        int retwrite = write(chfds[1], buf, sizeof(int));
        /* printf("retwrite: %d\n", retwrite); */
        if (retwrite <= 0) {
          break;
        }
        /* printf("writing %d\n", *buf); */
      }
      retread = read(parfds[0], buf, sizeof(int));
      /* printf("read returned\n"); */
      /* printf("retread: %d\n", retread); */
    }
    close(chfds[1]);
    close(parfds[0]);
    wait(0);
  } else {
    printf("fork failed\n");
    retcode = 1;
  }
  /* printf("hey\n"); */
  free(chfds);
  exit(retcode);
}

int main(int argc, char *argv[]) {
  int retcode = 0;
  int *fds = malloc(2 * sizeof(int));
  pipe(fds);

  int chpid = fork();
  if (chpid == 0) {
    int retcode = child(fds, 2);
    exit(retcode);
  } else if (chpid > 0) {
    close(fds[0]);
    for (int i = 2; i < 35; ++i) {
      int *buf = malloc(sizeof(int));
      buf[0] = i;
      write(fds[1], buf, sizeof(int));
      free(buf);
    }
    close(fds[1]);
    wait(0);
  } else {
    printf("fork failed\n");
    retcode = 1;
  }
  /* printf("hey\n"); */
  free(fds);

  exit(retcode);
}
