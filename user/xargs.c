#include "kernel/types.h"
#include "user/user.h"

#define BUFLEN 256

char *rdline() {
  char *buf = malloc((BUFLEN + 1) * sizeof(char));
  buf[BUFLEN] = 0;
  int idx = 0;
  /* char cur; */
  int must_exit = 0;
  do {
    int nread = read(0, buf + idx, 1);
    if (nread <= 0) {
      free(buf);
      buf = 0;
      must_exit = 1;
    }
    if (*(buf + idx) == '\n') {
      *(buf + idx) = 0;
      must_exit = 1;
    }
    ++idx;
  } while (must_exit == 0);
  return buf;
}

char *rdword(char *line, int *idx) {
  int len = strlen(line);
  char *buf = malloc((len + 1) * sizeof(char));
  buf[len] = 0;
  int bufidx = 0;
  for (; line[*idx] == ' ' || line[*idx] == '\t' || line[*idx] == 0; ++(*idx))
    ;
  for (; line[*idx] != ' ' && line[*idx] != '\t' && line[*idx] != 0; ++(*idx)) {
    buf[bufidx] = line[*idx];
    ++bufidx;
  }
  return buf;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "xargs: at least 1 argument is required\n");
    exit(1);
  }
  char *line;
  int nch = 0;
  do {
    line = rdline();
    if (line != 0) {
      int chpid = fork();
      if (chpid == 0) {
        /* printf("hey\n"); */
        int nws = 0;
        int i = 0;
        for (; line[i] != 0; ++i) {
          if (line[i] == ' ' || line[i] == '\t') {
            ++nws;
            for (; line[i] == ' ' || line[i] == '\t'; ++i)
              ;
          }
        }
        /* printf("hey\n"); */
        /* printf("nws: %d\n", nws); */
        /* printf("argc + nws + 1: %d\n", argc + nws + 1); */
        char **argvcp = malloc((argc + nws + 2) * sizeof(char *));
        argvcp[argc + nws + 1] = 0;
        for (int i = 0; i < argc; ++i) {
          int len = strlen(argv[i]);
          argvcp[i] = malloc((len + 1) * sizeof(char));
          argvcp[i][len] = 0;
          strcpy(argvcp[i], argv[i]);
          /* printf("argvcp[%d]: %s\n", i, argvcp[i]); */
        }
        /* printf("hey\n"); */
        int idx = 0;
        for (int i = argc; i < argc + nws + 1; ++i) {
          argvcp[i] = rdword(line, &idx);
          /* printf("argvcp[%d]: %s\n", i, argvcp[i]); */
          /* printf("hey\n"); */
        }
        /* printf("hey\n"); */
        for (int i = 0; i < argc + nws + 1; ++i) {
          /* printf("%s\n", argvcp[i]); */
        }
        /* printf("argvcp[1]: %s\n", argvcp[1]); */
        exec(argvcp[1], &(argvcp[1]));
        fprintf(2, "xargs: exec failed\n");
        exit(1);
      } else if (chpid > 0) {

      } else {
        fprintf(2, "xargs: fork failed\n");
        exit(1);
      }
      ++nch;
      free(line);
    }
  } while (line != 0);
  for (int i = 0; i < nch; ++i) {
    wait(0);
  }
  exit(0);
}
